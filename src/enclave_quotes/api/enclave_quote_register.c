/* Copyright (c) 2020-2021 Alibaba Cloud and Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <enclave-tls/err.h>
#include <enclave-tls/log.h>
#include "internal/enclave_quote.h"
#include "internal/sgxutils.h"

enclave_quote_err_t enclave_quote_register(const enclave_quote_opts_t *opts)
{
	if (!opts)
		return -ENCLAVE_QUOTE_ERR_INVALID;

	ETLS_DEBUG("registering the enclave quote '%s' ...\n", opts->type);

	if (opts->flags & ENCLAVE_QUOTE_OPTS_FLAGS_SGX_ENCLAVE) {
		if (!is_sgx_supported_and_configured()) {
			ETLS_DEBUG("failed to register the enclave quote '%s' due to lack of SGX capability\n", opts->type);
			return -ENCLAVE_QUOTE_ERR_INVALID;
		}
	}

	enclave_quote_opts_t *new_opts =
		(enclave_quote_opts_t *)malloc(sizeof(*new_opts));
	if (!new_opts)
		return -ENCLAVE_QUOTE_ERR_NO_MEM;

	memcpy(new_opts, opts, sizeof(*new_opts));

	if (new_opts->type[0] == '\0') {
		ETLS_ERR("invalid enclave quote type\n");
		goto err;
	}

	if (new_opts->api_version > ENCLAVE_QUOTE_API_VERSION_MAX) {
		ETLS_ERR("unsupported enclave quote api version %d > %d\n",
			 new_opts->api_version, ENCLAVE_QUOTE_API_VERSION_MAX);
		goto err;
	}

	enclave_quotes_opts[registerd_enclave_quote_nums++] = new_opts;

	ETLS_INFO("the enclave quote '%s' registered\n", opts->type);

	return ENCLAVE_QUOTE_ERR_NONE;

err:
	free(new_opts);
	return -ENCLAVE_QUOTE_ERR_INVALID;
}
