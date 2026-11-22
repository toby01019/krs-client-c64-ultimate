# Experimental opt-in integration. Include immediately BEFORE rules.mk in
# target/u2plus/nios/ultimate/Makefile (after OPTIONS/PATH_INC definitions).
KRS_TLS_DIR := $(PATH_SW)/io/krs_tls
KRS_BEAR := $(KRS_TLS_DIR)/bearssl-0.6
KRS_BEAR_DIRS := $(sort $(dir $(wildcard $(KRS_BEAR)/src/*.c $(KRS_BEAR)/src/*/*.c)))
VPATH += $(KRS_TLS_DIR) $(KRS_BEAR_DIRS)
PATH_INC += -I$(KRS_TLS_DIR) -I$(KRS_BEAR)/inc -I$(KRS_BEAR)/src
SRCS_CC += krs_tls_target.cc
SRCS_C += krs_tls.c $(notdir $(wildcard $(KRS_BEAR)/src/*.c $(KRS_BEAR)/src/*/*.c))
OPTIONS += -DBR_USE_URANDOM=0 -DBR_USE_WIN32_RAND=0 -DBR_USE_WIN32_TIME=0
OPTIONS += -DBR_RDRAND=0 -DBR_AES_X86NI=0 -DBR_SSE2=0 -DBR_INT128=0 -DBR_UMUL128=0 -DBR_POWER8=0
# No OS RNG fallback: the C64 supplies 32 fresh HMAC-DRBG bytes per open.
# The entropy source is inherited from the experimental C64 client, not
# independently certified. Firmware image/stack size MUST be measured on U2+.
