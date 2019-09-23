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

#include "array.h"

LsArray *ls_array_new(size_t item_size)
{
        return ls_array_new_size(item_size, 0);
}

LsArray *ls_array_new_size(size_t item_size, uint16_t reserved)
{
        LsArray *ret = NULL;

        ret = calloc(1, sizeof(struct LsArray));
        if (!ret) {
                return NULL;
        }
        ret->item_size = item_size;

        /* Try to reserve the data. */
        if (reserved > 0) {
                ret->data = calloc(reserved, item_size);
                if (!ret->data) {
                        free(ret);
                        return NULL;
                }
        }
        ret->size = reserved;
        ret->len = 0;

        return ret;
}

bool ls_array_add(LsArray *self, void *data)
{
        if (ls_unlikely(!self)) {
                return false;
        }

        uint16_t new_size = (uint16_t)(self->len + 1);

        /* First allocation of data blob */
        if (ls_unlikely(!self->data)) {
                self->data = calloc(1, self->item_size);
                if (!self->data) {
                        return false;
                }
                self->size = 1;
        }

        /* Or do we need to resize? */
        if (new_size > self->size) {
                self->data = realloc(self->data, self->item_size * new_size);
                if (!self->data) {
                        return false;
                }
                self->size = new_size;
        }
        self->len++;
        self->data[self->len - 1] = data;

        return true;
}

void ls_array_free(LsArray *self, ls_free_func freer)
{
        if (ls_unlikely(!self)) {
                return;
        }

        if (!freer) {
                goto cleanup_array;
        }

        for (uint16_t i = 0; i < self->len; i++) {
                freer(self->data[i]);
        }

cleanup_array:
        free(self->data);
        free(self);
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
