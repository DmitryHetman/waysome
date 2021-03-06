/*
 * waysome - wayland based window manager
 *
 * Copyright in alphabetical order:
 *
 * Copyright (C) 2014-2015 Julian Ganz
 * Copyright (C) 2014-2015 Manuel Messner
 * Copyright (C) 2014-2015 Marcel Müller
 * Copyright (C) 2014-2015 Matthias Beyer
 * Copyright (C) 2014-2015 Nadja Sommerfeld
 *
 * This file is part of waysome.
 *
 * waysome is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * waysome is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with waysome. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @addtogroup tests "Testing"
 *
 * @{
 */

/**
 * @addtogroup tests_objects_set "Testing: Set"
 *
 * @{
 */

#include <check.h>
#include <stdlib.h>

#include "tests.h"
#include "objects/object.h"
#include "objects/set.h"
#include "util/arithmetical.h"

/*
 *
 * We need a type with a non-ws_object-compare functionality, to be able to test
 * the set appropriately
 *
 */

struct ws_set_test_obj {
    struct ws_object obj;
};

static int
compare_set_test_objs(
    struct ws_object const* o1,
    struct ws_object const* o2
) {
    return signum(o2 - o1);
}

ws_object_type_id TEST_ID = {
    .supertype  = &WS_OBJECT_TYPE_ID_OBJECT,
    .typestr    = "ws_set_test_obj",

    .deinit_callback = NULL,
    .hash_callback = NULL,
    .cmp_callback = compare_set_test_objs,
};

struct ws_set_test_obj*
new_test_obj(void)
{
    struct ws_set_test_obj* o = calloc(1, sizeof(*o));

    if (o) {
        ws_object_init(&o->obj);
        o->obj.id = &TEST_ID;
    }

    return o;
}

static struct ws_set* set = NULL;

static struct ws_set* set_a = NULL;
static struct ws_set* set_b = NULL;

#define N_TEST_OBJS 15
static struct ws_object* TEST_OBJS[N_TEST_OBJS] = { 0 };

/*
 *
 * Selector helpers
 *
 */

static int
predicate(
    void const* obj,
    void* etc
) {
    return 1;
}

static int
processor(
    void* etc,
    void const* obj
) {
    struct ws_set* s = etc;
    ws_set_insert(s, (struct ws_object *) obj);
    return 0;
}

/*
 *
 * Setup/Teardown functions
 *
 */

static void
test_set_setup(void)
{
    ck_assert(set == NULL);
    set = ws_set_new();
    ck_assert(set != NULL);
}

static void
test_set_setup_objs(void)
{
    test_set_setup();

    for (int i = N_TEST_OBJS - 1; i; --i) {
        ck_assert(TEST_OBJS[i] == NULL);

        TEST_OBJS[i] = (struct ws_object*) new_test_obj();

        ck_assert(TEST_OBJS[i] != NULL);
    }
}

static void
test_set_setup_sets(void)
{
    test_set_setup_objs();

    ck_assert(set_a == NULL);
    ck_assert(set_b == NULL);

    set_a = ws_set_new();
    set_b = ws_set_new();

    ck_assert(set_a != NULL);
    ck_assert(set_b != NULL);

    for (int i = N_TEST_OBJS - 1; i; --i) {
        if (i % 2) {
            ck_assert(0 == ws_set_insert(set_a, TEST_OBJS[i]));
        } else {
            ck_assert(0 == ws_set_insert(set_b, TEST_OBJS[i]));
        }
    }

}

static void
test_set_teardown(void)
{
    ck_assert(set != NULL);
    ws_object_deinit(&set->obj);
    free(set);
    set = NULL;
    ck_assert(set == NULL);
}

static void
test_set_teardown_objs(void)
{
    test_set_teardown(); // So the set deallocates the own objects

    // and now, we can clean up

    int i;

    for (i = N_TEST_OBJS - 1; i; --i) {
        ck_assert(TEST_OBJS[i] != NULL);
        ws_object_deinit(TEST_OBJS[i]);

        free(TEST_OBJS[i]);

        TEST_OBJS[i] = NULL;
    }
}

static void
test_set_teardown_sets(void)
{
    ws_object_deinit((struct ws_object*) set_a);
    set_a = NULL;

    ws_object_deinit((struct ws_object*) set_b);
    set_b = NULL;

    test_set_teardown_objs();
}

/*
 *
 * Tests
 *
 */

START_TEST (test_set_init) {
    ck_assert(0 == ws_set_init(set));
}
END_TEST

START_TEST (test_set_init_deinit) {
    ck_assert(0 == ws_set_init(set));
    ws_object_deinit((struct ws_object*) set);
    ck_assert(0 == ws_set_init(set));
    // reinitialize here, so we do not segfault in the teardown function
}
END_TEST

/*
 *
 * Tests: Element insertion, removal, etc
 *
 */

START_TEST (test_set_insert) {
    int i;
    for (i = N_TEST_OBJS - 1; i; --i) {
        ck_assert(0 == ws_set_insert(set, TEST_OBJS[i]));
    }
}
END_TEST

START_TEST (test_set_insert_remove) {
    int i;
    for (i = N_TEST_OBJS - 1; i; --i) {
        ck_assert(0 == ws_set_insert(set, TEST_OBJS[i]));
    }

    for (i = N_TEST_OBJS - 1; i; --i) {
        ck_assert(0 == ws_set_remove(set, TEST_OBJS[i]));
    }
}
END_TEST

START_TEST (test_set_insert_get_remove) {
    int i;
    for (i = N_TEST_OBJS - 1; i; --i) {
        ck_assert(0 == ws_set_insert(set, TEST_OBJS[i]));
    }

    for (i = N_TEST_OBJS - 1; i; --i) {
        ck_assert(TEST_OBJS[i] == ws_set_get(set, TEST_OBJS[i]));
    }

    for (i = N_TEST_OBJS - 1; i; --i) {
        ck_assert(0 == ws_set_remove(set, TEST_OBJS[i]));
    }
}
END_TEST

/*
 *
 * Tests: Set operations
 *
 */

//START_TEST (test_set_union) {
//    ck_assert(0 == ws_set_union(set, set_a, set_b));
//
//    int i;
//
//    for (i = N_TEST_OBJS - 1; i; --i) {
//        ck_assert(TEST_OBJS[i] == ws_set_get(set, TEST_OBJS[i]));
//    }
//}
//END_TEST

//START_TEST (test_set_intersection) {
//    ck_assert(0 == ws_set_intersection(set, set_a, set_b));
//    // No intersection by now, as the sets `set_a` and `set_b` do not contain
//    // equal elements
//
//    int i;
//
//    for (i = N_TEST_OBJS - 1; i; --i) {
//        ck_assert(NULL == ws_set_get(set, TEST_OBJS[i]));
//    }
//
//    // Insert the same object into both sets here.
//    // No assertions here, as some of the objects are already in the sets
//    for (i = N_TEST_OBJS / 2; i; --i) {
//        ws_set_insert(set_a, TEST_OBJS[i]);
//        ws_set_insert(set_b, TEST_OBJS[i]);
//    }
//
//    // Now we create a _real_ intersection, where something actually happens
//    ck_assert(0 == ws_set_intersection(set, set_a, set_b));
//
//    for (i = N_TEST_OBJS / 2; i; --i) {
//        ck_assert(TEST_OBJS[i] == ws_set_get(set, TEST_OBJS[i]));
//    }
//}
//END_TEST

//START_TEST (test_set_xor) {
//    ck_assert(0 == ws_set_xor(set, set_a, set_b));
//
//    int i;
//
//    for (i = N_TEST_OBJS - 1; i; --i) {
//        ck_assert(NULL == ws_set_get(set, TEST_OBJS[i]));
//    }
//
//    ws_set_insert(set_a, TEST_OBJS[0]);
//    ws_set_insert(set_b, TEST_OBJS[0]);
//
//    ck_assert(0 == ws_set_xor(set, set_a, set_b));
//
//    for (i = N_TEST_OBJS - 1; i; --i) {
//        if (i == 0) {
//            ck_assert(TEST_OBJS[i] == ws_set_get(set, TEST_OBJS[i]));
//        } else {
//            ck_assert(NULL == ws_set_get(set, TEST_OBJS[i]));
//        }
//    }
//}
//END_TEST

//START_TEST (test_set_subset) {
//    // "set" is empty
//    ck_assert(0 == ws_set_is_subset(set, set_a));
//    ck_assert(0 == ws_set_is_subset(set, set_b));
//
//    ck_assert(1 == ws_set_is_subset(set_a, set));
//    ck_assert(1 == ws_set_is_subset(set_b, set));
//
//    // set_a and set_b are different
//    ck_assert(0 == ws_set_is_subset(set_a, set_b));
//    ck_assert(0 == ws_set_is_subset(set_b, set_a));
//}
//END_TEST

START_TEST (test_set_cardinality) {
    ck_assert(0 == ws_set_cardinality(set));

    ck_assert(N_TEST_OBJS / 2 == ws_set_cardinality(set_a));
    ck_assert(N_TEST_OBJS / 2 == ws_set_cardinality(set_b));

//    ck_assert(0 == ws_set_union(set, set_a, set_b));
//
//    ck_assert(15 == ws_set_cardinality(set));
}
END_TEST

START_TEST (test_set_select) {
    ck_assert(0 == ws_set_cardinality(set));

    ck_assert(0 == ws_set_select(set_a, predicate, NULL, processor, set));

    ck_assert(1 == ws_set_equal(set, set_a));
}
END_TEST

/*
 *
 * Suite
 *
 */

static Suite*
set_suite(void)
{
    Suite* s    = suite_create("Set");
    TCase* tc   = tcase_create("main case");
    TCase* tce  = tcase_create("Elements case");
    TCase* tcso = tcase_create("Set operations case");

    suite_add_tcase(s, tc);
    tcase_add_checked_fixture(tc, test_set_setup, test_set_teardown);

    tcase_add_test(tc, test_set_init);
    tcase_add_test(tc, test_set_init_deinit);

    suite_add_tcase(s, tce);
    tcase_add_checked_fixture(tce, test_set_setup_objs, test_set_teardown_objs);
    tcase_add_test(tce, test_set_insert);
    tcase_add_test(tce, test_set_insert_remove);
    tcase_add_test(tce, test_set_insert_get_remove);

    suite_add_tcase(s, tcso);
    tcase_add_checked_fixture(tcso,
                              test_set_setup_sets,
                              test_set_teardown_sets);
//    tcase_add_test(tcso, test_set_union);
//    tcase_add_test(tcso, test_set_intersection);
//    tcase_add_test(tcso, test_set_xor);
//    tcase_add_test(tcso, test_set_subset);
    tcase_add_test(tcso, test_set_cardinality);
    tcase_add_test(tcso, test_set_select);

    return s;
}

WS_TESTS_CHECK_MAIN(set_suite);

/**
 * @}
 */

/**
 * @}
 */

