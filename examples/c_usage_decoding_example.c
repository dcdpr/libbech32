#include "bech32.h"
#include <string.h>
#include <stdio.h>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <assert.h>

int main() {

    char str[] = "hello1w0rldjn365x";

    // create storage for decoded bech32 data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(str);
    if(!decodedResult) {
        printf("bech32 DecodedResult can not be created");
        return E_BECH32_NO_MEMORY;
    }

    // decode
    bech32_error err = bech32_decode(decodedResult, str);
    if(err != E_BECH32_SUCCESS) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_DecodedResult(decodedResult);
        return err;
    }

    // check for expected values (see c_usage_encoding_example.c)
    assert(strcmp(decodedResult->hrp, "hello") == 0);
    assert(decodedResult->dp[0] == 14);
    assert(decodedResult->dp[4] == 13);
    assert(ENCODING_BECH32M == decodedResult->encoding);

    // free memory
    bech32_free_DecodedResult(decodedResult);
}
