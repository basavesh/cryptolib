#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "api.h"
#include "randombytes.h"

// https://stackoverflow.com/a/1489985/1711232
#define PASTER(x, y) x##_##y
#define EVALUATOR(x, y) PASTER(x, y)
#define NAMESPACE(fun) EVALUATOR(JADE_NAMESPACE, fun)
#define NAMESPACE_LC(fun) EVALUATOR(JADE_NAMESPACE_LC, fun)

#define CRYPTO_BYTES           NAMESPACE(BYTES)
#define CRYPTO_PUBLICKEYBYTES  NAMESPACE(PUBLICKEYBYTES)
#define CRYPTO_SECRETKEYBYTES  NAMESPACE(SECRETKEYBYTES)
#define CRYPTO_CIPHERTEXTBYTES NAMESPACE(CIPHERTEXTBYTES)
#define CRYPTO_ALGNAME         NAMESPACE(ALGNAME)

#define crypto_kem_keypair NAMESPACE_LC(keypair)
#define crypto_kem_enc NAMESPACE_LC(enc)
#define crypto_kem_dec NAMESPACE_LC(dec)

void nist_kat_init(unsigned char *entropy_input, unsigned char *personalization_string, int security_strength);

static void fprintBstr(FILE *fp, const char *S, const uint8_t *A, size_t L) {
    size_t i;
    fprintf(fp, "%s", S);
    for (i = 0; i < L; i++) {
        fprintf(fp, "%02X", A[i]);
    }
    if (L == 0) {
        fprintf(fp, "00");
    }
    fprintf(fp, "\n");
}

int main() {

    uint8_t entropy_input[48];
    uint8_t seed[48];
    FILE *fh = stdout;
    uint8_t public_key[CRYPTO_PUBLICKEYBYTES];
    uint8_t secret_key[CRYPTO_SECRETKEYBYTES];
    uint8_t ciphertext[CRYPTO_CIPHERTEXTBYTES];
    uint8_t shared_secret_e[CRYPTO_BYTES];
    uint8_t shared_secret_d[CRYPTO_BYTES];
    int rc;

    uint8_t rnd[2*CRYPTO_BYTES]; // TODO: TO REMOVE (after randomness support in jasmin)

    for (uint8_t i = 0; i < 48; i++) {
        entropy_input[i] = i;
    }

    nist_kat_init(entropy_input, NULL, 256);

    fprintf(fh, "count = 0\n");
    randombytes(seed, 48);
    fprintBstr(fh, "seed = ", seed, 48);

    nist_kat_init(seed, NULL, 256);

    randombytes(rnd, CRYPTO_BYTES); // TODO: meanwhile, how to initialize rnd?
    randombytes(rnd+CRYPTO_BYTES, CRYPTO_BYTES); // TODO: meanwhile, how to initialize rnd?
    rc = crypto_kem_keypair(public_key, secret_key, rnd);
    if (rc != 0) {
        fprintf(stderr, "[kat_kem] %s ERROR: crypto_kem_keypair failed!\n", CRYPTO_ALGNAME);
        return -1;
    }
    fprintBstr(fh, "pk = ", public_key, CRYPTO_PUBLICKEYBYTES);
    fprintBstr(fh, "sk = ", secret_key, CRYPTO_SECRETKEYBYTES);

    randombytes(rnd, CRYPTO_BYTES); // TODO: meanwhile, how to initialize rnd?
    rc = crypto_kem_enc(ciphertext, shared_secret_e, public_key, rnd);
    if (rc != 0) {
        fprintf(stderr, "[kat_kem] %s ERROR: crypto_kem_enc failed!\n", CRYPTO_ALGNAME);
        return -2;
    }
    fprintBstr(fh, "ct = ", ciphertext, CRYPTO_CIPHERTEXTBYTES);
    fprintBstr(fh, "ss = ", shared_secret_e, CRYPTO_BYTES);

    rc = crypto_kem_dec(shared_secret_d, ciphertext, secret_key);
    if (rc != 0) {
        fprintf(stderr, "[kat_kem] %s ERROR: crypto_kem_dec failed!\n", CRYPTO_ALGNAME);
        return -3;
    }

    rc = memcmp(shared_secret_e, shared_secret_d, CRYPTO_BYTES);
    if (rc != 0) {
        fprintf(stderr, "[kat_kem] %s ERROR: shared secrets are not equal\n", CRYPTO_ALGNAME);
        return -4;
    }

    return 0;

}
