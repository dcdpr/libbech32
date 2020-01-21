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

    // create output array for bech32 string
    char bstr[sizeof(hrp) + 1 + sizeof(dp) + 6] = {0};

    // encode
    assert(bech32_encode(bstr, sizeof(bstr), hrp, sizeof(hrp), dp, sizeof(dp)) == E_BECH32_SUCCESS);

    // prints "hello1w0rldcs7fw6" : "hello" + Bech32.separator + encoded data + 6 char checksum
    printf("bech32 encoding of human-readable part \'hello\' and data part \'[14, 15, 3, 31, 13]\' is:\n");
    printf("%s\n", bstr);

    // allocate memory for decoded data
    bech32_HrpAndDp * hrpdp = create_HrpAndDp_storage(bstr);

    // decode
    assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, "hello") == 0);

    // free memory
    free_HrpAndDp_storage(hrpdp);
}
