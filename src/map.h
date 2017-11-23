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

#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * UfHashmap is an in-memory hashed key-value data structure (dict/map)
 * typically suited to string key/value pairs.
 */
typedef struct UfHashmap UfHashmap;

/**
 * Required definition for a free function
 */
typedef void (*uf_hashmap_free_func)(void *v);

/**
 * Required definition for a hash generator function
 *
 * @param v Key to be hashed
 * @returns uint32_t hash for the key
 */
typedef uint32_t (*uf_hashmap_hash_func)(const void *v);

/**
 * Required definition for key equality function
 *
 * @param a first item to be compared
 * @param b second item to be compared
 * @returns true if both keys are considered equal.
 */
typedef bool (*uf_hashmap_equal_func)(const void *a, const void *b);

/**
 * Simple comparison for pointer types.
 */
bool uf_hashmap_simple_equal(const void *a, const void *b);

/**
 * Simple hash for pointer types.
 */
uint32_t uf_hashmap_simple_hash(const void *v);

/**
 * Construct a new UfHashmap with the given @hash and @compare functions.
 *
 * @param hash A hash generator function
 * @param compare A key equality function
 *
 * @note Free with uf_hashmap_free
 *
 * @return A newly allocated UfHashmap
 */
UfHashmap *uf_hashmap_new(uf_hashmap_hash_func hash, uf_hashmap_equal_func compare);

/**
 * Construct a new UfHashmap with key/value free functions
 *
 * @param hash A hash generator function
 * @param compare A key equality function
 * @param key_free Function to call to free any keys when replaced or the table is freed
 * @param value_free Function to call to free any values when replaced or the table is freed
 *
 * @note Free with uf_hashmap_free
 *
 * @return A newly allocated UfHashmap
 */
UfHashmap *uf_hashmap_new_full(uf_hashmap_hash_func hash, uf_hashmap_equal_func compare,
                               uf_hashmap_free_func key_free, uf_hashmap_free_func value_free);

/**
 * Free a previously allocated hashmap
 *
 * @param map Pointer to a previously allocated map
 */
void uf_hashmap_free(UfHashmap *map);

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
