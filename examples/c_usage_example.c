// This small example is mainly meant for easy copy/paste into the README.md
#include "libbech32.h"
#include <string.h>
#include <stdio.h>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <assert.h>

int main() {
    // simple human readable part with some data
    char hrp[] = "hello";
    unsigned char dp[] = {14, 15, 3, 31, 13};

    // create output for bech32 string
    bech32_bstring *bstring = bech32_create_bstring(strlen(hrp), sizeof(dp));

    // encode
    assert(bech32_encode(bstring, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);

    // prints "hello1w0rldjn365x" : "hello" + Bech32.separator + encoded data + 6 char checksum
    printf("bech32 encoding of human-readable part \'hello\' and data part \'[14, 15, 3, 31, 13]\' is:\n");
    printf("%s\n", bstring->string);

    // allocate memory for decoded data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstring->string);

    // decode
    assert(bech32_decode(decodedResult, bstring->string) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, hrp) == 0);
    assert(decodedResult->dp[0] == dp[0]);
    assert(decodedResult->dp[4] == dp[4]);
    assert(ENCODING_BECH32M == decodedResult->encoding);

    // free memory
    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}
