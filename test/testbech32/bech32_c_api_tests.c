// test program calling bech32 library from C

#include "bech32.h"
#include <string.h>
#include <stdlib.h>

// make sure we can run these tests even when building a release version
#undef NDEBUG
#include <assert.h>


void strerror_withValidErrorCode_returnsErrorMessage(void) {
    const char *message = bech32_strerror(E_BECH32_SUCCESS);
    assert(message != NULL);
    assert(strcmp(message, "Success") == 0);
}

void strerror_withInvalidErrorCode_returnsUnknownErrorMessage(void) {
    const char *message = bech32_strerror(1234);
    assert(message != NULL);
    assert(strcmp(message, "Unknown error") == 0);
}

void stripUnknownChars_withNullInput_returnsError(void) {
    char *src = NULL;
    char dst[] = "";

    assert(bech32_stripUnknownChars(dst, sizeof(dst), src, 0) == E_BECH32_NULL_ARGUMENT);
}

void stripUnknownChars_withNullOutput_returnsError(void) {
    char src[] = "foo";
    char *dst = NULL;

    // sizeof(dst) is meaningless here as we are testing dst == NULL
    assert(bech32_stripUnknownChars(dst, 0, src, sizeof(src)) == E_BECH32_NULL_ARGUMENT);
}

void stripUnknownChars_withInsufficientOutputStorage_returnsError(void) {
    char src[] = "ffff";
    char dst[sizeof(src)-1];

    assert(bech32_stripUnknownChars(dst, sizeof(dst), src, sizeof(src)) == E_BECH32_LENGTH_TOO_SHORT);
}

void stripUnknownChars_withSimpleString_returnsSameString(void) {
    char src[] = "ffff";
    char dst[sizeof(src)];

    bech32_stripUnknownChars(dst, sizeof(dst), src, sizeof(src));

    assert(strcmp(src, dst) == 0);
}

void stripUnknownChars_withComplexString_returnsStrippedString(void) {
    char src[] = "foobar";
    char expected[] = "far";
    char dst[sizeof(src)];

    bech32_stripUnknownChars(dst, sizeof(dst), src, sizeof(src));

    assert(strcmp(expected, dst) == 0);
}

void stripUnknownChars_withFunkyString_returnsStrippedString(void) {
    char src[] = "tx1!rjk0\\u5ng*4jsf^^mc";
    char expected[] = "tx1rjk0u5ng4jsfmc";
    char dst[sizeof(src)];

    bech32_stripUnknownChars(dst, sizeof(dst), src, sizeof(src));

    assert(strcmp(expected, dst) == 0);
}

// ----- tests using default checksum constant = M (0x2bc830a3) ------

void decode_withBadArgs_isUnsuccessful(void) {

    { // input is null
        char *bstr = NULL;

        bech32_DecodedResult *decodedResult = malloc(sizeof(bech32_DecodedResult));
        decodedResult->hrplen = 10;
        decodedResult->hrp = (char *) calloc(decodedResult->hrplen, 1);
        decodedResult->dplen = 10;
        decodedResult->dp = (unsigned char *) calloc(decodedResult->dplen, 1);

        assert(bech32_decode(decodedResult, bstr) == E_BECH32_NULL_ARGUMENT);

        free(decodedResult->dp);
        free(decodedResult->hrp);
        free(decodedResult);
    }

    { // output is null
        char bstr[] = "xyz1pzrs3usye";

        bech32_DecodedResult *decodedResult = NULL;

        assert(bech32_decode(decodedResult, bstr) == E_BECH32_NULL_ARGUMENT);
    }

    { // hrp is null
        char bstr[] = "xyz1pzrs3usye";

        bech32_DecodedResult *decodedResult = malloc(sizeof(bech32_DecodedResult));
        decodedResult->dplen = 10;
        decodedResult->dp = (unsigned char *) calloc(decodedResult->dplen, 1);
        decodedResult->hrp = NULL;

        assert(bech32_decode(decodedResult, bstr) == E_BECH32_NULL_ARGUMENT);

        free(decodedResult->dp);
        free(decodedResult);
    }

    { // dp is null
        char bstr[] = "xyz1pzrs3usye";

        bech32_DecodedResult *decodedResult = malloc(sizeof(bech32_DecodedResult));
        decodedResult->hrplen = 1;
        decodedResult->hrp = (char *) calloc(decodedResult->hrplen, 1);
        decodedResult->dp = NULL;

        assert(bech32_decode(decodedResult, bstr) == E_BECH32_NULL_ARGUMENT);

        free(decodedResult->hrp);
        free(decodedResult);
    }

    { // allocated hrp is too short
        char bstr[] = "xyz1pzrs3usye";

        bech32_DecodedResult *decodedResult = malloc(sizeof(bech32_DecodedResult));
        decodedResult->hrplen = 1;
        decodedResult->hrp = (char *) calloc(decodedResult->hrplen, 1);
        decodedResult->dplen = 10;
        decodedResult->dp = (unsigned char *) calloc(decodedResult->dplen, 1);

        assert(bech32_decode(decodedResult, bstr) == E_BECH32_LENGTH_TOO_SHORT);

        free(decodedResult->dp);
        free(decodedResult->hrp);
        free(decodedResult);
    }

    { // allocated dp is too short
        char bstr[] = "xyz1pzrs3usye";

        bech32_DecodedResult *decodedResult = malloc(sizeof(bech32_DecodedResult));
        decodedResult->hrplen = 10;
        decodedResult->hrp = (char *) calloc(decodedResult->hrplen, 1);
        decodedResult->dplen = 1;
        decodedResult->dp = (unsigned char *) calloc(decodedResult->dplen, 1);

        assert(bech32_decode(decodedResult, bstr) == E_BECH32_LENGTH_TOO_SHORT);

        free(decodedResult->dp);
        free(decodedResult->hrp);
        free(decodedResult);
    }
}

void decode_minimalExample_isSuccessful(void) {
    char bstr[] = "a1lqfn3a";
    char expectedHrp[] = "a";

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, expectedHrp) == 0);
    assert(ENCODING_BECH32M == decodedResult->encoding);

    bech32_free_DecodedResult(decodedResult);
}

void decode_longExample_isSuccessful(void) {
    char bstr[] = "abcdef1l7aum6echk45nj3s0wdvt2fg8x9yrzpqzd3ryx";
    char expectedHrp[] = "abcdef";

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, expectedHrp) == 0);
    assert(decodedResult->dp[0] == '\x1f');    // first 'l' in above dp part
    assert(decodedResult->dp[31] == '\0'); // last 'q' in above dp part
    assert(ENCODING_BECH32M == decodedResult->encoding);

    bech32_free_DecodedResult(decodedResult);
}

void decode_minimalExampleBadChecksum_isUnsuccessful(void) {
    char bstr[] = "a1lqfn3q"; // last 'q' should be a 'a'

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_INVALID_CHECKSUM);

    bech32_free_DecodedResult(decodedResult);
}

void decode_whenCppMethodThrowsException_isUnsuccessful(void) {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to decode a string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char bstr[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_UNKNOWN_ERROR);

    bech32_free_DecodedResult(decodedResult);
}

void encode_withBadArgs_isUnsuccessful(void) {

    { // bstring is null
        char hrp[] = "a";
        unsigned char dp[] = {0};
        bech32_bstring * bstring = NULL;

        assert(bech32_encode(bstring, hrp, dp, sizeof(dp)) == E_BECH32_NULL_ARGUMENT);
    }

    { // hrp is null
        char *hrp = NULL;
        unsigned char dp[] = {0};
        bech32_bstring bstring;

        assert(bech32_encode(&bstring, hrp, dp, sizeof(dp)) == E_BECH32_NULL_ARGUMENT);
    }

    { // dp is null
        char hrp[] = {0};
        unsigned char *dp = NULL;
        bech32_bstring bstring;

        assert(bech32_encode(&bstring, hrp, dp, sizeof(dp)) == E_BECH32_NULL_ARGUMENT);
    }

    { // allocated string is too small
        char hrp[] = "xyz";
        unsigned char dp[] = {1,2,3};
        bech32_bstring bstring;
        bstring.length = 12; // length should (strlen(hrp) + 1 + sizeof(dp) + 6) = 13, but here we will mistakenly set it to 12
        bstring.string = (char *)calloc(bstring.length + 1, 1); //string size should be = string.length + 1 for '\0'
        // Should use bech32_create_bstring(void) to avoid this problem.

        assert(bech32_encode(&bstring, hrp, dp, sizeof(dp)) == E_BECH32_LENGTH_TOO_SHORT);
        free(bstring.string);
    }

}

void encode_emptyExample_isUnsuccessful(void) {
    char hrp[] = "";
    unsigned char dp[] = {0}; // C doesn't allow zero-length arrays
    bech32_bstring *bstring = bech32_create_bstring(strlen(hrp), 0);

    assert(bech32_encode(bstring, hrp, dp, 0) == E_BECH32_NULL_ARGUMENT);

    bech32_free_bstring(bstring);
}

void encode_minimalExample_isSuccessful(void) {
    char hrp[] = "a";
    unsigned char dp[] = {0}; // C doesn't allow zero-length arrays
    char expected[] = "a1lqfn3a";
    bech32_bstring *bstring = bech32_create_bstring(strlen(hrp), 0);

    assert(bech32_encode(bstring, hrp, dp, 0) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstring->string) == 0);

    bech32_free_bstring(bstring);
}

void encode_smallExample_isSuccessful(void) {
    char hrp[] = "xyz";
    unsigned char dp[] = {1,2,3};
    char expected[] = "xyz1pzrs3usye";
    bech32_bstring *bstring = bech32_create_bstring(strlen(hrp), sizeof(dp));

    assert(bech32_encode(bstring, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstring->string) == 0);

    bech32_free_bstring(bstring);
}

void encode_whenCppMethodThrowsException_isUnsuccessful(void) {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to encode an HRP string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char hrp[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    unsigned char dp[] = {1,2,3};
    bech32_bstring *bstring = bech32_create_bstring(strlen(hrp), sizeof(dp));

    assert(bech32_encode(bstring, hrp, dp, sizeof(dp)) == E_BECH32_UNKNOWN_ERROR);

    bech32_free_bstring(bstring);
}

void decode_and_encode_minimalExample_producesSameResult(void) {
    char bstr[] = "a1lqfn3a";
    char expectedHrp[] = "a";
    const size_t expectedDpSize = 0; // 0 = num chars after '1', minus 6 for checksum chars

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, expectedHrp) == 0);
    assert(decodedResult->dplen == expectedDpSize);
    assert(ENCODING_BECH32M == decodedResult->encoding);

    bech32_bstring *bstring = bech32_create_bstring(decodedResult->hrplen, decodedResult->dplen);

    assert(bech32_encode(bstring, decodedResult->hrp, decodedResult->dp, decodedResult->dplen) == E_BECH32_SUCCESS);
    assert(strcmp(bstr, bstring->string) == 0);

    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

void decode_and_encode_smallExample_producesSameResult(void) {
    char bstr[] = "xyz1pzrs3usye";
    char expectedHrp[] = "xyz";
    const size_t expectedDpSize = 3; // 3 = num chars after '1', minus 6 for checksum chars

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, expectedHrp) == 0);
    assert(decodedResult->dplen == expectedDpSize);
    assert(ENCODING_BECH32M == decodedResult->encoding);

    bech32_bstring *bstring = bech32_create_bstring(decodedResult->hrplen, decodedResult->dplen);

    assert(bech32_encode(bstring, decodedResult->hrp, decodedResult->dp, decodedResult->dplen) == E_BECH32_SUCCESS);
    assert(strcmp(bstr, bstring->string) == 0);

    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

void decode_and_encode_longExample_producesSameResult(void) {
    char bstr[] = "abcdef1l7aum6echk45nj3s0wdvt2fg8x9yrzpqzd3ryx";
    char expectedHrp[] = "abcdef";
    const size_t expectedDpSize = 32; // 32 = num chars after '1', minus 6 for checksum chars

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, expectedHrp) == 0);
    assert(decodedResult->dplen == expectedDpSize);
    assert(ENCODING_BECH32M == decodedResult->encoding);

    bech32_bstring *bstring = bech32_create_bstring(decodedResult->hrplen, decodedResult->dplen);

    assert(bech32_encode(bstring, decodedResult->hrp, decodedResult->dp, expectedDpSize) == E_BECH32_SUCCESS);
    assert(strcmp(bstr, bstring->string) == 0);

    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

// ---------- tests using original checksum constant = 1 ------------

void decode_c1_minimalExample_isSuccessful(void) {
    char bstr[] = "a12uel5l";
    char expectedHrp[] = "a";

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, expectedHrp) == 0);
    assert(ENCODING_BECH32 == decodedResult->encoding);

    bech32_free_DecodedResult(decodedResult);
}

void decode_c1_longExample_isSuccessful(void) {
    char bstr[] = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    char expectedHrp[] = "abcdef";

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, expectedHrp) == 0);
    assert(decodedResult->dp[0] == '\0');    // first 'q' in above dp part
    assert(decodedResult->dp[31] == '\x1f'); // last 'l' in above dp part
    assert(ENCODING_BECH32 == decodedResult->encoding);

    bech32_free_DecodedResult(decodedResult);
}

void encode_c1_minimalExample_isSuccessful(void) {
    char hrp[] = "a";
    unsigned char dp[] = {0}; // C doesn't allow zero-length arrays
    char expected[] = "a12uel5l";
    bech32_bstring *bstring = bech32_create_bstring(strlen(hrp), 0);

    assert(bech32_encode_using_original_constant(bstring, hrp, dp, 0) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstring->string) == 0);

    bech32_free_bstring(bstring);
}

void encode_c1_smallExample_isSuccessful(void) {
    char hrp[] = "xyz";
    unsigned char dp[] = {1,2,3};
    char expected[] = "xyz1pzr9dvupm";
    bech32_bstring *bstring = bech32_create_bstring(strlen(hrp), sizeof(dp));

    assert(bech32_encode_using_original_constant(bstring, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);
    assert(strcmp(expected, bstring->string) == 0);

    bech32_free_bstring(bstring);
}

void decode_and_encode_c1_minimalExample_producesSameResult(void) {
    char bstr[] = "a12uel5l";
    char expectedHrp[] = "a";
    const size_t expectedDpSize = 0; // 0 = num chars after '1', minus 6 for checksum chars

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, expectedHrp) == 0);
    assert(decodedResult->dplen == expectedDpSize);
    assert(ENCODING_BECH32 == decodedResult->encoding);

    bech32_bstring *bstring = bech32_create_bstring(decodedResult->hrplen, decodedResult->dplen);

    assert(bech32_encode_using_original_constant(bstring, decodedResult->hrp, decodedResult->dp, decodedResult->dplen) == E_BECH32_SUCCESS);
    assert(strcmp(bstr, bstring->string) == 0);

    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

void decode_and_encode_c1_smallExample_producesSameResult(void) {
    char bstr[] = "xyz1pzr9dvupm";
    char expectedHrp[] = "xyz";
    const size_t expectedDpSize = 3; // 3 = num chars after '1', minus 6 for checksum chars

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, expectedHrp) == 0);
    assert(decodedResult->dplen == expectedDpSize);
    assert(ENCODING_BECH32 == decodedResult->encoding);

    bech32_bstring *bstring = bech32_create_bstring(decodedResult->hrplen, decodedResult->dplen);

    assert(bech32_encode_using_original_constant(bstring, decodedResult->hrp, decodedResult->dp, decodedResult->dplen) == E_BECH32_SUCCESS);
    assert(strcmp(bstr, bstring->string) == 0);

    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

void decode_and_encode_c1_longExample_producesSameResult(void) {
    char bstr[] = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    char expectedHrp[] = "abcdef";
    const size_t expectedDpSize = 32; // 32 = num chars after '1', minus 6 for checksum chars

    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);

    assert(bech32_decode(decodedResult, bstr) == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, expectedHrp) == 0);
    assert(decodedResult->dplen == expectedDpSize);
    assert(ENCODING_BECH32 == decodedResult->encoding);

    bech32_bstring *bstring = bech32_create_bstring(decodedResult->hrplen, decodedResult->dplen);

    assert(bech32_encode_using_original_constant(bstring, decodedResult->hrp, decodedResult->dp, decodedResult->dplen) == E_BECH32_SUCCESS);
    assert(strcmp(bstr, bstring->string) == 0);

    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}


void test_strerror(void) {
    strerror_withValidErrorCode_returnsErrorMessage();
    strerror_withInvalidErrorCode_returnsUnknownErrorMessage();
}

void test_stripUnknownChars(void) {
    stripUnknownChars_withNullInput_returnsError();
    stripUnknownChars_withNullOutput_returnsError();
    stripUnknownChars_withInsufficientOutputStorage_returnsError();
    stripUnknownChars_withSimpleString_returnsSameString();
    stripUnknownChars_withComplexString_returnsStrippedString();
    stripUnknownChars_withFunkyString_returnsStrippedString();
}

void create_DecodedResult_storage_withNullInput_returnsNull(void) {
    bech32_DecodedResult *p = bech32_create_DecodedResult(NULL);
    assert(p == NULL);
}

void create_DecodedResult_storage_withZeroLengthInput_returnsNull(void) {
    char empty[] = "";
    bech32_DecodedResult *p = bech32_create_DecodedResult(empty);
    assert(p == NULL);
}

void create_DecodedResult_storage_withMalformedInput_returnsNull(void) {
    char tooShort[] = "a1a";
    bech32_DecodedResult *p = bech32_create_DecodedResult(tooShort);
    assert(p == NULL);

    char noSeparator[] = "aaaaaaaa";
    p = bech32_create_DecodedResult(noSeparator);
    assert(p == NULL);

    char checksumTooShort[] = "aa1qqqqq";
    p = bech32_create_DecodedResult(checksumTooShort);
    assert(p == NULL);
}

void create_DecodedResult_storage_minimalExample_isSuccessful(void) {
    char bstr[] = "a12uel5l";
    bech32_DecodedResult *p = bech32_create_DecodedResult(bstr);
    assert(p != NULL);
    bech32_free_DecodedResult(p);
}

void create_DecodedResult_storage_smallExample_isSuccessful(void) {
    char bstr[] = "xyz1pzr9dvupm";
    bech32_DecodedResult *p = bech32_create_DecodedResult(bstr);
    assert(p != NULL);
    bech32_free_DecodedResult(p);
}

void create_encoded_string_storage_withMalformedInput_returnsNull(void) {
    // hrp too short
    bech32_bstring *bstring = bech32_create_bstring(0, 3);
    assert(bstring == NULL);
}

void create_encoded_string_storage_minimalExample_isSuccessful(void) {
    bech32_bstring *bstring = bech32_create_bstring(1, 0);
    assert(bstring != NULL);
    bech32_free_bstring(bstring);
}

void create_encoded_string_storage_smallExample_isSuccessful(void) {
    bech32_bstring *bstring = bech32_create_bstring(3, 3);
    assert(bstring != NULL);
    bech32_free_bstring(bstring);
}

void create_encoded_string_storage_from_DecodedResult_withNullInput_returnsNull(void) {
    bech32_bstring *bstring = bech32_create_bstring_from_DecodedResult(NULL);
    assert(bstring == NULL);
}

void create_encoded_string_storage_from_DecodedResult_withMalformedInput_returnsNull(void) {
    // create a valid bech32_DecodedResult, then corrupt it for testing
    char bstr[] = "xyz1pzr9dvupm";
    bech32_DecodedResult *decodedResult = bech32_create_DecodedResult(bstr);

    // hrp too short
    decodedResult->hrplen = 0;
    bech32_bstring *bstring = bech32_create_bstring_from_DecodedResult(decodedResult);
    assert(bstring == NULL);

    bech32_free_DecodedResult(decodedResult);
}

void create_encoded_string_storage_from_DecodedResult_minimalExample_isSuccessful(void) {
    char bstr[] = "a12uel5l";
    bech32_DecodedResult *decodedResult = bech32_create_DecodedResult(bstr);
    assert(decodedResult != NULL);

    bech32_bstring *bstring = bech32_create_bstring_from_DecodedResult(decodedResult);
    assert(bstring != NULL);

    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

void create_encoded_string_storage_from_DecodedResult_smallExample_isSuccessful(void) {
    char bstr[] = "xyz1pzr9dvupm";
    bech32_DecodedResult *decodedResult = bech32_create_DecodedResult(bstr);
    assert(decodedResult != NULL);

    bech32_bstring *bstring = bech32_create_bstring_from_DecodedResult(decodedResult);
    assert(bstring != NULL);

    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

void test_memoryAllocation(void) {
    create_DecodedResult_storage_withNullInput_returnsNull();
    create_DecodedResult_storage_withZeroLengthInput_returnsNull();
    create_DecodedResult_storage_withMalformedInput_returnsNull();
    create_DecodedResult_storage_minimalExample_isSuccessful();
    create_DecodedResult_storage_smallExample_isSuccessful();

    create_encoded_string_storage_withMalformedInput_returnsNull();
    create_encoded_string_storage_minimalExample_isSuccessful();
    create_encoded_string_storage_smallExample_isSuccessful();

    create_encoded_string_storage_from_DecodedResult_withNullInput_returnsNull();
    create_encoded_string_storage_from_DecodedResult_withMalformedInput_returnsNull();
    create_encoded_string_storage_from_DecodedResult_minimalExample_isSuccessful();
    create_encoded_string_storage_from_DecodedResult_smallExample_isSuccessful();
}

void tests_using_default_checksum_constant(void) {
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

void tests_using_original_checksum_constant(void) {
    decode_c1_minimalExample_isSuccessful();
    decode_c1_longExample_isSuccessful();

    encode_c1_minimalExample_isSuccessful();
    encode_c1_smallExample_isSuccessful();

    decode_and_encode_c1_minimalExample_producesSameResult();
    decode_and_encode_c1_smallExample_producesSameResult();
    decode_and_encode_c1_longExample_producesSameResult();
}

int main(void) {

    test_strerror();
    test_stripUnknownChars();
    test_memoryAllocation();

    tests_using_default_checksum_constant();
    tests_using_original_checksum_constant();

    return 0;
}


