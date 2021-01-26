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
    bech32_bstring *bstr = bech32_create_bstring(sizeof(hrp), sizeof(dp));

    // encode
    assert(bech32_encode(bstr, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);

    // prints "hello1w0rldjn365x" : "hello" + Bech32.separator + encoded data + 6 char checksum
    printf("bech32 encoding of human-readable part \'hello\' and data part \'[14, 15, 3, 31, 13]\' is:\n");
    printf("%s\n", bstr->bstr);

    // allocate memory for decoded data
    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr->bstr);

    // decode
    assert(bech32_decode(hrpdp, bstr->bstr) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, hrp) == 0);
    assert(hrpdp->dp[0] == dp[0]);
    assert(hrpdp->dp[4] == dp[4]);
    assert(ENCODING_BECH32M == hrpdp->encoding);

    // free memory
    bech32_free_HrpAndDp(hrpdp);
    bech32_free_bstring(bstr);
}
