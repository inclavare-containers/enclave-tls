/* Copyright (c) 2021 Intel Corporation
 * Copyright (c) 2020-2021 Alibaba Cloud
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <enclave-tls/err.h>
#include <enclave-tls/log.h>
#include "internal/core.h"
#include "internal/verifier.h"

#define PATTERN_PREFIX "libverifier_"
#define PATTERN_SUFFIX ".so"

enclave_tls_err_t etls_enclave_verifier_load_single(const char *fname)
{
	ETLS_DEBUG("loading the enclave verifier instance '%s' ...\n", fname);

	/* Check whether the filename pattern matches up libenclave_verifier_<name>.so */
	if (strlen(fname) <= strlen(PATTERN_PREFIX) + strlen(PATTERN_SUFFIX) ||
	    strncmp(fname, PATTERN_PREFIX, strlen(PATTERN_PREFIX)) ||
	    strncmp(fname + strlen(fname) - strlen(PATTERN_SUFFIX), PATTERN_SUFFIX,
		    strlen(PATTERN_SUFFIX))) {
		ETLS_ERR("The filename pattern of '%s' NOT match " PATTERN_PREFIX
			 "<name>" PATTERN_SUFFIX "\n",
			 fname);
		return -ENCLAVE_TLS_ERR_INVALID;
	}

	char realpath[strlen(ENCLAVE_VERIFIERS_DIR) + strlen(fname) + 1];
	sprintf(realpath, "%s%s", ENCLAVE_VERIFIERS_DIR, fname);

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
	enclave_verifier_opts_t *opts = NULL;
	for (; i < registerd_enclave_verifier_nums; ++i) {
		opts = enclave_verifiers_opts[i];

		if (!strcmp(name, opts->name))
			break;
	}

	if (i == registerd_enclave_verifier_nums) {
		ETLS_DEBUG("the enclave verifier '%s' failed to be registered\n", name);
		return -ENCLAVE_TLS_ERR_NOT_REGISTERED;
	}

	if (opts->pre_init) {
		enclave_tls_err_t err = opts->pre_init();

		if (err != ENCLAVE_VERIFIER_ERR_NONE) {
			ETLS_ERR("failed on pre_init() of enclave verifier '%s' %#x\n", name, err);
			return err;
		}
	}

	enclave_verifier_ctx_t *verifier_ctx = calloc(1, sizeof(*verifier_ctx));
	if (!verifier_ctx)
		return -ENCLAVE_TLS_ERR_NO_MEM;

	verifier_ctx->opts = opts;
	verifier_ctx->log_level = global_core_context.config.log_level;
	verifier_ctx->handle = handle;

	enclave_verifiers_ctx[enclave_verifier_nums++] = verifier_ctx;

	ETLS_DEBUG("the enclave verifier '%s' loaded\n", name);

	return ENCLAVE_TLS_ERR_NONE;
}
