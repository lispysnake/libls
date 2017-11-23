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

#include "map.h"
#include "util.h"

typedef struct UfHashmapNode UfHashmapNode;

/**
 * Initial size of 128 items. Slight overcommit but prevents too much future
 * growth as our growth ratio and algorithm is ^2 based.
 */
#define UF_HASH_INITIAL_SIZE 128

/**
 * Opaque UfHashmap implementation, simply an organised header for the
 * function pointers, state and buckets.
 */
struct UfHashmap {
        UfHashmapNode *buckets; /**<Contiguous blob of buckets, over-commits */
        int n_buckets;          /**<How many buckets are currently allocated? */
        int n_elements;         /**<How many items do we currently have? */
        unsigned int root_mask; /**< pow2 n_buckets - 1 */

        struct {
                uf_hashmap_hash_func key;    /**<Key hash generator */
                uf_hashmap_equal_func value; /**<Key value comparison */
        } hash;
        struct {
                uf_hashmap_free_func key;   /**<Key free function */
                uf_hashmap_free_func value; /**<Value free function */
        } free;
};

/**
 * A UfHashmapNode is simply a single bucket within a UfHashmap and can have
 * a key/value. This is not a chained mechanism.
 */
struct UfHashmapNode {
        void *key;
        void *value;
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
                .hash.key = hash,
                .hash.value = compare,
                .free.key = key_free,
                .free.value = value_free,
                .n_buckets = UF_HASH_INITIAL_SIZE,
                .n_elements = 0,
                .root_mask = UF_HASH_INITIAL_SIZE - 1,
        };

        /* Some things we actually do need, sorry programmer. */
        assert(clone.hash.key);
        assert(clone.hash.value);

        ret = calloc(1, sizeof(struct UfHashmap));
        if (!ret) {
                return NULL;
        }
        *ret = clone;

        ret->buckets = calloc(UF_HASH_INITIAL_SIZE, sizeof(struct UfHashmapNode));
        if (!ret->buckets) {
                uf_hashmap_free(ret);
                return NULL;
        }

        return ret;
}

void uf_hashmap_free(UfHashmap *self)
{
        if (uf_unlikely(!self)) {
                return;
        }
        free(self->buckets);
        free(self);
        return;
}

bool uf_hashmap_simple_equal(const void *a, const void *b)
{
        return a == b;
}

uint32_t uf_hashmap_simple_hash(const void *v)
{
        return (uint32_t)((uintptr_t)(v));
}

/**
 * Find the base bucket to work from
 */
static inline UfHashmapNode *uf_hashmap_initial_bucket(UfHashmap *self, void *key)
{
        return &self->buckets[self->hash.key(key) & self->root_mask];
}

bool uf_hashmap_put(UfHashmap *self, void *key, void *value)
{
        UfHashmapNode *bucket = NULL;

        if (uf_unlikely(!self)) {
                return false;
        }

        /* Cheat for now. Soon, we need to handle collisions */
        bucket = uf_hashmap_initial_bucket(self, key);
        bucket->key = key;
        bucket->value = value;

        return true;
}

void *uf_hashmap_get(UfHashmap *self, void *key)
{
        UfHashmapNode *bucket = NULL;

        if (uf_unlikely(!self)) {
                return NULL;
        }

        /* Cheat for now. Soon, we need to handle collisions */
        bucket = uf_hashmap_initial_bucket(self, key);
        if (!self->hash.value(bucket->key, key)) {
                return NULL;
        }
        return bucket->value;
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
