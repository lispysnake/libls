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

#include <stdlib.h>

#include "map.h"
#include "util.h"

/**
 * Opaque UfHashmap implementation, simply an organised header for the
 * function pointers, state and buckets.
 */
struct UfHashmap {
        struct {
                uf_hashmap_hash_func key;    /**<Key hash generator */
                uf_hashmap_equal_func value; /**<Key value comparison */
        } hash;
        struct {
                uf_hashmap_free_func key;   /**<Key free function */
                uf_hashmap_free_func value; /**<Value free function */
        } free;
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
        };

        ret = calloc(1, sizeof(struct UfHashmap));
        if (!ret) {
                return NULL;
        }
        *ret = clone;

        /* TODO: Assign buckets and such */

        return ret;
}

void uf_hashmap_free(UfHashmap *map)
{
        if (uf_unlikely(!map)) {
                return;
        }
        free(map);
        return;
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
