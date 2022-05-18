#include "bech32.h"
#include <string.h>
#include <stdio.h>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <assert.h>
#include <stdlib.h>

/**
 * Illustrates encoding a "human readable part" and a "data part" to a bech32 string, then
 * decoding that bech32 string and comparing the results.
 */
void encodeAndDecode() {

    // hrp and data to encode
    char hrp[] = "example";
    unsigned char dp[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    // create storage for bech32 string
    bech32_bstring *bstring = bech32_create_bstring(sizeof(hrp), sizeof(dp));
    if(!bstring) {
        printf("bech32 string can not be created");
        exit(E_BECH32_NO_MEMORY);
    }

    // expected bech32 string output
    char expected[] = "example1qpzry9x8ge8sqgv";

    // encode
    bech32_error err = bech32_encode(bstring, hrp, dp, sizeof(dp));
    if(err != E_BECH32_SUCCESS) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_bstring(bstring);
        exit(err);
    }

    if(strcmp(expected, bstring->string) != 0) {
        printf("Expected %s does not match encoded %s\n", expected, bstring->string);
        bech32_free_bstring(bstring);
        exit(E_BECH32_UNKNOWN_ERROR);
    }

    // create storage for decoded bech32 data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstring->string);
    if(!decodedResult) {
        printf("bech32 DecodedResult can not be created");
        bech32_free_bstring(bstring);
        exit(E_BECH32_NO_MEMORY);
    }

    // decode
    err = bech32_decode(decodedResult, bstring->string);
    if(err != E_BECH32_SUCCESS) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_DecodedResult(decodedResult);
        exit(err);
    }

    // check for expected values
    assert(strcmp(decodedResult->hrp, hrp) == 0);
    assert(decodedResult->dp[0] == dp[0]);
    assert(decodedResult->dp[8] == dp[8]);
    assert(ENCODING_BECH32M == decodedResult->encoding);

    // free memory
    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

/**
 * Illustrates cleaning a possibly "dirty" bech32 string, then decoding it into its "human
 * readable part" and "data part". Then encodes that data to another bech32 string and compares
 * the results.
 */
void decodeAndEncode() {

    // bech32 string with extra invalid characters
    char input_bstr[] = " example1:qpz!r--y9#x8&%&%ge-8-sqgv ";
    // expected_bstr bech32 string output
    char expected_bstr[] = "example1qpzry9x8ge8sqgv";

    // if the string you are trying to decode comes from an untrusted source, make sure
    // to strip invalid characters before allocating storage
    bech32_error err = bech32_stripUnknownChars(input_bstr, sizeof(input_bstr), input_bstr, sizeof(input_bstr));
    if(err != E_BECH32_SUCCESS) {
        printf("%s\n", bech32_strerror(err));
        exit(err);
    }

    // create storage for decoded bech32 data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(input_bstr);
    if(!decodedResult) {
        printf("bech32 DecodedResult can not be created");
        exit(E_BECH32_NO_MEMORY);
    }

    // decode
    err = bech32_decode(decodedResult, input_bstr);
    if(err != E_BECH32_SUCCESS) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_DecodedResult(decodedResult);
        exit(err);
    }

    // create storage for bech32 string
    bech32_bstring *bstring = bech32_create_bstring_from_DecodedResult(decodedResult);
    if(!bstring) {
        printf("bech32 string can not be created");
        bech32_free_DecodedResult(decodedResult);
        exit(E_BECH32_NO_MEMORY);
    }

    // encode
    err = bech32_encode(bstring, decodedResult->hrp, decodedResult->dp, decodedResult->dplen);
    if(err != E_BECH32_SUCCESS) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_bstring(bstring);
        bech32_free_DecodedResult(decodedResult);
        exit(err);
    }

    // encoding of "cleaned" decoded data should match expected_bstr string
    assert(strcmp(expected_bstr, bstring->string) == 0);

    // free memory
    bech32_free_DecodedResult(decodedResult);
    bech32_free_bstring(bstring);
}

/**
 * Illustrates the error returned when attempting to encode bad data--in this case, "33" is not in the
 * range of allowed data values (0-31).
 */
void badEncoding() {

    // hrp and data to encode
    char hrp[] = "example";
    unsigned char dp[] = {0, 1, 2, 3, 4, 5, 6, 7, 33};

    // create storage for bech32 string
    bech32_bstring *bstring = bech32_create_bstring(sizeof(hrp), sizeof(dp));
    if(!bstring) {
        printf("bech32 string can not be created");
        exit(E_BECH32_NO_MEMORY);
    }

    // encode
    bech32_error err = bech32_encode(bstring, hrp, dp, sizeof(dp));
    if(err != E_BECH32_UNKNOWN_ERROR) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_bstring(bstring);
        exit(err);
    }

    // free memory
    bech32_free_bstring(bstring);
}

/**
 * Illustrates the error returned when attempting to decode bad data--in this case, the "z"
 * character from the bech32 string has been corrupted and changed to a "x". This will cause
 * the checksum verification to fail.
 */
void badDecoding_corruptData() {

    // bech32 string
    char bstr[] = "example1qpzry9x8ge8sqgv";
    // simulate corrupted data--checksum verification will fail
    bstr[10] = 'x';

    // create storage for decoded bech32 data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);
    if(!decodedResult) {
        printf("bech32 DecodedResult can not be created");
        exit(E_BECH32_NO_MEMORY);
    }

    // decode
    bech32_error err = bech32_decode(decodedResult, bstr);
    if(err != E_BECH32_INVALID_CHECKSUM) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_DecodedResult(decodedResult);
        exit(err);
    }

    // free memory
    bech32_free_DecodedResult(decodedResult);
}

/**
 * Illustrates the error returned when attempting to decode bad data--in this case, the "s"
 * character from the bech32 string's checksum has been corrupted and changed to a "q". This
 * will cause the checksum verification to fail.
 */
void badDecoding_corruptChecksum() {

    // bech32 string
    char bstr[] = "example1qpzry9x8ge8sqgv";
    // simulate corrupted checksum--verification will fail
    bstr[19] = 'q';

    // create storage for decoded bech32 data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(bstr);
    if(!decodedResult) {
        printf("bech32 DecodedResult can not be created");
        exit(E_BECH32_NO_MEMORY);
    }

    // decode
    bech32_error err = bech32_decode(decodedResult, bstr);
    if(err != E_BECH32_INVALID_CHECKSUM) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_DecodedResult(decodedResult);
        exit(err);
    }

    // free memory
    bech32_free_DecodedResult(decodedResult);
}

int main() {
    encodeAndDecode();
    decodeAndEncode();
    badEncoding();
    badDecoding_corruptData();
    badDecoding_corruptChecksum();
}
