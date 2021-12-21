#include <stdio.h>
#include "api.h"

#define PASTER(x, y) x##_##y
#define EVALUATOR(x, y) PASTER(x, y)
#define NAMESPACE(fun) EVALUATOR(JADE_NAMESPACE, fun)
#define NAMESPACE_LC(fun) EVALUATOR(JADE_NAMESPACE_LC, fun)

int main() {
    printf("{\n");
    printf("\t\"CRYPTO_SECRETKEYBYTES\": %u,\n",  NAMESPACE(SECRETKEYBYTES));
    printf("\t\"CRYPTO_PUBLICKEYBYTES\": %u,\n",  NAMESPACE(PUBLICKEYBYTES));
    printf("\t\"CRYPTO_CIPHERTEXTBYTES\": %u,\n", NAMESPACE(CIPHERTEXTBYTES));
    printf("\t\"CRYPTO_BYTES\": %u,\n",           NAMESPACE(BYTES));
    printf("\t\"CRYPTO_ALGNAME\": \"%s\"\n}\n",   NAMESPACE(ALGNAME));
}
