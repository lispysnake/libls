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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "util.h"

int main(__uf_unused__ int argc, __uf_unused__ char **argv)
{
        UfHashmap *map = NULL;
        int ret = EXIT_FAILURE;

        map = uf_hashmap_new_full(uf_hashmap_string_hash, uf_hashmap_string_equal, free, NULL);
        if (!map) {
                return EXIT_FAILURE;
        }

        for (int i = 0; i < 500; i++) {
                char *p = NULL;
                if (asprintf(&p, "STRING: %d", i) < 0) {
                        abort();
                }
                const char *ret = NULL;
                if (!uf_hashmap_put(map, p, p)) {
                        free(p);
                        fprintf(stderr, "Storage failed\n");
                        goto end;
                }

                ret = uf_hashmap_get(map, p);
                if (strcmp(p, ret) != 0) {
                        fprintf(stderr, "No match! Got %s expected %s\n", ret, p);
                        goto end;
                }
        }

        fprintf(stdout, "20 = %s\n", (char *)uf_hashmap_get(map, "STRING: 20"));

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
