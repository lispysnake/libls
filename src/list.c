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

#include <stdlib.h>

#include "list.h"

/**
 * Main helper to create new nodes for a list. This will result in
 * a sparse memory layout, so if concerned you should probably use
 * an array type.
 */
static inline LsList *ls_node_calloc(void *data)
{
        LsList *ret = NULL;

        ret = calloc(1, sizeof(struct LsList));
        if (!ret) {
                return NULL;
        }

        ret->data = data;
        return ret;
}

LsList *ls_list_prepend(LsList *list, void *data)
{
        LsList *ret = NULL;

        ret = ls_node_calloc(data);
        if (!ret) {
                return NULL;
        }
        ret->next = list;
        return ret;
}

/**
 * Attempt to find the tail of the list (O(1))
 */
static inline LsList *ls_list_tail(LsList *list)
{
        LsList *node = list;

        while (node && node->next) {
                node = node->next;
        }

        return node;
}

LsList *ls_list_append(LsList *list, void *data)
{
        LsList *tail = NULL;
        LsList *node = NULL;

        /* Create new storage link */
        node = ls_node_calloc(data);
        if (!node) {
                return NULL;
        }

        /* Find the tail if a list exists. */
        tail = ls_list_tail(list);

        /* We're the new list */
        if (!tail) {
                return node;
        }

        /* Link into the chain */
        tail->next = node;
        return list;
}

LsList *ls_list_reverse(LsList *list)
{
        LsList *node, *prev, *next;

        node = list;
        prev = next = NULL;

        while (node) {
                next = node->next;
                node->next = prev;
                prev = node;
                node = next;
        }

        return prev;
}

void ls_list_free(LsList *list)
{
        ls_list_free_full(list, NULL);
}

void ls_list_free_full(LsList *list, ls_free_func freer)
{
        /* Walk the list and free each part. */

        LsList *node = list;
        LsList *next = NULL;

        while (node) {
                next = node->next;
                if (freer && node->data) {
                        freer(node->data);
                }
                free(node);
                node = next;
        }
}

unsigned int ls_list_length(LsList *list)
{
        LsList *node = list;
        unsigned int length = 0;

        while (node) {
                node = node->next;
                ++length;
        }

        return length;
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
