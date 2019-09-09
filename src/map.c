/*
 * This file is part of libls.
 *
 * Copyright (c) 2017-2018 Ikey Doherty
 * Copyright (c) 2019 Lispy Snake, Ltd.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "map.h"

typedef struct LsHashmapNode LsHashmapNode;

/**
 * Initial size of 256 items. Slight overcommit but prevents too much future
 * growth as our growth ratio and algorithm is ^2 based.
 */
#define LS_HASH_INITIAL_SIZE 256

/**
 * 60% = full hashmap from our perspective.
 */
#define LS_HASH_FILL_RATE 0.6

/**
 * Grow by a factor of 4, first regrowth takes us to 512, then 2048, etc.
 * This helps with distribution and maintains ^2 constraint.
 */
#define LS_HASH_GROWTH 4

/**
 * Construct a new internal hashmap from the given hashmap and copy
 * all the relevant components.
 */
static void ls_hashmap_from(LsHashmap *map, LsHashmap *target);
static bool ls_hashmap_resize(LsHashmap *self);
static bool ls_hashmap_insert_map(LsHashmap *self, const uint32_t hash, void *key, void *value);
static LsHashmapNode *ls_hashmap_get_node(LsHashmap *self, void *key);

/**
 * Opaque LsHashmap implementation, simply an organised header for the
 * function pointers, state and buckets.
 */
struct LsHashmap {
        struct {
                LsHashmapNode *blob;      /**<Contiguous blob of buckets, over-commits */
                unsigned int max;         /**<How many buckets are currently allocated? */
                unsigned int current;     /**<How many items do we currently have? */
                unsigned int mask;        /**< pow2 n_buckets - 1 */
                unsigned int next_resize; /**<At what point do we perform resize? */
        } buckets;
        struct {
                ls_hashmap_hash_func hash;     /**<Key hash generator */
                ls_hashmap_equal_func compare; /**<Key value comparison */
        } key;
        struct {
                ls_hashmap_free_func key;   /**<Key free function */
                ls_hashmap_free_func value; /**<Value free function */
        } free;
};

/**
 * A LsHashmapNode is simply a single bucket within a LsHashmap and can have
 * a key/value. This is a chained mechanism.
 */
struct LsHashmapNode {
        void *key;
        void *value;
        struct LsHashmapNode *next;
        uint32_t hash;
};

LsHashmap *ls_hashmap_new(ls_hashmap_hash_func hash, ls_hashmap_equal_func compare)
{
        return ls_hashmap_new_full(hash, compare, NULL, NULL);
}

LsHashmap *ls_hashmap_new_full(ls_hashmap_hash_func hash, ls_hashmap_equal_func compare,
                               ls_hashmap_free_func key_free, ls_hashmap_free_func value_free)
{
        LsHashmap *ret = NULL;

        LsHashmap clone = {
                .key.hash = hash,
                .key.compare = compare,
                .free.key = key_free,
                .free.value = value_free,
                .buckets.blob = NULL,
                .buckets.current = 0,
                .buckets.max = LS_HASH_INITIAL_SIZE,
                .buckets.mask = LS_HASH_INITIAL_SIZE - 1,
                .buckets.next_resize = (int)(((double)LS_HASH_INITIAL_SIZE) * LS_HASH_FILL_RATE),
        };

        /* Some things we actually do need, sorry programmer. */
        assert(clone.key.hash);
        assert(clone.key.compare);

        ret = calloc(1, sizeof(struct LsHashmap));
        if (!ret) {
                return NULL;
        }
        *ret = clone;

        ret->buckets.blob = calloc((size_t)clone.buckets.max, sizeof(struct LsHashmapNode));
        if (!ret->buckets.blob) {
                ls_hashmap_free(ret);
                return NULL;
        }

        return ret;
}

static inline void bucket_free_one(LsHashmap *self, LsHashmapNode *node)
{
        if (self->free.key) {
                self->free.key(node->key);
        }
        if (self->free.value) {
                self->free.value(node->value);
        }
}

static inline void bucket_free(LsHashmap *self, LsHashmapNode *node, bool free_values)
{
        if (!node) {
                return;
        }
        bucket_free(self, node->next, free_values);
        if (free_values) {
                bucket_free_one(self, node);
        }
        free(node);
}

static void ls_hashmap_free_internal(LsHashmap *self, bool free_blobs)
{
        for (size_t i = 0; i < self->buckets.max; i++) {
                LsHashmapNode *node = &self->buckets.blob[i];
                if (free_blobs) {
                        bucket_free_one(self, node);
                }
                bucket_free(self, node->next, free_blobs);
        }
        free(self->buckets.blob);
}

void ls_hashmap_free(LsHashmap *self)
{
        if (ls_unlikely(!self)) {
                return;
        }
        ls_hashmap_free_internal(self, true);
        free(self);
        return;
}

bool ls_hashmap_simple_equal(const void *a, const void *b)
{
        return a == b;
}

uint32_t ls_hashmap_simple_hash(const void *v)
{
        /* because NULL == 0 - we always increment the hash by one.
         * this ensures no special handling is required for "is a hash set
         */
        return LS_PTR_TO_INT(v) + 1;
}

bool ls_hashmap_string_equal(const void *a, const void *b)
{
        if (!a || !b) {
                return false;
        }
        return strcmp((const char *)a, (const char *)b) == 0;
}

/**
 * Currently this is just a version of the well known DJB hash so that
 * I can do some direct fair comparisons with the old libnica hashmap
 * that isn't skewed by the hash being different.
 *
 * This will be replaced by a more efficient version soon.
 */
uint32_t ls_hashmap_string_hash(const void *v)
{
        unsigned int hash = 5381;
        const signed char *c;

        for (c = v; *c != '\0'; c++) {
                hash = (hash << 5) + hash + (unsigned)*c;
        }

        return (uint32_t)hash;
}

/**
 * Find the base bucket to work from
 */
static inline LsHashmapNode *ls_hashmap_initial_bucket(LsHashmap *self, const uint32_t hash)
{
        return &self->buckets.blob[hash & self->buckets.mask];
}

/**
 * Internal insert helper, will never attempt a resize, as that is only handled
 * by the public API.
 */
static bool ls_hashmap_insert_map(LsHashmap *self, const uint32_t hash, void *key, void *value)
{
        LsHashmapNode *bucket = NULL;
        LsHashmapNode *candidate = NULL;

        /* Cheat for now. Soon, we need to handle collisions */
        bucket = ls_hashmap_initial_bucket(self, hash);

        for (LsHashmapNode *node = bucket; node; node = node->next) {
                /* Root node isn't occupied */
                if (node->hash == 0) {
                        /* We have more buckets used now */
                        self->buckets.current++;
                        node->hash = hash;
                        node->key = key;
                        node->value = value;
                        return true;
                }

                /* Attempt to find dupe */
                if (ls_unlikely(self->key.compare(node->key, key))) {
                        if (ls_likely(self->free.key != NULL)) {
                                self->free.key(node->key);
                        }
                        if (ls_likely(self->free.value != NULL)) {
                                self->free.value(node->value);
                        }
                        node->hash = hash;
                        node->key = key;
                        node->value = value;
                        return true;
                }
        }

        /* Construct a new input node */
        candidate = calloc(1, sizeof(LsHashmapNode));
        if (!candidate) {
                return false;
        }

        /* Prepend and balance leaf */
        candidate->next = bucket->next;
        bucket->next = candidate;
        self->buckets.current++;
        candidate->hash = hash;
        candidate->key = key;
        candidate->value = value;

        return true;
}

bool ls_hashmap_put(LsHashmap *self, void *key, void *value)
{
        uint32_t hash;

        if (ls_unlikely(!self)) {
                return false;
        }

        /* Check if we need a resize before the insert */
        if (!ls_hashmap_resize(self)) {
                return false;
        }

        hash = self->key.hash(key);

        /* Ensure we have at least key *and* value together */
        if (ls_unlikely(!key && !value)) {
                return true;
        }

        return ls_hashmap_insert_map(self, hash, key, value);
}

/**
 * Find the parent node for a key and return it
 */
static LsHashmapNode *ls_hashmap_get_node(LsHashmap *self, void *key)
{
        LsHashmapNode *bucket = NULL;
        uint32_t hash;

        hash = self->key.hash(key);
        bucket = ls_hashmap_initial_bucket(self, hash);

        for (LsHashmapNode *node = bucket; node; node = node->next) {
                if (node->hash != 0 && self->key.compare(node->key, key)) {
                        return node;
                }
        }

        return NULL;
}

void *ls_hashmap_get(LsHashmap *self, void *key)
{
        LsHashmapNode *node = NULL;

        if (ls_unlikely(!self)) {
                return NULL;
        }

        node = ls_hashmap_get_node(self, key);
        if (ls_unlikely(!node)) {
                return NULL;
        }
        return node->value;
}

static void ls_hashmap_from(LsHashmap *source, LsHashmap *target)
{
        *target = *source;
        memset(&target->buckets, 0, sizeof(target->buckets));
}

/**
 * Check if our current count is at the resize count, and start our
 * resize if at all possible.
 */
static bool ls_hashmap_resize(LsHashmap *self)
{
        LsHashmap target = { 0 };

        /* Continue unimpeded */
        if (ls_likely(self->buckets.next_resize != self->buckets.current)) {
                return true;
        }

        /* Set up the target from the source and bind up new blobs.. */
        ls_hashmap_from(self, &target);
        target.buckets.max = LS_HASH_GROWTH * self->buckets.max;
        target.buckets.mask = target.buckets.max - 1;
        target.buckets.next_resize =
            (unsigned int)(((double)target.buckets.max) * LS_HASH_FILL_RATE),
        target.buckets.blob = calloc(target.buckets.max, sizeof(struct LsHashmapNode));
        if (ls_unlikely(!target.buckets.blob)) {
                return false;
        }

        /* Start moving everything across and preserve the hash (no need to rehash) */
        for (unsigned int i = 0; i < self->buckets.max; i++) {
                for (LsHashmapNode *node = &self->buckets.blob[i]; node; node = node->next) {
                        uint32_t hash = node->hash;

                        if (ls_unlikely(hash == 0)) {
                                continue;
                        }

                        if (!ls_hashmap_insert_map(&target, hash, node->key, node->value)) {
                                goto failed;
                        }
                }
        }

        /* Woot, we won */
        ls_hashmap_free_internal(self, false);
        *self = target;

        return true;
failed:
        ls_hashmap_free_internal(&target, false);
        return false;
}

bool ls_hashmap_remove(LsHashmap *self, void *key)
{
        LsHashmapNode *node = NULL;

        if (ls_unlikely(!self)) {
                return false;
        }

        node = ls_hashmap_get_node(self, key);
        if (ls_unlikely(!node)) {
                return false;
        }

        if (ls_likely(self->free.key != NULL)) {
                self->free.key(node->key);
        }
        if (ls_likely(self->free.value != NULL)) {
                self->free.value(node->value);
        }

        node->key = NULL;
        node->value = NULL;
        /* Ensure we reset hash so that we can reclaim this guy */
        node->hash = 0;

        return true;
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 8
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=8 tabstop=8 expandtab:
 * :indentSize=8:tabSize=8:noTabs=true:
 */
