#include "libbech32.h"
#include <string.h>
#include <stdio.h>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <assert.h>

void encodeAndDecode() {

    // hrp and data to encode
    char hrp[] = "example";
    unsigned char dp[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    // create output for bech32 string
    bech32_bstring *bstr = bech32_create_bstring(sizeof(hrp), sizeof(dp));

    // expected bech32 string output
    char expected[] = "example1qpzry9x8ge8sqgv";

    // encode
    assert(bech32_encode(bstr, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstr->bstr) == 0);

    // allocate memory for decoded data
    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr->bstr);

    // decode
    assert(bech32_decode(hrpdp, bstr->bstr) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, "example") == 0);
    assert(hrpdp->dp[0] == 0);
    assert(hrpdp->dp[8] == 8);
    assert(ENCODING_BECH32M == hrpdp->encoding);

    // free memory
    bech32_free_HrpAndDp(hrpdp);
    bech32_free_bstring(bstr);
}

void decodeAndEncode() {

    // bech32 string with extra invalid characters
    char bstr[] = " example1:qpz!r--y9#x8&%&%ge-8-sqgv ";
    // expected bech32 string output
    char expected[] = "example1qpzry9x8ge8sqgv";

    // make sure to strip invalid characters before allocating storage and trying to decode
    assert(bech32_stripUnknownChars(bstr, sizeof(bstr), bstr, sizeof(bstr)) == E_BECH32_SUCCESS);

    // allocate memory for decoded data
    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr);

    // decode
    assert(bech32_decode(hrpdp, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, "example") == 0);
    assert(hrpdp->dp[0] == 0);
    assert(hrpdp->dp[8] == 8);

    // create output array for bech32 string
    size_t bstr2len = bech32_compute_encoded_string_length(hrpdp->hrplen, hrpdp->dplen);
    bech32_bstring *bstr2 = bech32_create_bstring_from_HrpAndDp(hrpdp);

    // encode
    assert(bech32_encode(bstr2, hrpdp->hrp, hrpdp->dp, hrpdp->dplen) == E_BECH32_SUCCESS);

    // encoding of "cleaned" decoded data should match expected string
    assert(strcmp(expected, bstr2->bstr) == 0);

    // free memory
    bech32_free_HrpAndDp(hrpdp);
    bech32_free_bstring(bstr2);
}

void badEncoding() {

    // hrp and data to encode
    char hrp[] = "example";
    unsigned char dp[] = {0, 1, 2, 3, 4, 5, 6, 7, 33};

    // create output for bech32 string
    bech32_bstring *bstr = bech32_create_bstring(sizeof(hrp), sizeof(dp));

    // encode
    assert(bech32_encode(bstr, hrp, dp, sizeof(dp)) == E_BECH32_UNKNOWN_ERROR);

    // free memory
    bech32_free_bstring(bstr);
}

void badDecoding_corruptData() {

    // bech32 string
    char bstr[] = "example1qpzry9x8ge8sqgv";
    // simulate corrupted data--checksum verification will fail
    bstr[10] = 'x';

    // allocate memory for decoded data
    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr);

    // decode
    assert(bech32_decode(hrpdp, bstr) == E_BECH32_INVALID_CHECKSUM);

    // free memory
    bech32_free_HrpAndDp(hrpdp);
}

void badDecoding_corruptChecksum() {

    // bech32 string
    char bstr[] = "example1qpzry9x8ge8sqgv";
    // simulate corrupted checksum--verification will fail
    bstr[19] = 'q';

    // allocate memory for decoded data
    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr);

    // decode
    assert(bech32_decode(hrpdp, bstr) == E_BECH32_INVALID_CHECKSUM);

    // free memory
    bech32_free_HrpAndDp(hrpdp);
}

int main() {
    encodeAndDecode();
    decodeAndEncode();
    badEncoding();
    badDecoding_corruptData();
    badDecoding_corruptChecksum();
}
