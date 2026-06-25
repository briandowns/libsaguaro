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

#include "saguaro.h"

struct saguaro_subject_role {
    saguaro_subject_id_t subject_id;
    saguaro_role_id_t role_id;
};

struct saguaro_role_permission {
    saguaro_role_id_t role_id;
    saguaro_permission_id_t permission_id;
};

struct saguaro_role_inheritance {
    saguaro_role_id_t parent_role_id;
    saguaro_role_id_t child_role_id;
};

struct saguaro {
    struct saguaro_subject_role *subject_roles;
    struct saguaro_role_permission *role_permissions;
    struct saguaro_role_inheritance *role_inheritance;
    size_t subject_role_count;
    size_t role_permission_count;
    size_t role_inheritance_count;
};

static bool
role_inheritance_exists(const saguaro_t *ctx, saguaro_role_id_t parent_role_id,
    saguaro_role_id_t child_role_id)
{
    for (size_t i = 0; i < ctx->role_inheritance_count; i++) {
        if (ctx->role_inheritance[i].parent_role_id == parent_role_id &&
            ctx->role_inheritance[i].child_role_id == child_role_id) {
            return true;
        }
    }

    return false;
}

static bool
role_reaches(const saguaro_t *ctx, saguaro_role_id_t start_role_id,
    saguaro_role_id_t target_role_id)
{
    if (start_role_id == target_role_id) {
        return true;
    }

    for (size_t i = 0; i < ctx->role_inheritance_count; i++) {
        if (ctx->role_inheritance[i].parent_role_id != start_role_id) {
            continue;
        }

        if (role_reaches(ctx, ctx->role_inheritance[i].child_role_id,
                target_role_id)) {
            return true;
        }
    }

    return false;
}

saguaro_t*
saguaro_init(void)
{
    return calloc(1, sizeof(saguaro_t));
}

static bool
subject_role_exists(const saguaro_t *ctx, saguaro_subject_id_t subject_id,
                    saguaro_role_id_t role_id)
{
    if (ctx == NULL) {
        return false;
    }

    for (size_t i = 0; i < ctx->subject_role_count; i++) {
        if (ctx->subject_roles[i].subject_id == subject_id &&
            ctx->subject_roles[i].role_id == role_id) {
            return true;
        }
    }

    return false;
}

static bool
role_permission_exists(const saguaro_t *ctx, saguaro_role_id_t role_id,
                       saguaro_permission_id_t permission_id)
{
    if (ctx == NULL) {
        return false;
    }

    for (size_t i = 0; i < ctx->role_permission_count; i++) {
        if (ctx->role_permissions[i].role_id == role_id &&
            ctx->role_permissions[i].permission_id == permission_id) {
            return true;
        }
    }

    return false;
}

void
saguaro_free(saguaro_t *ctx)
{
    if (ctx == NULL) {
        return;
    }

    free(ctx->subject_roles);
    free(ctx->role_permissions);
    free(ctx);
}

saguaro_result_t
saguaro_assign_role(saguaro_t *ctx, saguaro_subject_id_t subject_id,
                    saguaro_role_id_t role_id)
{
    if (ctx == NULL) {
        return SAGUARO_ERR_INVALID;
    }

    if (subject_role_exists(ctx, subject_id, role_id)) {
        return SAGUARO_ERR_EXISTS;
    }

    struct saguaro_subject_role *tmp = realloc(ctx->subject_roles,
        sizeof(*tmp)*(ctx->subject_role_count+1));

    if (tmp == NULL) {
        return SAGUARO_ERR_NOMEM;
    }

    ctx->subject_roles = tmp;
    ctx->subject_roles[ctx->subject_role_count].subject_id = subject_id;
    ctx->subject_roles[ctx->subject_role_count].role_id = role_id;
    ctx->subject_role_count++;

    return SAGUARO_OK;
}

saguaro_result_t
saguaro_unassign_role(saguaro_t *ctx, saguaro_subject_id_t subject_id,
    saguaro_role_id_t role_id)
{
    if (ctx == NULL) {
        return SAGUARO_ERR_INVALID;
    }

    for (size_t i = 0; i < ctx->subject_role_count; i++) {
        if (ctx->subject_roles[i].subject_id == subject_id &&
            ctx->subject_roles[i].role_id == role_id) {
            ctx->subject_roles[i] =
                ctx->subject_roles[ctx->subject_role_count-1];
            ctx->subject_role_count--;

            return SAGUARO_OK;
        }
    }

    return SAGUARO_ERR_NOT_FOUND;
}

saguaro_result_t
saguaro_assign_role_inheritance(saguaro_t *ctx,
    saguaro_role_id_t parent_role_id, saguaro_role_id_t child_role_id)
{
    if (ctx == NULL) {
        return SAGUARO_ERR_INVALID;
    }

    if (parent_role_id == child_role_id) {
        return SAGUARO_ERR_CYCLE;
    }

    if (role_inheritance_exists(ctx, parent_role_id, child_role_id)) {
        return SAGUARO_ERR_EXISTS;
    }

    if (role_reaches(ctx, child_role_id, parent_role_id)) {
        return SAGUARO_ERR_CYCLE;
    }

    struct saguaro_role_inheritance *tmp = realloc(ctx->role_inheritance,
        sizeof(*tmp)*(ctx->role_inheritance_count+1));
    if (tmp == NULL) {
        return SAGUARO_ERR_NOMEM;
    }

    ctx->role_inheritance = tmp;
    ctx->role_inheritance[
        ctx->role_inheritance_count].parent_role_id = parent_role_id;
    ctx->role_inheritance[
        ctx->role_inheritance_count].child_role_id = child_role_id;
    ctx->role_inheritance_count++;

    return SAGUARO_OK;
}

saguaro_result_t
saguaro_unassign_role_inheritance(saguaro_t *ctx,
    saguaro_role_id_t parent_role_id, saguaro_role_id_t child_role_id)
{
    if (ctx == NULL) {
        return SAGUARO_ERR_INVALID;
    }

    for (size_t i = 0; i < ctx->role_inheritance_count; i++) {
        if (ctx->role_inheritance[i].parent_role_id ==
                parent_role_id &&
            ctx->role_inheritance[i].child_role_id ==
                child_role_id) {

            ctx->role_inheritance[i] =
                ctx->role_inheritance[
                    ctx->role_inheritance_count - 1];

            ctx->role_inheritance_count--;

            return SAGUARO_OK;
        }
    }

    return SAGUARO_ERR_NOT_FOUND;
}

static bool
role_has_permission(const saguaro_t *ctx, saguaro_role_id_t role_id,
    saguaro_permission_id_t permission_id)
{
    for (size_t i = 0; i < ctx->role_permission_count; i++) {
        if (ctx->role_permissions[i].role_id ==
                role_id &&
            ctx->role_permissions[i].permission_id ==
                permission_id) {
            return true;
        }
    }

    for (size_t i = 0; i < ctx->role_inheritance_count; i++) {
        if (ctx->role_inheritance[i].parent_role_id !=
                role_id) {
            continue;
        }

        if (role_has_permission(
                ctx,
                ctx->role_inheritance[i].child_role_id,
                permission_id)) {
            return true;
        }
    }

    return false;
}

saguaro_result_t
saguaro_assign_permission(saguaro_t *ctx, saguaro_role_id_t role_id,
                          saguaro_permission_id_t permission_id)
{
    if (ctx == NULL) {
        return SAGUARO_ERR_INVALID;
    }

    if (role_permission_exists(ctx, role_id, permission_id)) {
        return SAGUARO_ERR_EXISTS;
    }

    struct saguaro_role_permission *tmp = realloc(ctx->role_permissions,
        sizeof(*tmp)*(ctx->role_permission_count+1));
    if (tmp == NULL) {
        return SAGUARO_ERR_NOMEM;
    }

    ctx->role_permissions = tmp;
    ctx->role_permissions[ctx->role_permission_count].role_id = role_id;
    ctx->role_permissions[ctx->role_permission_count].permission_id =
        permission_id;
    ctx->role_permission_count++;

    return SAGUARO_OK;
}

saguaro_result_t
saguaro_unassign_permission(saguaro_t *ctx, saguaro_role_id_t role_id,
                            saguaro_permission_id_t permission_id)
{
    if (ctx == NULL) {
        return SAGUARO_ERR_INVALID;
    }

    for (size_t i = 0; i < ctx->role_permission_count; i++) {
        if (ctx->role_permissions[i].role_id == role_id &&
            ctx->role_permissions[i].permission_id == permission_id) {
            ctx->role_permissions[i] =
                ctx->role_permissions[ctx->role_permission_count-1];
            ctx->role_permission_count--;

            return SAGUARO_OK;
        }
    }

    return SAGUARO_ERR_NOT_FOUND;
}

bool
saguaro_is_authorized(const saguaro_t *ctx, saguaro_subject_id_t subject_id,
    saguaro_permission_id_t permission_id)
{
    if (ctx == NULL) {
        return false;
    }

    for (size_t i = 0; i < ctx->subject_role_count; i++) {
        if (ctx->subject_roles[i].subject_id !=
                subject_id) {
            continue;
        }

        if (role_has_permission(ctx, ctx->subject_roles[i].role_id,
            permission_id)) {
            return true;
        }
    }

    return false;
}