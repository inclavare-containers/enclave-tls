#include <enclave-tls/log.h>
#include <enclave-tls/tls_wrapper.h>

tls_wrapper_err_t wolfssl_pre_init(void)
{
	ETLS_DEBUG("called\n");

	return TLS_WRAPPER_ERR_NONE;
}
