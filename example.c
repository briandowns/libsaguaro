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
#include <stdio.h>

#include "saguaro.h"

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

static void
print_result(saguaro_t *ctx, saguaro_subject_id_t subject_id,
    saguaro_permission_id_t permission_id, const char *name)
{
    bool ret = saguaro_is_authorized(ctx, subject_id, permission_id);
    printf("%s: %s\n", name, ret ? "authorized" : "denied");
}

int
main(void)
{
    saguaro_t *ctx;

    ctx = saguaro_init();

    if (ctx == NULL) {
        fprintf(stderr, "failed to create context\n");
        return 1;
    }

    // assign permissions directly to roles
    saguaro_assign_permission(ctx, ROLE_USER, PERM_READ);
    saguaro_assign_permission(ctx, ROLE_EDITOR, PERM_WRITE);
    saguaro_assign_permission(ctx, ROLE_ADMIN, PERM_DELETE);

    // Build inheritance tree.
    // ROLE_ADMIN
    //  -> ROLE_EDITOR
    //      -> ROLE_USER
    saguaro_assign_role_inheritance(ctx, ROLE_ADMIN, ROLE_EDITOR);
    saguaro_assign_role_inheritance(ctx, ROLE_EDITOR, ROLE_USER);

    // assign subject 1001 the ADMIN role
    saguaro_assign_role(ctx, 1001, ROLE_ADMIN);

    // assign subject 1002 the EDITOR role
    saguaro_assign_role(ctx, 1002, ROLE_EDITOR);

    // assign subject 1003 the USER role
    saguaro_assign_role(ctx, 1003, ROLE_USER);

    puts("Subject 1001 (ADMIN)");
    print_result(ctx, 1001, PERM_READ, "PERM_READ");
    print_result(ctx, 1001, PERM_WRITE, "PERM_WRITE");
    print_result(ctx, 1001, PERM_DELETE, "PERM_DELETE");

    puts("");

    puts("Subject 1002 (EDITOR)");
    print_result(ctx, 1002, PERM_READ, "PERM_READ");
    print_result(ctx, 1002, PERM_WRITE, "PERM_WRITE");
    print_result(ctx, 1002, PERM_DELETE, "PERM_DELETE");

    puts("");

    puts("Subject 1003 (USER)");
    print_result(ctx, 1003, PERM_READ, "PERM_READ");
    print_result(ctx, 1003, PERM_WRITE, "PERM_WRITE");
    print_result(ctx, 1003, PERM_DELETE, "PERM_DELETE");

    saguaro_free(ctx);

    return 0;
}
