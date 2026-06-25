/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 Brian J. Downs
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <crosscheck.h>

#include "../saguaro.h"

#define USER_ID 1001

enum {
    ROLE_USER = 1,
    ROLE_EDITOR = 2,
    ROLE_ADMIN = 3
};

enum {
    PERM_READ = 1,
    PERM_WRITE = 2,
    PERM_DELETE = 3
};

saguaro_t *ctx = NULL;

void
cc_setup()
{
    ctx = saguaro_init();
}

void
cc_tear_down()
{
    saguaro_free(ctx);
    ctx = NULL;
}

cc_result_t
test_assign_role(void)
{
    saguaro_result_t res = saguaro_assign_role(ctx, USER_ID, ROLE_ADMIN);
    CC_ASSERT_INT_EQUAL((int)res, (int)SAGUARO_OK);
    CC_SUCCESS;
}

cc_result_t
test_unassign_role(void)
{
    saguaro_result_t res1 = saguaro_assign_role(ctx, USER_ID, ROLE_ADMIN);
    CC_ASSERT_INT_EQUAL((int)res1, (int)SAGUARO_OK);
    saguaro_result_t res2 = saguaro_unassign_role(ctx, USER_ID, ROLE_ADMIN);
    CC_ASSERT_INT_EQUAL((int)res2, (int)SAGUARO_OK);
    CC_SUCCESS;
}

cc_result_t
test_assign_permission(void)
{
    saguaro_result_t res = saguaro_assign_permission(ctx, ROLE_ADMIN, PERM_DELETE);
    CC_ASSERT_INT_EQUAL((int)res, (int)SAGUARO_OK);
    CC_SUCCESS;
}

cc_result_t
test_unassign_permission(void)
{
    saguaro_result_t res1 = saguaro_assign_permission(ctx, ROLE_ADMIN, PERM_DELETE);
    CC_ASSERT_INT_EQUAL((int)res1, (int)SAGUARO_OK);
    saguaro_result_t res2 = saguaro_unassign_permission(ctx, ROLE_ADMIN, PERM_DELETE);
    CC_ASSERT_INT_EQUAL((int)res2, (int)SAGUARO_OK);
    CC_SUCCESS;
}

cc_result_t
test_cycle_detected(void)
{

    saguaro_result_t res1 = saguaro_assign_role_inheritance(ctx, ROLE_ADMIN,
        ROLE_EDITOR);
    CC_ASSERT_INT_EQUAL((int)res1, (int)SAGUARO_OK);
    saguaro_result_t res2 = saguaro_assign_role_inheritance(ctx, ROLE_EDITOR,
        ROLE_USER);
    CC_ASSERT_INT_EQUAL((int)res2, (int)SAGUARO_OK);
    saguaro_result_t res3 = saguaro_assign_role_inheritance(ctx, ROLE_USER,
        ROLE_ADMIN);
    CC_ASSERT_INT_EQUAL((int)res3, (int)SAGUARO_ERR_CYCLE);
    CC_SUCCESS;
}

cc_result_t
test_is_authorized(void)
{
    saguaro_result_t res1 = saguaro_assign_role(ctx, USER_ID, ROLE_ADMIN);
    CC_ASSERT_INT_EQUAL((int)res1, (int)SAGUARO_OK);
    saguaro_result_t res2 = saguaro_assign_permission(ctx, ROLE_ADMIN, PERM_DELETE);
    CC_ASSERT_INT_EQUAL((int)res2, (int)SAGUARO_OK);
    bool authorized = saguaro_is_authorized(ctx, USER_ID, PERM_DELETE);
    CC_ASSERT_TRUE(authorized);
    CC_SUCCESS;
}

int
main(void)
{
    srand(time(NULL));

    CC_INIT;

    CC_RUN(test_assign_role);
    CC_RUN(test_unassign_role);
    CC_RUN(test_assign_permission);
    CC_RUN(test_unassign_permission);
    CC_RUN(test_cycle_detected);
    CC_RUN(test_is_authorized);

    CC_COMPLETE;

}