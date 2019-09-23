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
#include <string.h>

#include "array.h"
#include "macros.h"
#include "ptr-array.h"

/**
 * Simple test to validate append
 */
START_TEST(test_array_simple_add)
{
        LsArray *array = NULL;
        struct TestStruct {
                int x;
                int j;
        };
        struct TestStruct x = { .x = 30, .j = 20 };
        struct TestStruct y = { .x = 22, .j = 15 };
        struct TestStruct z = { .x = 12, .j = 62 };
        struct TestStruct *ptr_x = NULL;

        array = ls_array_new(sizeof(struct TestStruct));
        fail_if(!array, "Failed to construct array");

        fail_if(!ls_array_add(array, &x), "Failed to add new TestStruct");
        fail_if(!ls_array_add(array, &y), "Failed to add new TestStruct");
        fail_if(!ls_array_add(array, &z), "Failed to add new TestStruct");

        ptr_x = array->data[0];
        fail_if(ptr_x->x != 30, "Incorrect X");
        fail_if(ptr_x->j != 20, "Incorrect X");

        ptr_x = array->data[1];
        fail_if(ptr_x->x != 22, "Incorrect Y");
        fail_if(ptr_x->j != 15, "Incorrect Y");

        ptr_x = array->data[2];
        fail_if(ptr_x->x != 12, "Incorrect Z");
        fail_if(ptr_x->j != 62, "Incorrect Z");

        fail_if(array->len != 3, "Incorrect array length");

        ls_array_free(array, NULL);
}
END_TEST

START_TEST(test_ptr_array_simple_add)
{
        LsPtrArray *array = NULL;

        array = ls_ptr_array_new();
        fail_if(!array, "Failed to construct pointer array");

        fail_if(!ls_array_add(array, strdup("john")), "Failed to add john");
        fail_if(!ls_array_add(array, strdup("bobby")), "Failed to add bobby");
        fail_if(!ls_array_add(array, strdup("rupert")), "Failed to add rupert");
        fail_if(!ls_array_add(array, strdup("harry")), "Failed to add harry");

        fail_if(strcmp(array->data[0], "john") != 0, "Failed to get john");
        fail_if(strcmp(array->data[1], "bobby") != 0, "Failed to get bobby");
        fail_if(strcmp(array->data[2], "rupert") != 0, "Failed to get rupert");
        fail_if(strcmp(array->data[3], "harry") != 0, "Failed to get harry");

        fail_if(array->len != 4, "Incorrect array length");

        ls_array_free(array, free);
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

        tcase_add_test(tc, test_array_simple_add);
        tcase_add_test(tc, test_ptr_array_simple_add);

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
