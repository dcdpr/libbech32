// test program calling bech32 library from C

#include "libbech32.h"
#include <string.h>
#include <stdlib.h>

// make sure we can run these tests even when building a release version
#undef NDEBUG
#include <assert.h>


void strerror_withValidErrorCode_returnsErrorMessage() {
    const char *message = bech32_strerror(E_BECH32_SUCCESS);
    assert(message != NULL);
    assert(strcmp(message, "Success") == 0);
}

void strerror_withInvalidErrorCode_returnsUnknownErrorMessage() {
    const char *message = bech32_strerror(1234);
    assert(message != NULL);
    assert(strcmp(message, "Unknown error") == 0);
}

void stripUnknownChars_withNullInput_returnsError() {
    char *src = NULL;
    char dst[] = "";

    assert(bech32_stripUnknownChars(dst, sizeof(dst), src, 0) == E_BECH32_NULL_ARGUMENT);
}

void stripUnknownChars_withNullOutput_returnsError() {
    char src[] = "foo";
    char *dst = NULL;

    // sizeof(dst) is meaningless here as we are testing dst == NULL
    assert(bech32_stripUnknownChars(dst, 0, src, sizeof(src)) == E_BECH32_NULL_ARGUMENT);
}

void stripUnknownChars_withInsufficientOutputStorage_returnsError() {
    char src[] = "ffff";
    char dst[sizeof(src)-1];

    assert(bech32_stripUnknownChars(dst, sizeof(dst), src, sizeof(src)) == E_BECH32_LENGTH_TOO_SHORT);
}

void stripUnknownChars_withSimpleString_returnsSameString() {
    char src[] = "ffff";
    char dst[sizeof(src)];

    bech32_stripUnknownChars(dst, sizeof(dst), src, sizeof(src));

    assert(strcmp(src, dst) == 0);
}

void stripUnknownChars_withComplexString_returnsStrippedString() {
    char src[] = "foobar";
    char expected[] = "far";
    char dst[sizeof(src)];

    bech32_stripUnknownChars(dst, sizeof(dst), src, sizeof(src));

    assert(strcmp(expected, dst) == 0);
}

void stripUnknownChars_withFunkyString_returnsStrippedString() {
    char src[] = "tx1!rjk0\\u5ng*4jsf^^mc";
    char expected[] = "tx1rjk0u5ng4jsfmc";
    char dst[sizeof(src)];

    bech32_stripUnknownChars(dst, sizeof(dst), src, sizeof(src));

    assert(strcmp(expected, dst) == 0);
}

// ----- tests using default checksum constant = M (0x2bc830a3) ------

void decode_withBadArgs_isUnsuccessful() {

    { // input is null
        char *bstr = NULL;

        bech32_HrpAndDp *hrpdp = malloc(sizeof(bech32_HrpAndDp));
        hrpdp->hrplen = 10;
        hrpdp->hrp = (char *) calloc(hrpdp->hrplen, 1);
        hrpdp->dplen = 10;
        hrpdp->dp = (unsigned char *) calloc(hrpdp->dplen, 1);

        assert(bech32_decode(hrpdp, bstr) == E_BECH32_NULL_ARGUMENT);

        free(hrpdp->dp);
        free(hrpdp->hrp);
        free(hrpdp);
    }

    { // output is null
        char bstr[] = "xyz1pzrs3usye";

        bech32_HrpAndDp *hrpdp = NULL;

        assert(bech32_decode(hrpdp, bstr) == E_BECH32_NULL_ARGUMENT);
    }

    { // hrp is null
        char bstr[] = "xyz1pzrs3usye";

        bech32_HrpAndDp *hrpdp = malloc(sizeof(bech32_HrpAndDp));
        hrpdp->dplen = 10;
        hrpdp->dp = (unsigned char *) calloc(hrpdp->dplen, 1);
        hrpdp->hrp = NULL;

        assert(bech32_decode(hrpdp, bstr) == E_BECH32_NULL_ARGUMENT);

        free(hrpdp->dp);
        free(hrpdp);
    }

    { // dp is null
        char bstr[] = "xyz1pzrs3usye";

        bech32_HrpAndDp *hrpdp = malloc(sizeof(bech32_HrpAndDp));
        hrpdp->hrplen = 1;
        hrpdp->hrp = (char *) calloc(hrpdp->hrplen, 1);
        hrpdp->dp = NULL;

        assert(bech32_decode(hrpdp, bstr) == E_BECH32_NULL_ARGUMENT);

        free(hrpdp->hrp);
        free(hrpdp);
    }

    { // allocated hrp is too short
        char bstr[] = "xyz1pzrs3usye";

        bech32_HrpAndDp *hrpdp = malloc(sizeof(bech32_HrpAndDp));
        hrpdp->hrplen = 1;
        hrpdp->hrp = (char *) calloc(hrpdp->hrplen, 1);
        hrpdp->dplen = 10;
        hrpdp->dp = (unsigned char *) calloc(hrpdp->dplen, 1);

        assert(bech32_decode(hrpdp, bstr) == E_BECH32_LENGTH_TOO_SHORT);

        free(hrpdp->dp);
        free(hrpdp->hrp);
        free(hrpdp);
    }

    { // allocated dp is too short
        char bstr[] = "xyz1pzrs3usye";

        bech32_HrpAndDp *hrpdp = malloc(sizeof(bech32_HrpAndDp));
        hrpdp->hrplen = 10;
        hrpdp->hrp = (char *) calloc(hrpdp->hrplen, 1);
        hrpdp->dplen = 1;
        hrpdp->dp = (unsigned char *) calloc(hrpdp->dplen, 1);

        assert(bech32_decode(hrpdp, bstr) == E_BECH32_LENGTH_TOO_SHORT);

        free(hrpdp->dp);
        free(hrpdp->hrp);
        free(hrpdp);
    }
}

void decode_minimalExample_isSuccessful() {
    char bstr[] = "a1lqfn3a";
    char expectedHrp[] = "a";

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr);

    assert(bech32_decode(hrpdp, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(ENCODING_BECH32M == hrpdp->encoding);

    bech32_free_HrpAndDp(hrpdp);
}

void decode_longExample_isSuccessful() {
    char bstr[] = "abcdef1l7aum6echk45nj3s0wdvt2fg8x9yrzpqzd3ryx";
    char expectedHrp[] = "abcdef";

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr);

    assert(bech32_decode(hrpdp, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(hrpdp->dp[0] == '\x1f');    // first 'l' in above dp part
    assert(hrpdp->dp[31] == '\0'); // last 'q' in above dp part
    assert(ENCODING_BECH32M == hrpdp->encoding);

    bech32_free_HrpAndDp(hrpdp);
}

void decode_minimalExampleBadChecksum_isUnsuccessful() {
    char bstr[] = "a1lqfn3q"; // last 'q' should be a 'a'

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr);

    assert(bech32_decode(hrpdp, bstr) == E_BECH32_INVALID_CHECKSUM);

    bech32_free_HrpAndDp(hrpdp);
}

void decode_whenCppMethodThrowsException_isUnsuccessful() {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to decode a string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char bstr[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr);

    assert(bech32_decode(hrpdp, bstr) == E_BECH32_UNKNOWN_ERROR);

    bech32_free_HrpAndDp(hrpdp);
}

void encode_withBadArgs_isUnsuccessful() {

    { // output is null
        char hrp[] = "a";
        unsigned char dp[] = {};
        bech32_bstring * bstr = NULL;

        assert(bech32_encode(bstr, hrp, dp, sizeof(dp)) == E_BECH32_NULL_ARGUMENT);
    }

    { // hrp is null
        char *hrp = NULL;
        unsigned char dp[] = {};
        bech32_bstring bstr;

        assert(bech32_encode(&bstr, hrp, dp, sizeof(dp)) == E_BECH32_NULL_ARGUMENT);
    }

    { // dp is null
        char hrp[] = {};
        unsigned char *dp = NULL;
        bech32_bstring bstr;

        assert(bech32_encode(&bstr, hrp, dp, sizeof(dp)) == E_BECH32_NULL_ARGUMENT);
    }

    { // allocated bstr is too small
        char hrp[] = "xyz";
        unsigned char dp[] = {1,2,3};
        bech32_bstring bstr;
        bstr.length = 12; // length should (strlen(hrp) + 1 + sizeof(dp) + 6) = 13, but here we will mistakenly set it to 12
        bstr.bstr = (char *)calloc(bstr.length + 1, 1); //bstr size should be = bstr.length + 1 for '\0'
        // Could also use bech32_create_bstring() to avoid this problem.

        assert(bech32_encode(&bstr, hrp, dp, sizeof(dp)) == E_BECH32_LENGTH_TOO_SHORT);
        free(bstr.bstr);
    }

}

void encode_emptyExample_isUnsuccessful() {
    char hrp[] = "";
    unsigned char dp[] = {};
    bech32_bstring *bstr = bech32_create_bstring(strlen(hrp), sizeof(dp));

    assert(bech32_encode(bstr, hrp, dp, sizeof(dp)) == E_BECH32_NULL_ARGUMENT);

    bech32_free_bstring(bstr);
}

void encode_minimalExample_isSuccessful() {
    char hrp[] = "a";
    unsigned char dp[] = {};
    char expected[] = "a1lqfn3a";
    bech32_bstring *bstr = bech32_create_bstring(strlen(hrp), sizeof(dp));

    assert(bech32_encode(bstr, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstr->bstr) == 0);

    bech32_free_bstring(bstr);
}

void encode_smallExample_isSuccessful() {
    char hrp[] = "xyz";
    unsigned char dp[] = {1,2,3};
    char expected[] = "xyz1pzrs3usye";
    bech32_bstring *bstr = bech32_create_bstring(strlen(hrp), sizeof(dp));

    assert(bech32_encode(bstr, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstr->bstr) == 0);

    bech32_free_bstring(bstr);
}

void encode_whenCppMethodThrowsException_isUnsuccessful() {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to encode an HRP string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char hrp[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    unsigned char dp[] = {1,2,3};
    bech32_bstring *bstr = bech32_create_bstring(strlen(hrp), sizeof(dp));

    assert(bech32_encode(bstr, hrp, dp, sizeof(dp)) == E_BECH32_UNKNOWN_ERROR);

    bech32_free_bstring(bstr);
}

void decode_and_encode_minimalExample_producesSameResult() {
    char bstr1[] = "a1lqfn3a";
    char expectedHrp[] = "a";
    const size_t expectedDpSize = 0; // 0 = num chars after '1', minus 6 for checksum chars

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr1);

    assert(bech32_decode(hrpdp, bstr1) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(hrpdp->dplen == expectedDpSize);
    assert(ENCODING_BECH32M == hrpdp->encoding);

    bech32_bstring *bstr2 = bech32_create_bstring(hrpdp->hrplen, hrpdp->dplen);

    assert(bech32_encode(bstr2, hrpdp->hrp, hrpdp->dp, hrpdp->dplen) == E_BECH32_SUCCESS);
    assert(strcmp(bstr1, bstr2->bstr) == 0);

    bech32_free_HrpAndDp(hrpdp);
    bech32_free_bstring(bstr2);
}

void decode_and_encode_smallExample_producesSameResult() {
    char bstr1[] = "xyz1pzrs3usye";
    char expectedHrp[] = "xyz";
    const size_t expectedDpSize = 3; // 3 = num chars after '1', minus 6 for checksum chars

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr1);

    assert(bech32_decode(hrpdp, bstr1) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(hrpdp->dplen == expectedDpSize);
    assert(ENCODING_BECH32M == hrpdp->encoding);

    bech32_bstring *bstr2 = bech32_create_bstring(hrpdp->hrplen, hrpdp->dplen);

    assert(bech32_encode(bstr2, hrpdp->hrp, hrpdp->dp, hrpdp->dplen) == E_BECH32_SUCCESS);
    assert(strcmp(bstr1, bstr2->bstr) == 0);

    bech32_free_HrpAndDp(hrpdp);
    bech32_free_bstring(bstr2);
}

void decode_and_encode_longExample_producesSameResult() {
    char bstr1[] = "abcdef1l7aum6echk45nj3s0wdvt2fg8x9yrzpqzd3ryx";
    char expectedHrp[] = "abcdef";
    const size_t expectedDpSize = 32; // 32 = num chars after '1', minus 6 for checksum chars

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr1);

    assert(bech32_decode(hrpdp, bstr1) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(hrpdp->dplen == expectedDpSize);
    assert(ENCODING_BECH32M == hrpdp->encoding);

    bech32_bstring *bstr2 = bech32_create_bstring(hrpdp->hrplen, hrpdp->dplen);

    assert(bech32_encode(bstr2, hrpdp->hrp, hrpdp->dp, expectedDpSize) == E_BECH32_SUCCESS);
    assert(strcmp(bstr1, bstr2->bstr) == 0);

    bech32_free_HrpAndDp(hrpdp);
    bech32_free_bstring(bstr2);
}

// ---------- tests using original checksum constant = 1 ------------

void decode_c1_minimalExample_isSuccessful() {
    char bstr[] = "a12uel5l";
    char expectedHrp[] = "a";

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr);

    assert(bech32_decode(hrpdp, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(ENCODING_BECH32 == hrpdp->encoding);

    bech32_free_HrpAndDp(hrpdp);
}

void decode_c1_longExample_isSuccessful() {
    char bstr[] = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    char expectedHrp[] = "abcdef";

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr);

    assert(bech32_decode(hrpdp, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(hrpdp->dp[0] == '\0');    // first 'q' in above dp part
    assert(hrpdp->dp[31] == '\x1f'); // last 'l' in above dp part
    assert(ENCODING_BECH32 == hrpdp->encoding);

    bech32_free_HrpAndDp(hrpdp);
}

void encode_c1_minimalExample_isSuccessful() {
    char hrp[] = "a";
    unsigned char dp[] = {};
    char expected[] = "a12uel5l";
    bech32_bstring *bstr = bech32_create_bstring(strlen(hrp), sizeof(dp));

    assert(bech32_encode_1(bstr, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstr->bstr) == 0);

    bech32_free_bstring(bstr);
}

void encode_c1_smallExample_isSuccessful() {
    char hrp[] = "xyz";
    unsigned char dp[] = {1,2,3};
    char expected[] = "xyz1pzr9dvupm";
    bech32_bstring *bstr = bech32_create_bstring(strlen(hrp), sizeof(dp));

    assert(bech32_encode_1(bstr, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstr->bstr) == 0);

    bech32_free_bstring(bstr);
}

void decode_and_encode_c1_minimalExample_producesSameResult() {
    char bstr1[] = "a12uel5l";
    char expectedHrp[] = "a";
    const size_t expectedDpSize = 0; // 0 = num chars after '1', minus 6 for checksum chars

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr1);

    assert(bech32_decode(hrpdp, bstr1) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(hrpdp->dplen == expectedDpSize);
    assert(ENCODING_BECH32 == hrpdp->encoding);

    bech32_bstring *bstr2 = bech32_create_bstring(hrpdp->hrplen, hrpdp->dplen);

    assert(bech32_encode_1(bstr2, hrpdp->hrp, hrpdp->dp, hrpdp->dplen) == E_BECH32_SUCCESS);
    assert(strcmp(bstr1, bstr2->bstr) == 0);

    bech32_free_HrpAndDp(hrpdp);
    bech32_free_bstring(bstr2);
}

void decode_and_encode_c1_smallExample_producesSameResult() {
    char bstr1[] = "xyz1pzr9dvupm";
    char expectedHrp[] = "xyz";
    const size_t expectedDpSize = 3; // 3 = num chars after '1', minus 6 for checksum chars

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr1);

    assert(bech32_decode(hrpdp, bstr1) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(hrpdp->dplen == expectedDpSize);
    assert(ENCODING_BECH32 == hrpdp->encoding);

    bech32_bstring *bstr2 = bech32_create_bstring(hrpdp->hrplen, hrpdp->dplen);

    assert(bech32_encode_1(bstr2, hrpdp->hrp, hrpdp->dp, hrpdp->dplen) == E_BECH32_SUCCESS);
    assert(strcmp(bstr1, bstr2->bstr) == 0);

    bech32_free_HrpAndDp(hrpdp);
    bech32_free_bstring(bstr2);
}

void decode_and_encode_c1_longExample_producesSameResult() {
    char bstr1[] = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    char expectedHrp[] = "abcdef";
    const size_t expectedDpSize = 32; // 32 = num chars after '1', minus 6 for checksum chars

    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstr1);

    assert(bech32_decode(hrpdp, bstr1) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(hrpdp->dplen == expectedDpSize);
    assert(ENCODING_BECH32 == hrpdp->encoding);

    bech32_bstring *bstr2 = bech32_create_bstring(hrpdp->hrplen, hrpdp->dplen);

    assert(bech32_encode_1(bstr2, hrpdp->hrp, hrpdp->dp, hrpdp->dplen) == E_BECH32_SUCCESS);
    assert(strcmp(bstr1, bstr2->bstr) == 0);

    bech32_free_HrpAndDp(hrpdp);
    bech32_free_bstring(bstr2);
}


void test_strerror() {
    strerror_withValidErrorCode_returnsErrorMessage();
    strerror_withInvalidErrorCode_returnsUnknownErrorMessage();
}

void test_stripUnknownChars() {
    stripUnknownChars_withNullInput_returnsError();
    stripUnknownChars_withNullOutput_returnsError();
    stripUnknownChars_withInsufficientOutputStorage_returnsError();
    stripUnknownChars_withSimpleString_returnsSameString();
    stripUnknownChars_withComplexString_returnsStrippedString();
    stripUnknownChars_withFunkyString_returnsStrippedString();
}

void create_HrpAndDp_storage_withNullInput_returnsNull() {
    bech32_HrpAndDp *p = bech32_create_HrpAndDp(NULL);
    assert(p == NULL);
}

void create_HrpAndDp_storage_withZeroLengthInput_returnsNull() {
    char empty[] = "";
    bech32_HrpAndDp *p = bech32_create_HrpAndDp(empty);
    assert(p == NULL);
}

void create_HrpAndDp_storage_withMalformedInput_returnsNull() {
    char tooShort[] = "a1a";
    bech32_HrpAndDp *p = bech32_create_HrpAndDp(tooShort);
    assert(p == NULL);

    char noSeparator[] = "aaaaaaaa";
    p = bech32_create_HrpAndDp(noSeparator);
    assert(p == NULL);

    char checksumTooShort[] = "aa1qqqqq";
    p = bech32_create_HrpAndDp(checksumTooShort);
    assert(p == NULL);
}

void create_HrpAndDp_storage_minimalExample_isSuccessful() {
    char bstr[] = "a12uel5l";
    bech32_HrpAndDp *p = bech32_create_HrpAndDp(bstr);
    assert(p != NULL);
    bech32_free_HrpAndDp(p);
}

void create_HrpAndDp_storage_smallExample_isSuccessful() {
    char bstr[] = "xyz1pzr9dvupm";
    bech32_HrpAndDp *p = bech32_create_HrpAndDp(bstr);
    assert(p != NULL);
    bech32_free_HrpAndDp(p);
}

void create_encoded_string_storage_withMalformedInput_returnsNull() {
    // hrp too short
    bech32_bstring *p = bech32_create_bstring(0, 3);
    assert(p == NULL);
}

void create_encoded_string_storage_minimalExample_isSuccessful() {
    bech32_bstring *p = bech32_create_bstring(1, 0);
    assert(p != NULL);
    bech32_free_bstring(p);
}

void create_encoded_string_storage_smallExample_isSuccessful() {
    bech32_bstring *p = bech32_create_bstring(3, 3);
    assert(p != NULL);
    bech32_free_bstring(p);
}

void create_encoded_string_storage_from_HrpAndDp_withNullInput_returnsNull() {
    bech32_bstring *p = bech32_create_bstring_from_HrpAndDp(NULL);
    assert(p == NULL);
}

void create_encoded_string_storage_from_HrpAndDp_withMalformedInput_returnsNull() {
    // create a valid bech32_HrpAndDp, then corrupt it for testing
    char bstr[] = "xyz1pzr9dvupm";
    bech32_HrpAndDp *hd = bech32_create_HrpAndDp(bstr);

    // hrp too short
    hd->hrplen = 0;
    bech32_bstring *p = bech32_create_bstring_from_HrpAndDp(hd);
    assert(p == NULL);

    bech32_free_HrpAndDp(hd);
}

void create_encoded_string_storage_from_HrpAndDp_minimalExample_isSuccessful() {
    char bstr[] = "a12uel5l";
    bech32_HrpAndDp *hd = bech32_create_HrpAndDp(bstr);
    assert(hd != NULL);

    bech32_bstring *p = bech32_create_bstring_from_HrpAndDp(hd);
    assert(p != NULL);

    bech32_free_HrpAndDp(hd);
    bech32_free_bstring(p);
}

void create_encoded_string_storage_from_HrpAndDp_smallExample_isSuccessful() {
    char bstr[] = "xyz1pzr9dvupm";
    bech32_HrpAndDp *hd = bech32_create_HrpAndDp(bstr);
    assert(hd != NULL);

    bech32_bstring *p = bech32_create_bstring_from_HrpAndDp(hd);
    assert(p != NULL);

    bech32_free_HrpAndDp(hd);
    bech32_free_bstring(p);
}

void test_memoryAllocation() {
    create_HrpAndDp_storage_withNullInput_returnsNull();
    create_HrpAndDp_storage_withZeroLengthInput_returnsNull();
    create_HrpAndDp_storage_withMalformedInput_returnsNull();
    create_HrpAndDp_storage_minimalExample_isSuccessful();
    create_HrpAndDp_storage_smallExample_isSuccessful();

    create_encoded_string_storage_withMalformedInput_returnsNull();
    create_encoded_string_storage_minimalExample_isSuccessful();
    create_encoded_string_storage_smallExample_isSuccessful();

    create_encoded_string_storage_from_HrpAndDp_withNullInput_returnsNull();
    create_encoded_string_storage_from_HrpAndDp_withMalformedInput_returnsNull();
    create_encoded_string_storage_from_HrpAndDp_minimalExample_isSuccessful();
    create_encoded_string_storage_from_HrpAndDp_smallExample_isSuccessful();
}

void tests_using_default_checksum_constant() {
    decode_withBadArgs_isUnsuccessful();
    decode_whenCppMethodThrowsException_isUnsuccessful();
    decode_minimalExample_isSuccessful();
    decode_longExample_isSuccessful();
    decode_minimalExampleBadChecksum_isUnsuccessful();

    encode_withBadArgs_isUnsuccessful();
    encode_whenCppMethodThrowsException_isUnsuccessful();
    encode_emptyExample_isUnsuccessful();
    encode_minimalExample_isSuccessful();
    encode_smallExample_isSuccessful();

    decode_and_encode_minimalExample_producesSameResult();
    decode_and_encode_smallExample_producesSameResult();
    decode_and_encode_longExample_producesSameResult();
}

void tests_using_original_checksum_constant() {
    decode_c1_minimalExample_isSuccessful();
    decode_c1_longExample_isSuccessful();

    encode_c1_minimalExample_isSuccessful();
    encode_c1_smallExample_isSuccessful();

    decode_and_encode_c1_minimalExample_producesSameResult();
    decode_and_encode_c1_smallExample_producesSameResult();
    decode_and_encode_c1_longExample_producesSameResult();
}

int main() {

    test_strerror();
    test_stripUnknownChars();
    test_memoryAllocation();

    tests_using_default_checksum_constant();
    tests_using_original_checksum_constant();

    return 0;
}


