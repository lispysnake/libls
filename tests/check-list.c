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

#include "list.h"
#include "macros.h"

/**
 * Simple test to validate append
 */
START_TEST(test_list_simple_append)
{
        LsList *list = NULL;
        unsigned int len = 0;

        len = ls_list_length(list);
        fail_if(len != 0, "Length of NULL list should be 0");

        list = ls_list_append(list, "rory");
        fail_if(!list, "Failed to construct basic list");
        len = ls_list_length(list);
        fail_if(len != 1, "Length should be 1");

        list = ls_list_append(list, "jimmy");
        fail_if(!list, "Failed to append to list");
        len = ls_list_length(list);
        fail_if(len != 2, "Length should be 2");

        fail_if(strcmp(list->data, "rory") != 0, "Invalid data at 0");
        fail_if(strcmp(list->next->data, "jimmy") != 0, "Invalid data at 1");

        list = ls_list_reverse(list);
        fail_if(!list, "Failed to reverse the list");
        fail_if(ls_list_length(list) != 2, "Broken links in list");

        fail_if(strcmp(list->data, "jimmy") != 0, "Invalid data at 0");
        fail_if(strcmp(list->next->data, "rory") != 0, "Invalid data at 1");

        ls_list_free(list);
}
END_TEST

/**
 * Simple test to validate prepend
 */
START_TEST(test_list_simple_prepend)
{
        LsList *list = NULL;
        unsigned int len = 0;

        len = ls_list_length(list);
        fail_if(len != 0, "Length of NULL list should be 0");

        list = ls_list_prepend(list, "bob");
        fail_if(!list, "Failed to construct basic list");
        len = ls_list_length(list);
        fail_if(len != 1, "Length should be 1");

        list = ls_list_prepend(list, "charles");
        fail_if(!list, "Failed to prepend to list");
        len = ls_list_length(list);
        fail_if(len != 2, "Length should be 2");

        fail_if(strcmp(list->data, "charles") != 0, "Invalid data at 0");
        fail_if(strcmp(list->next->data, "bob") != 0, "Invalid data at 1");

        list = ls_list_reverse(list);
        fail_if(!list, "Failed to reverse the list");
        fail_if(ls_list_length(list) != 2, "Broken links in list");

        fail_if(strcmp(list->data, "bob") != 0, "Invalid data at 0");
        fail_if(strcmp(list->next->data, "charles") != 0, "Invalid data at 1");

        ls_list_free(list);
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

        tcase_add_test(tc, test_list_simple_append);
        tcase_add_test(tc, test_list_simple_prepend);

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
