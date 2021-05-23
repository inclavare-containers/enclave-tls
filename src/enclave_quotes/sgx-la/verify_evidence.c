/* Copyright (c) 2020-2021 Alibaba Cloud and Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <enclave-tls/log.h>
#include <enclave-tls/enclave_quote.h>
#include "sgx_error.h"
#include "sgx_la.h"
#include "sgx_stub_u.h"
#include "sgx_quote_3.h"
#include "sgx_dcap_ql_wrapper.h"

/* Refer to explanation in sgx_la_collect_evidence */
enclave_quote_err_t sgx_la_verify_evidence(enclave_quote_ctx_t *ctx,
					   attestation_evidence_t *evidence, uint8_t *hash,
					   unsigned int hash_len)
{
	uint32_t quote_size = 0;
	unsigned char quote[8192];
	sgx_target_info_t qe_target_info;
	quote3_error_t qe3_ret = SGX_QL_SUCCESS;

	ETLS_DEBUG("ctx %p, evidence %p, hash %p\n", ctx, evidence, hash);

	// First verify hash value
	sgx_report_t *lreport = (sgx_report_t *)evidence->la.report;

	if (memcmp(hash, lreport->body.report_data.d, hash_len) != 0) {
		ETLS_ERR("Unmatched hash value in evidence\n");
		return -ENCLAVE_QUOTE_ERR_INVALID;
	}

	qe3_ret = sgx_qe_get_target_info(&qe_target_info);
	if (SGX_QL_SUCCESS != qe3_ret) {
		ETLS_ERR("failed to get QE's target info %04x\n", qe3_ret);
		return SGX_LA_ERR_CODE((int)qe3_ret);
	}

	qe3_ret = sgx_qe_get_quote_size(&quote_size);
	if (SGX_QL_SUCCESS != qe3_ret) {
		ETLS_ERR("failed to get quote size %04x\n", qe3_ret);
		return SGX_LA_ERR_CODE((int)qe3_ret);
	}

	qe3_ret = sgx_qe_get_quote((sgx_report_t *)evidence->la.report, quote_size, quote);
	if (SGX_QL_SUCCESS != qe3_ret) {
		ETLS_ERR("failed to get quote %04x\n", qe3_ret);
		return SGX_LA_ERR_CODE((int)qe3_ret);
	}

	return ENCLAVE_QUOTE_ERR_NONE;
}
