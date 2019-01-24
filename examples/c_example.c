#include "libbech32.h"
#include <string.h>
#include <stdio.h>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <assert.h>

int main() {

    // hrp and data to encode
    char hrp[] = "example";
    unsigned char dp[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    // create output array for bech32 string
    char bstr[sizeof(hrp) + 1 + sizeof(dp) + 6] = {0};

    // expected bech32 string output
    char expected[] = "example1qpzry9x8gvmqvdw";

    // encode
    assert(bech32_encode(bstr, sizeof(bstr), hrp, sizeof(hrp), dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstr) == 0);

    printf("bech32 encoding of human-readable part \'example\' and data part \'[0, 1, 2, 3, 4, 5, 6, 7, 8]\' is:\n");
    printf("%s\n", bstr);

    // allocate memory for decoded data
    bech32_HrpAndDp * hrpdp = create_HrpAndDp_storage(bstr);

    // decode
    assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, "example") == 0);
    assert(hrpdp->dp[0] == 0);
    assert(hrpdp->dp[8] == 8);

    // free memory
    free_HrpAndDp_storage(hrpdp);

}
