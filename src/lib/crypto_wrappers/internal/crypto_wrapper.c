#include "internal/crypto_wrapper.h"

crypto_wrapper_opts_t *crypto_wrappers_opts[CRYPTO_WRAPPER_TYPE_MAX];
unsigned int registerd_crypto_wrapper_nums;

crypto_wrapper_ctx_t *crypto_wrappers_ctx[CRYPTO_WRAPPER_TYPE_MAX];
unsigned int crypto_wrappers_nums;
