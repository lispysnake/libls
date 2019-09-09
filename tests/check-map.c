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

#define _GNU_SOURCE

#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "map.h"

START_TEST(test_map_simple)
{
        LsHashmap *map = NULL;
        void *v = NULL;

        map = ls_hashmap_new(ls_hashmap_string_hash, ls_hashmap_string_equal);
        fail_if(!map, "Failed to construct string hashmap!");

        fail_if(!ls_hashmap_put(map, "charlie", LS_INT_TO_PTR(12)), "Failed to insert");
        fail_if(!ls_hashmap_put(map, "bob", LS_INT_TO_PTR(38)), "Failed to insert");

        v = ls_hashmap_get(map, "charlie");
        fail_if(!v, "Failed to get charlie");
        fail_if(LS_PTR_TO_INT(v) != 12, "Retrieved value is incorrect");
        v = NULL;

        v = ls_hashmap_get(map, "bob");
        fail_if(!v, "Failed to get bob");
        fail_if(LS_PTR_TO_INT(v) != 38, "Retrieved value is incorrect");

        ls_hashmap_free(map);
}
END_TEST

START_TEST(test_map_null_zero)
{
        LsHashmap *map = NULL;
        char *ret = NULL;

        /* Construct hashmap of int to string value to check null key */
        map = ls_hashmap_new_full(ls_hashmap_simple_hash, ls_hashmap_simple_equal, NULL, free);
        fail_if(!map, "Failed to construct hashmap");

        for (size_t i = 0; i < 1000; i++) {
                char *p = NULL;
                if (asprintf(&p, "VALUE: %ld", i) < 0) {
                        abort();
                }
                fail_if(!ls_hashmap_put(map, LS_INT_TO_PTR(i), p), "Failed to insert keypair");
        }

        ret = ls_hashmap_get(map, LS_INT_TO_PTR(0));
        fail_if(!ret, "Failed to retrieve key 0 (glibc NULL)");
        fail_if(strcmp(ret, "VALUE: 0") != 0, "Returned string is incorrect");

        ls_hashmap_free(map);
}
END_TEST

/**
 * Very nasty test that tries to brute force the map into crippling.
 *
 * We'll insert a 1000 allocated values, knowing we'll force both collisions
 * in the map and resizes.
 * We'll remove 200 elements from the final map, and make sure that within the
 * current scope they're really gone (link buggery).
 *
 * We'll finally run over them again and check they did indeed get nuked, in
 * a single cycle after the removals, to ensure we've not been tricked by
 * some broken tombstone in the list mechanism.
 *
 * Finally, we free the map with our constructor free function, and any values
 * left would cause valgrind to scream very very loudly.
 */
START_TEST(test_map_remove)
{
        LsHashmap *map = NULL;

        /* Construct hashmap like test_map_null_zero but remove elements */
        map = ls_hashmap_new_full(ls_hashmap_simple_hash, ls_hashmap_simple_equal, NULL, free);
        fail_if(!map, "Failed to construct hashmap");

        for (size_t i = 0; i < 1000; i++) {
                char *p = NULL;
                if (asprintf(&p, "VALUE: %ld", i) < 0) {
                        abort();
                }
                fail_if(!ls_hashmap_put(map, LS_INT_TO_PTR(i), p), "Failed to insert keypair");
        }

        /* Remove and check at time of removal they're really gone. */
        for (size_t i = 500; i < 700; i++) {
                void *v = NULL;
                char *p = NULL;
                if (asprintf(&p, "VALUE: %ld", i) < 0) {
                        abort();
                }

                v = ls_hashmap_get(map, LS_INT_TO_PTR(i));
                fail_if(!v, "Key doesn't actually exist!");
                fail_if(strcmp(v, p) != 0, "Key in map is wrong!");
                free(p);
                v = NULL;

                fail_if(!ls_hashmap_remove(map, LS_INT_TO_PTR(i)), "Failed to remove keypair");

                v = ls_hashmap_get(map, LS_INT_TO_PTR(i));
                fail_if(v != NULL, "Key should not longer exist in map!");
        }

        /* Now go check they all did disappear and it wasn't a list link fluke */
        for (size_t i = 500; i < 700; i++) {
                void *v = NULL;

                v = ls_hashmap_get(map, LS_INT_TO_PTR(i));
                fail_if(v != NULL, "Key should not longer exist in map!");
        }

        /* Valgrind test would scream here if the list is broken */
        ls_hashmap_free(map);
}
END_TEST

/**
 * Standard helper for running a test suite
 */
static int ls_test_run(Suite *suite)
{
        SRunner *runner = NULL;
        int n_failed = 0;

        runner = srunner_create(suite);
        srunner_run_all(runner, CK_VERBOSE);
        n_failed = srunner_ntests_failed(runner);
        srunner_free(runner);

        return n_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

static Suite *test_create(void)
{
        Suite *s = NULL;
        TCase *tc = NULL;

        s = suite_create(__FILE__);
        tc = tcase_create(__FILE__);
        suite_add_tcase(s, tc);

        tcase_add_test(tc, test_map_simple);
        tcase_add_test(tc, test_map_null_zero);
        tcase_add_test(tc, test_map_remove);

        /* TODO: Add actual tests. */
        return s;
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
        return ls_test_run(test_create());
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
