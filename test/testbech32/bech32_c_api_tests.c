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

void decode_withBadArgs_isUnsuccessful() {

    { // input is null
        char *bstr = NULL;

        bech32_HrpAndDp *hrpdp = malloc(sizeof(bech32_HrpAndDp));
        hrpdp->hrplen = 10;
        hrpdp->hrp = (char *) calloc(hrpdp->hrplen, 1);
        hrpdp->dplen = 10;
        hrpdp->dp = (unsigned char *) calloc(hrpdp->dplen, 1);

        assert(bech32_decode(hrpdp, bstr, 0) == E_BECH32_NULL_ARGUMENT);

        free(hrpdp->dp);
        free(hrpdp->hrp);
        free(hrpdp);
    }

    { // output is null
        char bstr[] = "xyz1pzr6jnr79";

        bech32_HrpAndDp *hrpdp = NULL;

        assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_NULL_ARGUMENT);
    }

    { // hrp is null
        char bstr[] = "xyz1pzr6jnr79";

        bech32_HrpAndDp *hrpdp = malloc(sizeof(bech32_HrpAndDp));
        hrpdp->dplen = 10;
        hrpdp->dp = (unsigned char *) calloc(hrpdp->dplen, 1);
        hrpdp->hrp = NULL;

        assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_NULL_ARGUMENT);

        free(hrpdp->dp);
        free(hrpdp);
    }

    { // dp is null
        char bstr[] = "xyz1pzr6jnr79";

        bech32_HrpAndDp *hrpdp = malloc(sizeof(bech32_HrpAndDp));
        hrpdp->hrplen = 1;
        hrpdp->hrp = (char *) calloc(hrpdp->hrplen, 1);
        hrpdp->dp = NULL;

        assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_NULL_ARGUMENT);

        free(hrpdp->hrp);
        free(hrpdp);
    }

    { // allocated hrp is too short
        char bstr[] = "xyz1pzr6jnr79";

        bech32_HrpAndDp *hrpdp = malloc(sizeof(bech32_HrpAndDp));
        hrpdp->hrplen = 1;
        hrpdp->hrp = (char *) calloc(hrpdp->hrplen, 1);
        hrpdp->dplen = 10;
        hrpdp->dp = (unsigned char *) calloc(hrpdp->dplen, 1);

        assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_LENGTH_TOO_SHORT);

        free(hrpdp->dp);
        free(hrpdp->hrp);
        free(hrpdp);
    }

    { // allocated dp is too short
        char bstr[] = "xyz1pzr6jnr79";

        bech32_HrpAndDp *hrpdp = malloc(sizeof(bech32_HrpAndDp));
        hrpdp->hrplen = 10;
        hrpdp->hrp = (char *) calloc(hrpdp->hrplen, 1);
        hrpdp->dplen = 1;
        hrpdp->dp = (unsigned char *) calloc(hrpdp->dplen, 1);

        assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_LENGTH_TOO_SHORT);

        free(hrpdp->dp);
        free(hrpdp->hrp);
        free(hrpdp);
    }
}

void decode_minimalExample_isSuccessful() {
    char bstr[] = "a14rxqtp";
    char expectedHrp[] = "a";

    bech32_HrpAndDp * hrpdp = create_HrpAndDp_storage(bstr);

    assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);

    free_HrpAndDp_storage(hrpdp);
}

void decode_longExample_isSuccessful() {
    char bstr[] = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lyllles";
    char expectedHrp[] = "abcdef";

    bech32_HrpAndDp * hrpdp = create_HrpAndDp_storage(bstr);

    assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);
    assert(hrpdp->dp[0] == '\0');    // 'q' in above dp part
    assert(hrpdp->dp[31] == '\x1f'); // 'l' in above dp part

    free_HrpAndDp_storage(hrpdp);
}

void decode_minimalExampleBadChecksum_isUnsuccessful() {
    char bstr[] = "a14rxqtq"; // last 'q' should be a 'p'
    char expectedHrp[] = "a";

    bech32_HrpAndDp * hrpdp = create_HrpAndDp_storage(bstr);

    assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_INVALID_CHECKSUM);

    free_HrpAndDp_storage(hrpdp);
}

void decode_whenMethodThrowsException_isUnsuccessful() {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to decode a string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char bstr[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    char expectedHrp[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    bech32_HrpAndDp * hrpdp = create_HrpAndDp_storage(bstr);

    assert(bech32_decode(hrpdp, bstr, sizeof(bstr)) == E_BECH32_UNKNOWN_ERROR);

    free_HrpAndDp_storage(hrpdp);
}

void encode_withBadArgs_isUnsuccessful() {

    { // output is null
        char hrp[] = "a";
        unsigned char dp[] = {};
        char * bstr = NULL;

        assert(bech32_encode(bstr, sizeof(bstr), hrp, sizeof(hrp), dp, sizeof(dp)) == E_BECH32_NULL_ARGUMENT);
    }

    { // dp is null
        char *hrp = NULL;
        unsigned char dp[] = {};
        char bstr[] = {};

        assert(bech32_encode(bstr, sizeof(bstr), hrp, sizeof(hrp), dp, sizeof(dp)) == E_BECH32_NULL_ARGUMENT);
    }

    { // hrp is null
        char hrp[] = {};
        unsigned char *dp = NULL;
        char bstr[] = {};

        assert(bech32_encode(bstr, sizeof(bstr), hrp, sizeof(hrp), dp, sizeof(dp)) == E_BECH32_NULL_ARGUMENT);
    }

    { // allocated bstr is too small
        char hrp[] = "xyz";
        unsigned char dp[] = {1,2,3};
        char bstr[sizeof(hrp) + 1 + sizeof(dp) + 5] = {0}; // should be sizeof(hrp) + 1 + sizeof(dp) + 6

        assert(bech32_encode(bstr, sizeof(bstr), hrp, sizeof(hrp), dp, sizeof(dp)) == E_BECH32_LENGTH_TOO_SHORT);
    }

}

void encode_emptyExample_isUnsuccessful() {
    char hrp[] = "";
    unsigned char dp[] = {};
    char expected[] = "a14rxqtp";
    char bstr[sizeof(hrp) + 1 + 6] = {0};

    assert(bech32_encode(bstr, sizeof(bstr), hrp, sizeof(hrp), dp, sizeof(dp)) == E_BECH32_UNKNOWN_ERROR);
}

void encode_minimalExample_isSuccessful() {
    char hrp[] = "a";
    unsigned char dp[] = {};
    char expected[] = "a14rxqtp";
    char bstr[sizeof(hrp) + 1 + 6] = {0};

    assert(bech32_encode(bstr, sizeof(bstr), hrp, sizeof(hrp), dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstr) == 0);
}

void encode_smallExample_isSuccessful() {
    char hrp[] = "xyz";
    unsigned char dp[] = {1,2,3};
    char expected[] = "xyz1pzr6jnr79";
    char bstr[sizeof(hrp) + 1 + sizeof(dp) + 6] = {0};

    assert(bech32_encode(bstr, sizeof(bstr), hrp, sizeof(hrp), dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstr) == 0);
}

void encode_whenMethodThrowsException_isUnsuccessful() {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to encode an HRP string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char hrp[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    unsigned char dp[] = {1,2,3};
    char bstr[sizeof(hrp) + 1 + sizeof(dp) + 6] = {0};

    assert(bech32_encode(bstr, sizeof(bstr), hrp, sizeof(hrp), dp, sizeof(dp)) == E_BECH32_UNKNOWN_ERROR);
}

void decode_and_encode_minimalExample_producesSameResult() {
    char bstr1[] = "a14rxqtp";
    char expectedHrp[] = "a";
    const size_t expectedDpSize = 0; // 0 = num chars after '1', minus 6 for checksum chars

    bech32_HrpAndDp * hrpdp = create_HrpAndDp_storage(bstr1);

    assert(bech32_decode(hrpdp, bstr1, sizeof(bstr1)) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);

    size_t bstr2len = sizeof(expectedHrp) + 1 + expectedDpSize + 6;
    char bstr2[bstr2len];
    memset(bstr2, 0, bstr2len);

    assert(bech32_encode(bstr2, sizeof(bstr2), hrpdp->hrp, sizeof(expectedHrp), hrpdp->dp, expectedDpSize) == E_BECH32_SUCCESS);
    assert(strcmp(bstr1, bstr2) == 0);

    free_HrpAndDp_storage(hrpdp);
}

void decode_and_encode_smallExample_producesSameResult() {
    char bstr1[] = "xyz1pzr6jnr79";
    char expectedHrp[] = "xyz";
    const size_t expectedDpSize = 3; // 3 = num chars after '1', minus 6 for checksum chars

    bech32_HrpAndDp * hrpdp = create_HrpAndDp_storage(bstr1);

    assert(bech32_decode(hrpdp, bstr1, sizeof(bstr1)) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);

    size_t bstr2len = sizeof(expectedHrp) + 1 + expectedDpSize + 6;
    char bstr2[bstr2len];
    memset(bstr2, 0, bstr2len);

    assert(bech32_encode(bstr2, sizeof(bstr2), hrpdp->hrp, sizeof(expectedHrp), hrpdp->dp, expectedDpSize) == E_BECH32_SUCCESS);
    assert(strcmp(bstr1, bstr2) == 0);

    free_HrpAndDp_storage(hrpdp);
}

void decode_and_encode_longExample_producesSameResult() {
    char bstr1[] = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lyllles";
    char expectedHrp[] = "abcdef";
    const size_t expectedDpSize = 32; // 32 = num chars after '1', minus 6 for checksum chars

    bech32_HrpAndDp * hrpdp = create_HrpAndDp_storage(bstr1);

    assert(bech32_decode(hrpdp, bstr1, sizeof(bstr1)) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, expectedHrp) == 0);

    size_t bstr2len = sizeof(expectedHrp) + 1 + expectedDpSize + 6;
    char bstr2[bstr2len];
    memset(bstr2, 0, bstr2len);

    assert(bech32_encode(bstr2, sizeof(bstr2), hrpdp->hrp, sizeof(expectedHrp), hrpdp->dp, expectedDpSize) == E_BECH32_SUCCESS);
    assert(strcmp(bstr1, bstr2) == 0);

    free_HrpAndDp_storage(hrpdp);
}

int main() {

    strerror_withValidErrorCode_returnsErrorMessage();
    strerror_withInvalidErrorCode_returnsUnknownErrorMessage();

    stripUnknownChars_withNullInput_returnsError();
    stripUnknownChars_withNullOutput_returnsError();
    stripUnknownChars_withInsufficientOutputStorage_returnsError();
    stripUnknownChars_withSimpleString_returnsSameString();
    stripUnknownChars_withComplexString_returnsStrippedString();
    stripUnknownChars_withFunkyString_returnsStrippedString();

    decode_withBadArgs_isUnsuccessful();
    decode_whenMethodThrowsException_isUnsuccessful();
    decode_minimalExample_isSuccessful();
    decode_longExample_isSuccessful();
    decode_minimalExampleBadChecksum_isUnsuccessful();

    encode_withBadArgs_isUnsuccessful();
    encode_whenMethodThrowsException_isUnsuccessful();
    encode_emptyExample_isUnsuccessful();
    encode_minimalExample_isSuccessful();
    encode_smallExample_isSuccessful();

    decode_and_encode_minimalExample_producesSameResult();
    decode_and_encode_smallExample_producesSameResult();
    decode_and_encode_longExample_producesSameResult();

    return 0;
}


