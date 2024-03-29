# This Makefile can be used with GNU Make or BSD Make

# override as desired
TYPE=kem
SCHEME=kyber/kyber768
IMPLEMENTATION=amd64/avx2

TOP_DIR:=$(shell git rev-parse --show-toplevel)
SRC_DIR=$(TOP_DIR)/src
SCHEME_DIR=$(SRC_DIR)/crypto_$(TYPE)/$(SCHEME)/$(IMPLEMENTATION)
SCHEME_INCLUDE_DIR=$(SRC_DIR)/crypto_$(TYPE)/$(SCHEME)/$(IMPLEMENTATION)/include

TYPE_UPPERCASE=$(shell echo $(TYPE) | tr a-z A-Z | tr / _ | sed 's/-//g')
SCHEME_UPPERCASE:=$(shell echo $(SCHEME) | tr a-z A-Z | tr / _ | sed 's/-//g')
IMPLEMENTATION_UPPERCASE:=$(shell echo $(IMPLEMENTATION) | tr a-z A-Z | tr / _ | sed 's/-//g')
JADE_NAMESPACE=JADE_$(TYPE_UPPERCASE)_$(SCHEME_UPPERCASE)_$(IMPLEMENTATION_UPPERCASE)

TYPE_LOWERCASE:=$(shell echo $(TYPE) | tr A-Z a-z | tr / _ | sed 's/-//g')
SCHEME_LOWERCASE:=$(shell echo $(SCHEME) | tr A-Z a-z | tr / _ | sed 's/-//g')
IMPLEMENTATION_LOWERCASE:=$(shell echo $(IMPLEMENTATION) | tr A-Z a-z | tr / _ | sed 's/-//g')
JADE_NAMESPACE_LC=jade_$(TYPE_LOWERCASE)_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)

TEST_DIR=$(TOP_DIR)/test
TEST_COMMON_DIR=$(TEST_DIR)/common1# prev TOP_DIR/test/common # TODO CHECKME
COMMON_DIR=$(TEST_DIR)/common2# prev TOP_DIR/common # TODO CHECKME
COMMON_FILES=$(COMMON_DIR)/aes.c $(COMMON_DIR)/sha2.c $(COMMON_DIR)/fips202.c $(COMMON_DIR)/nistseedexpander.c $(COMMON_DIR)/sp800-185.c
COMMON_HEADERS=$(COMMON_DIR)/*.h
DEST_DIR=$(TEST_DIR)/bin

SCHEME_SRC:=$(SCHEME_DIR)/$(TYPE).jazz
SCHEME_ASM:=$(SCHEME_SRC:%.jazz=%.s)
SCHEME_OBJ:=$(SCHEME_ASM:%.s=%.o)
SCHEME_LIBRARY:=$(SCHEME_DIR)/lib$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE).a

# This -Wall was supported by the European Commission through the ERC Starting Grant 805031 (EPOQUE)
CFLAGS=-O3 -Wall -Wextra -Wpedantic -Wvla -Werror -std=c99 \
	   -Wundef -Wshadow -Wcast-align -Wpointer-arith -Wmissing-prototypes\
	   -fstrict-aliasing -fno-common -pipe \
	   -I$(COMMON_DIR) $(EXTRAFLAGS)

# Number of tests run for functests
NTESTS=1

all: $(DEST_DIR)/functest_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE) \
	$(DEST_DIR)/testvectors_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE) \
	$(DEST_DIR)/printparams_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)

.PHONY: build-scheme
build-scheme: $(SCHEME_LIBRARY)

# TODO CHECKME
$(SCHEME_DIR)/%.s : $(SCHEME_DIR)/%.jazz
	$(MAKE) -C $(SCHEME_DIR) clean && $(MAKE) -C $(SCHEME_DIR)

$(SCHEME_LIBRARY): $(SCHEME_OBJ)
	ar -rc $@ $? && ranlib $@

.PHONY: clean-scheme
clean-scheme:
	cd $(SCHEME_DIR) && $(MAKE) clean

.PHONY: functest
functest: $(DEST_DIR)/functest_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)

.PHONY: testvectors
testvectors: $(DEST_DIR)/testvectors_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)

.PHONY: printparams
printparams: $(DEST_DIR)/printparams_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)

.PHONY: nistkat
nistkat: $(DEST_DIR)/nistkat_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)

$(DEST_DIR)/test_common_aes: test_common/aes.c $(COMMON_FILES)
	mkdir -p $(DEST_DIR)
	$(CC) $(CFLAGS) $< $(COMMON_FILES) -o $@
$(DEST_DIR)/test_common_fips202: test_common/fips202.c $(COMMON_FILES)
	mkdir -p $(DEST_DIR)
	$(CC) $(CFLAGS) $< $(COMMON_FILES) -o $@
$(DEST_DIR)/test_common_sha2: test_common/sha2.c $(COMMON_FILES)
	mkdir -p $(DEST_DIR)
	$(CC) $(CFLAGS) $< $(COMMON_FILES) -o $@
$(DEST_DIR)/test_common_sp800-185: test_common/sp800-185.c $(COMMON_FILES)
	mkdir -p $(DEST_DIR)
	$(CC) $(CFLAGS) $< $(COMMON_FILES) -o $@

$(DEST_DIR)/test_common_nistseedexpander: test_common/sp800-185.c $(COMMON_FILES)
	mkdir -p $(DEST_DIR)
	$(CC) $(CFLAGS) $< $(COMMON_FILES) -o $@

$(DEST_DIR)/functest_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE): $(SCHEME_LIBRARY) crypto_$(TYPE)/functest.c $(COMMON_FILES) $(COMMON_DIR)/randombytes.c $(COMMON_HEADERS)
	mkdir -p $(DEST_DIR)
	$(CC) $(CFLAGS) -DNTESTS=$(NTESTS) -DJADE_NAMESPACE=$(JADE_NAMESPACE) -DJADE_NAMESPACE_LC=$(JADE_NAMESPACE_LC) -I$(SCHEME_INCLUDE_DIR) crypto_$(TYPE)/functest.c $(COMMON_FILES) $(COMMON_DIR)/randombytes.c -o $@ -L$(SCHEME_DIR) -l$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)

$(DEST_DIR)/testvectors_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE): $(SCHEME_LIBRARY) crypto_$(TYPE)/testvectors.c $(COMMON_FILES) $(TEST_COMMON_DIR)/notrandombytes.c $(COMMON_HEADERS)
	mkdir -p $(DEST_DIR)
	$(CC) $(CFLAGS) -DJADE_NAMESPACE=$(JADE_NAMESPACE) -DJADE_NAMESPACE_LC=$(JADE_NAMESPACE_LC) -I$(SCHEME_INCLUDE_DIR) crypto_$(TYPE)/testvectors.c $(COMMON_FILES) $(TEST_COMMON_DIR)/notrandombytes.c -o $@ -L$(SCHEME_DIR) -l$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)

$(DEST_DIR)/printparams_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE): crypto_$(TYPE)/printparams.c
	mkdir -p $(DEST_DIR)
	$(CC) $(CFLAGS) -DJADE_NAMESPACE=$(JADE_NAMESPACE) -DJADE_NAMESPACE_LC=$(JADE_NAMESPACE_LC) -I$(SCHEME_INCLUDE_DIR) crypto_$(TYPE)/printparams.c -o $@

$(DEST_DIR)/nistkat_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE): $(SCHEME_LIBRARY) crypto_$(TYPE)/nistkat.c $(COMMON_FILES) $(TEST_COMMON_DIR)/nistkatrng.c $(COMMON_HEADERS)
	mkdir -p $(DEST_DIR)
	$(CC) $(CFLAGS) -DJADE_NAMESPACE=$(JADE_NAMESPACE) -DJADE_NAMESPACE_LC=$(JADE_NAMESPACE_LC) -I$(SCHEME_INCLUDE_DIR) crypto_$(TYPE)/nistkat.c $(COMMON_FILES) $(TEST_COMMON_DIR)/nistkatrng.c -o $@ -L$(SCHEME_DIR) -l$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)

.PHONY: clean
clean:
	$(RM) $(DEST_DIR)/functest_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)
	$(RM) $(DEST_DIR)/testvectors_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)
	$(RM) $(DEST_DIR)/printparams_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)
	$(RM) $(DEST_DIR)/nistkat_$(SCHEME_LOWERCASE)_$(IMPLEMENTATION_LOWERCASE)
	$(RM) $(DEST_DIR)/test_common_aes
	$(RM) $(DEST_DIR)/test_common_fips202
	$(RM) $(DEST_DIR)/test_common_sha2
	$(RM) $(DEST_DIR)/test_common_sp800-185
	$(RM) $(DEST_DIR)/test_common_nistseedexpander
	$(MAKE) -C $(SRC_DIR) clean # TODO CHECKME

.PHONY: distclean
distclean:
	$(RM) -r $(DEST_DIR)
