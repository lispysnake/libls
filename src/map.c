/*
 * This file is part of libuf.
 *
 * Copyright © 2017 Ikey Doherty
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "util.h"

typedef struct UfHashmapNode UfHashmapNode;

/**
 * Initial size of 128 items. Slight overcommit but prevents too much future
 * growth as our growth ratio and algorithm is ^2 based.
 */
#define UF_HASH_INITIAL_SIZE 128

/**
 * 60% = full hashmap from our perspective.
 */
#define UF_HASH_FILL_RATE 0.6

/**
 * Grow by a factor of 4, first regrowth takes us to 512, then 2048, etc.
 * This helps with distribution and maintains ^2 constraint.
 */
#define UF_HASH_GROWTH 4

/**
 * Construct a new internal hashmap from the given hashmap and copy
 * all the relevant components.
 */
static void uf_hashmap_from(UfHashmap *map, UfHashmap *target);
static bool uf_hashmap_resize(UfHashmap *self);
static bool uf_hashmap_insert_map(UfHashmap *self, uint32_t hash, void *key, void *value);
static UfHashmapNode *uf_hashmap_get_node(UfHashmap *self, void *key);

/**
 * Opaque UfHashmap implementation, simply an organised header for the
 * function pointers, state and buckets.
 */
struct UfHashmap {
        struct {
                UfHashmapNode *blob;      /**<Contiguous blob of buckets, over-commits */
                unsigned int max;         /**<How many buckets are currently allocated? */
                unsigned int current;     /**<How many items do we currently have? */
                unsigned int mask;        /**< pow2 n_buckets - 1 */
                unsigned int next_resize; /**<At what point do we perform resize? */
        } buckets;
        struct {
                uf_hashmap_hash_func hash;     /**<Key hash generator */
                uf_hashmap_equal_func compare; /**<Key value comparison */
        } key;
        struct {
                uf_hashmap_free_func key;   /**<Key free function */
                uf_hashmap_free_func value; /**<Value free function */
        } free;
};

/**
 * A UfHashmapNode is simply a single bucket within a UfHashmap and can have
 * a key/value. This is a chained mechanism.
 */
struct UfHashmapNode {
        void *key;
        void *value;
        struct UfHashmapNode *next;
        uint32_t hash;
};

UfHashmap *uf_hashmap_new(uf_hashmap_hash_func hash, uf_hashmap_equal_func compare)
{
        return uf_hashmap_new_full(hash, compare, NULL, NULL);
}

UfHashmap *uf_hashmap_new_full(uf_hashmap_hash_func hash, uf_hashmap_equal_func compare,
                               uf_hashmap_free_func key_free, uf_hashmap_free_func value_free)
{
        UfHashmap *ret = NULL;

        UfHashmap clone = {
                .key.hash = hash,
                .key.compare = compare,
                .free.key = key_free,
                .free.value = value_free,
                .buckets.blob = NULL,
                .buckets.current = 0,
                .buckets.max = UF_HASH_INITIAL_SIZE,
                .buckets.mask = UF_HASH_INITIAL_SIZE - 1,
                .buckets.next_resize = (int)(((double)UF_HASH_INITIAL_SIZE) * UF_HASH_FILL_RATE),
        };

        /* Some things we actually do need, sorry programmer. */
        assert(clone.key.hash);
        assert(clone.key.compare);

        ret = calloc(1, sizeof(struct UfHashmap));
        if (!ret) {
                return NULL;
        }
        *ret = clone;

        ret->buckets.blob = calloc((size_t)clone.buckets.max, sizeof(struct UfHashmapNode));
        if (!ret->buckets.blob) {
                uf_hashmap_free(ret);
                return NULL;
        }

        return ret;
}

static inline void bucket_free(UfHashmap *self, UfHashmapNode *node, bool free_values)
{
        if (!node) {
                return;
        }
        bucket_free(self, node->next, free_values);
        if (!free_values) {
                goto node_free;
        }
        if (self->free.key) {
                self->free.key(node->key);
        }
        if (self->free.value) {
                self->free.value(node->value);
        }
node_free:
        free(node);
}

static void uf_hashmap_free_internal(UfHashmap *self, bool free_blobs)
{
        for (size_t i = 0; i < self->buckets.max; i++) {
                UfHashmapNode *node = &self->buckets.blob[i];
                bucket_free(self, node->next, free_blobs);
        }
        free(self->buckets.blob);
}

void uf_hashmap_free(UfHashmap *self)
{
        if (uf_unlikely(!self)) {
                return;
        }
        uf_hashmap_free_internal(self, true);
        free(self);
        return;
}

bool uf_hashmap_simple_equal(const void *a, const void *b)
{
        return a == b;
}

uint32_t uf_hashmap_simple_hash(const void *v)
{
        /* because NULL == 0 - we always increment the hash by one.
         * this ensures no special handling is required for "is a hash set
         */
        return UF_PTR_TO_INT(v) + 1;
}

/**
 * Find the base bucket to work from
 */
static inline UfHashmapNode *uf_hashmap_initial_bucket(UfHashmap *self, uint32_t hash)
{
        return &self->buckets.blob[hash & self->buckets.mask];
}

/**
 * Internal insert helper, will never attempt a resize, as that is only handled
 * by the public API.
 */
static bool uf_hashmap_insert_map(UfHashmap *self, uint32_t hash, void *key, void *value)
{
        UfHashmapNode *bucket = NULL;
        UfHashmapNode *candidate = NULL;

        /* Cheat for now. Soon, we need to handle collisions */
        bucket = uf_hashmap_initial_bucket(self, hash);

        for (UfHashmapNode *node = bucket; node; node = node->next) {
                /* Root node isn't occupied */
                if (node->hash == 0) {
                        candidate = node;
                        /* We have more buckets used now */
                        ++self->buckets.current;
                        break;
                }

                /* Attempt to find dupe */
                if (uf_unlikely(self->key.compare(node->key, key))) {
                        candidate = node;
                        /* Replacing a bucket, no diff in count */
                        break;
                }
        }

        /* Displace an existing node */
        if (uf_unlikely(candidate != NULL)) {
                if (uf_likely(self->free.key != NULL)) {
                        self->free.key(candidate->key);
                }
                if (uf_likely(self->free.value != NULL)) {
                        self->free.value(candidate->value);
                }
                goto insert_bucket;
        }

        /* Construct a new input node */
        candidate = calloc(1, sizeof(UfHashmapNode));
        if (!candidate) {
                return false;
        }

        /* Prepend and balance leaf */
        candidate->next = bucket->next;
        bucket->next = candidate;
        ++self->buckets.current;

insert_bucket:
        candidate->hash = hash;
        candidate->key = key;
        candidate->value = value;

        return true;
}

bool uf_hashmap_put(UfHashmap *self, void *key, void *value)
{
        uint32_t hash;

        if (uf_unlikely(!self)) {
                return false;
        }

        /* Check if we need a resize before the insert */
        if (!uf_hashmap_resize(self)) {
                return false;
        }

        hash = self->key.hash(key);

        /* Ensure we have at least key *and* value together */
        if (uf_unlikely(!key && !value)) {
                return true;
        }

        return uf_hashmap_insert_map(self, hash, key, value);
}

/**
 * Find the parent node for a key and return it
 */
static UfHashmapNode *uf_hashmap_get_node(UfHashmap *self, void *key)
{
        UfHashmapNode *bucket = NULL;
        uint32_t hash;

        hash = self->key.hash(key);
        bucket = uf_hashmap_initial_bucket(self, hash);

        for (UfHashmapNode *node = bucket; node; node = node->next) {
                if (self->key.compare(node->key, key)) {
                        return node;
                }
        }

        return NULL;
}

void *uf_hashmap_get(UfHashmap *self, void *key)
{
        UfHashmapNode *node = NULL;

        if (uf_unlikely(!self)) {
                return NULL;
        }

        node = uf_hashmap_get_node(self, key);
        if (uf_unlikely(!node)) {
                return NULL;
        }
        return node->value;
}

static void uf_hashmap_from(UfHashmap *source, UfHashmap *target)
{
        *target = *source;
        memset(&target->buckets, 0, sizeof(target->buckets));
}

/**
 * Check if our current count is at the resize count, and start our
 * resize if at all possible.
 */
static bool uf_hashmap_resize(UfHashmap *self)
{
        UfHashmap target = { 0 };

        /* Continue unimpeded */
        if (uf_likely(self->buckets.next_resize != self->buckets.current)) {
                return true;
        }

        /* Set up the target from the source and bind up new blobs.. */
        uf_hashmap_from(self, &target);
        target.buckets.max = UF_HASH_GROWTH * self->buckets.max;
        target.buckets.mask = target.buckets.max - 1;
        target.buckets.next_resize =
            (unsigned int)(((double)target.buckets.max) * UF_HASH_FILL_RATE),
        target.buckets.blob = calloc(target.buckets.max, sizeof(struct UfHashmapNode));
        if (uf_unlikely(!target.buckets.blob)) {
                return false;
        }

        /* Start moving everything across and preserve the hash (no need to rehash) */
        for (unsigned int i = 0; i < self->buckets.max; i++) {
                for (UfHashmapNode *node = &self->buckets.blob[i]; node; node = node->next) {
                        uint32_t hash = node->hash;

                        if (uf_unlikely(hash == 0)) {
                                continue;
                        }

                        if (!uf_hashmap_insert_map(&target, hash, node->key, node->value)) {
                                goto failed;
                        }
                }
        }

        /* Woot, we won */
        uf_hashmap_free_internal(self, false);
        *self = target;

        return true;
failed:
        uf_hashmap_free_internal(&target, false);
        return false;
}

bool uf_hashmap_remove(UfHashmap *self, void *key)
{
        UfHashmapNode *node = NULL;

        if (uf_unlikely(!self)) {
                return false;
        }

        node = uf_hashmap_get_node(self, key);
        if (uf_unlikely(!node)) {
                return false;
        }

        if (uf_likely(self->free.key != NULL)) {
                self->free.key(node->key);
        }
        if (uf_likely(self->free.value != NULL)) {
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
