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

/**
 * LsList is a singly-linked list type used for basic storage needs.
 * List types should be used with care, as insert operations may become
 * unnecessarily expensive if not used correctly.
 */
typedef struct LsList {
        void *data;
        struct LsList *next;
} LsList;

/**
 * ls_free_func defines the prototype for free-helpers
 */
typedef void (*ls_free_func)(void *v);

/**
 * Prepend the given pointer to the list, returning the new location
 * of the list.
 *
 * @note Prepends to a singly-linked list are fast, O(1)
 */
LsList *ls_list_prepend(LsList *list, void *data);

/**
 * Append data to the tail of the list, returning the new location
 * of the list.
 *
 * @note Appends to a singly-linked list are slow, O(N)
 */
LsList *ls_list_append(LsList *list, void *data);

/**
 * Reverse the list order and return a pointer to the updated
 * start of the list
 */
LsList *ls_list_reverse(LsList *list);

/**
 * Free a previously allocated list structure and any nodes.
 * You must be very careful to pass the current HEAD of the list
 * for this to function correctly.
 */
void ls_list_free(LsList *list);

/**
 * Identical to ls_list_free, but will additionally call the passed
 * free_func to deallocate data pointers in each link of the list.
 */
void ls_list_free_full(LsList *list, ls_free_func freer);

/**
 * Return the length of the list. If the list is NULL, this will
 * still return 0.
 */
unsigned int ls_list_length(LsList *list);

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
