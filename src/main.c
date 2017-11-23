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

#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "util.h"

int main(__uf_unused__ int argc, __uf_unused__ char **argv)
{
        UfHashmap *map = NULL;
        int ret = EXIT_FAILURE;

        map = uf_hashmap_new(uf_hashmap_simple_hash, uf_hashmap_simple_equal);
        if (!map) {
                return EXIT_FAILURE;
        }

        for (int i = 1; i < 500; i++) {
                const void *ret = NULL;
                if (!uf_hashmap_put(map, UF_INT_TO_PTR(i), UF_INT_TO_PTR(i))) {
                        fprintf(stderr, "Storage failed\n");
                        goto end;
                }
                ret = uf_hashmap_get(map, UF_INT_TO_PTR(i));
                if (!ret) {
                        fprintf(stderr, "Return failed\n");
                        goto end;
                }
                int r = (int)UF_PTR_TO_INT(ret);
                if (r != i) {
                        fprintf(stderr, "No match! Got %d expected %d\n", r, i);
                        goto end;
                }
                fprintf(stderr, "%d = %d\n", r, i);
        }

        ret = EXIT_SUCCESS;
end:
        uf_hashmap_free(map);
        return ret;
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
