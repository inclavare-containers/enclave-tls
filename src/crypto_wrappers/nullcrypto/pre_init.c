#include <enclave-tls/log.h>
#include <enclave-tls/crypto_wrapper.h>

crypto_wrapper_err_t nullcrypto_pre_init(void)
{
	ETLS_DEBUG("called\n");

	return CRYPTO_WRAPPER_ERR_NONE;
}