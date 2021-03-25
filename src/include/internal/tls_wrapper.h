/* *INDENT-OFF* */
#ifndef _INTERNAL_TLS_WRAPPER_H
#define _INTERNAL_TLS_WRAPPER_H
/* *INDENT-ON* */

#include <enclave-tls/tls_wrapper.h>
#include "internal/core.h"

#define TLS_WRAPPERS_PATH "/opt/enclave-tls/lib/tls-wrappers"

extern enclave_tls_err_t etls_tls_wrapper_load_all(void);
extern enclave_tls_err_t etls_tls_wrapper_load_single(const char *);
extern enclave_tls_err_t etls_tls_wrapper_select(etls_core_context_t *,
						 const char *);

extern tls_wrapper_ctx_t *tls_wrappers_ctx[TLS_WRAPPER_TYPE_MAX];
extern tls_wrapper_opts_t *tls_wrappers_opts[TLS_WRAPPER_TYPE_MAX];
extern unsigned int tls_wrappers_nums;
extern unsigned registerd_tls_wrapper_nums;


/* *INDENT-OFF* */
#endif /* _INTERNAL_TLS_WRAPPER_H */
/* *INDENT-ON* */
