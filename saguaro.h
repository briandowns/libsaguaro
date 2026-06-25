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

#ifndef __SAGUARO_H
#define __SAGUARO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct saguaro saguaro_t;

#ifdef SAGUARO_32BIT_IDS
typedef uint32_t saguaro_id_t;
#else
typedef uint64_t saguaro_id_t;
#endif
typedef saguaro_id_t saguaro_subject_id_t;
typedef saguaro_id_t saguaro_role_id_t;
typedef saguaro_id_t saguaro_permission_id_t;

typedef enum {
    SAGUARO_OK = 0,
    SAGUARO_ERR,
    SAGUARO_ERR_NOMEM,
    SAGUARO_ERR_EXISTS,
    SAGUARO_ERR_NOT_FOUND,
    SAGUARO_ERR_INVALID,
    SAGUARO_ERR_CYCLE
} saguaro_result_t;

/**
 * Initialize a new Saguaro pointer.
 */
saguaro_t*
saguaro_init(void);

/**
 * Free the memory associated with a Saguaro pointer.
 */
void
saguaro_free(saguaro_t *ctx);

/**
 * Assign a role to a subject.
 */
saguaro_result_t
saguaro_assign_role(saguaro_t *ctx, saguaro_subject_id_t subject_id,
                    saguaro_role_id_t role_id);

/**
 * Unassign a role from a subject.
 */
saguaro_result_t
saguaro_unassign_role(saguaro_t *ctx, saguaro_subject_id_t subject_id,
                      saguaro_role_id_t role_id);

saguaro_result_t
saguaro_assign_role_inheritance(saguaro_t *ctx,
    saguaro_role_id_t parent_role_id, saguaro_role_id_t child_role_id);

saguaro_result_t
saguaro_unassign_role_inheritance(saguaro_t *ctx,
    saguaro_role_id_t parent_role_id, saguaro_role_id_t child_role_id);

/**
 * Assign a permission to a role.
 */
saguaro_result_t
saguaro_assign_permission(saguaro_t *ctx, saguaro_role_id_t role_id,
                          saguaro_permission_id_t permission_id);

/**
 * Unassign a permission from a role.
 */
saguaro_result_t
saguaro_unassign_permission(saguaro_t *ctx, saguaro_role_id_t role_id,
                            saguaro_permission_id_t permission_id);

/**
 * Check if a subject is authorized to perform a specific action.
 */
bool
saguaro_is_authorized(const saguaro_t *ctx, saguaro_subject_id_t subject_id,
                      saguaro_permission_id_t permission_id);

#ifdef __cplusplus
}
#endif
#endif /** end __SAGUARO_H */
