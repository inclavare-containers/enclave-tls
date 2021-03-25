Topdir := $(shell readlink -f .)

export Topdir

include $(Topdir)/rules/build_env.mk

client_dir := $(Topdir)/samples/enclave-tls-client
server_dir := $(Topdir)/samples/enclave-tls-server
stub_dir := $(Topdir)/samples/sgx-stub-enclave
dirs := $(Enclave_Tls_Srcdir) $(client_dir) $(server_dir) $(stub_dir)

all: $(Build_Bindir)/enclave-tls-client $(Build_Bindir)/enclave-tls-server $(stub_dir)/sgx_stub_enclave.signed.so
	make -C $(Enclave_Tls_Srcdir)

$(Build_Bindir)/enclave-tls-client:
	make -C $(client_dir)

$(Build_Bindir)/enclave-tls-server:
	make -C $(server_dir)

$(stub_dir)/sgx_stub_enclave.signed.so:
	make -C $(stub_dir)

Cleans += $(Build_Dir)

define clean_all
  for d in $(dirs); do \
    make -C $$d $@; \
  done
endef
Clean_Cmds += clean_all

clean:
	@rm -rf $(Build_Dir)
	for d in $(dirs); do \
	  make -C $$d $@; \
	done

install: all
	for d in $(dirs); do \
	  make -C $$d $@ || exit 1; \
	done

uninstall:
	for d in $(dirs); do \
	  make -C $$d $@; \
	done

#include $(Topdir)/rules/build_rules.mk
