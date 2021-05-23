/* Copyright (c) 2020-2021 Alibaba Cloud and Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <enclave-tls/err.h>
#include <enclave-tls/log.h>
#include "internal/core.h"
#include "internal/tls_wrapper.h"

#define PATTERN_PREFIX "libtls_wrapper_"
#define PATTERN_SUFFIX ".so"

enclave_tls_err_t etls_tls_wrapper_load_single(const char *fname)
{
	ETLS_DEBUG("loading the tls wrapper instance '%s' ...\n", fname);

	/* Check whether the filename pattern matches up libtls_wrapper_<name>.so */
	if (strlen(fname) <= strlen(PATTERN_PREFIX) + strlen(PATTERN_SUFFIX) ||
	    strncmp(fname, PATTERN_PREFIX, strlen(PATTERN_PREFIX)) ||
	    strncmp(fname + strlen(fname) - strlen(PATTERN_SUFFIX), PATTERN_SUFFIX,
		    strlen(PATTERN_SUFFIX))) {
		ETLS_ERR("The filename pattern of '%s' NOT match " PATTERN_PREFIX
			 "<name>" PATTERN_SUFFIX "\n",
			 fname);
		return -ENCLAVE_TLS_ERR_INVALID;
	}

	char realpath[strlen(TLS_WRAPPERS_DIR) + strlen(fname) + 1];
	sprintf(realpath, "%s%s", TLS_WRAPPERS_DIR, fname);

	void *handle = dlopen(realpath, RTLD_LAZY);
	if (!handle) {
		ETLS_ERR("failed on dlopen(): %s\n", dlerror());
		return -ENCLAVE_TLS_ERR_DLOPEN;
	}

	size_t name_len = strlen(fname) - strlen(PATTERN_PREFIX) - strlen(PATTERN_SUFFIX);
	char name[name_len + 1];
	strncpy(name, fname + strlen(PATTERN_PREFIX), name_len);
	name[name_len] = '\0';

	unsigned int i = 0;
	tls_wrapper_opts_t *opts = NULL;
	for (i = 0; i < registerd_tls_wrapper_nums; ++i) {
		opts = tls_wrappers_opts[i];

		if (!strcmp(name, opts->name))
			break;
	}

	if (i == registerd_tls_wrapper_nums) {
		ETLS_ERR("the tls wrapper '%s' is not registered yet\n", name);
		return -ENCLAVE_TLS_ERR_NOT_REGISTERED;
	}

	if (opts->pre_init) {
		enclave_tls_err_t err = opts->pre_init();

		if (err != TLS_WRAPPER_ERR_NONE) {
			ETLS_ERR("failed on pre_init() of tls wrapper '%s' %#x\n", name, err);
			return err;
		}
	}

	tls_wrapper_ctx_t *tls_ctx = calloc(1, sizeof(*tls_ctx));
	if (!tls_ctx)
		return -ENCLAVE_TLS_ERR_NO_MEM;

	tls_ctx->opts = opts;
	tls_ctx->fd = -1;
	tls_ctx->conf_flags = global_core_context.config.flags;
	tls_ctx->log_level = global_core_context.config.log_level;
	tls_ctx->handle = handle;

	tls_wrappers_ctx[tls_wrappers_nums++] = tls_ctx;

	ETLS_DEBUG("the tls wrapper '%s' loaded\n", name);

	return ENCLAVE_TLS_ERR_NONE;
}
