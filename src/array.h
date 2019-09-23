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

#pragma once

#define _GNU_SOURCE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "macros.h"

/**
 * LsArray is a dynamically growing array that allows items to sit
 * in memory more easily.
 *
 * Note, the conventional approach to using the array is to construct
 * a new empty array with a fixed item size, and add to the array
 * as you go along.
 *
 * An unconventional approach is to cast the data blob, and
 * preallocate based on the size of some struct, to give contiguous
 * blocks in memory.
 *
 */
typedef struct LsArray {
        uint16_t len;     /*< Current length */
        uint16_t size;    /*< Current allocated size */
        size_t item_size; /*< Size of each allocated item. */
        void **data;      /*<Blob to access data */
} LsArray;

/**
 * Construct a new LsArray with no pre-allocated member regions
 */
LsArray *ls_array_new(size_t item_size);

/**
 * Construct a new LsArray with the given item size, pre-allocating
 * the given number of blocks.
 */
LsArray *ls_array_new_size(size_t item_size, uint16_t reserved);

/**
 * Add a new element of data to the array. It must have the same
 * fixed size as at construction time.
 * @returns True if we added an item
 */
bool ls_array_add(LsArray *self, void *data);

void ls_array_free(LsArray *self, ls_free_func freer);

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
