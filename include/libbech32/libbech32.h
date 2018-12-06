#ifndef TXREF_BECH32_H
#define TXREF_BECH32_H

#ifdef __cplusplus

#include <string>
#include <vector>


namespace bech32 {

    // The Bech32 separator character
    static const char separator = '1';

    // Represents the payload within a bech32 string.
    // hrp: the human-readable part
    //  dp: the data part
    struct HrpAndDp {
        std::string hrp;
        std::vector<unsigned char> dp;
    };

    // clean a bech32 string of any stray characters not in the allowed charset, except for
    // the separator character, which is '1'
    std::string stripUnknownChars(const std::string & bstring);

    // encode a "human-readable part" and a "data part", returning a bech32 string
    std::string encode(const std::string & hrp, const std::vector<unsigned char> & dp);

    // decode a bech32 string, returning the "human-readable part" and a "data part"
    HrpAndDp decode(const std::string & bstring);

    namespace limits {

        // size of the set of character values which are valid for a bech32 string
        const int VALID_CHARSET_SIZE = 32;

        // while there are only 32 valid character values in a bech32 string, other characters
        // can be present but will be stripped out. however, all character values must fall
        // within the following range.
        const int MIN_BECH32_CHAR_VALUE = 33;  // ascii '!'
        const int MAX_BECH32_CHAR_VALUE = 126; // ascii '~'

        // human-readable part of a bech32 string can only be between 1 and 83 characters long
        const int MIN_HRP_LENGTH = 1;
        const int MAX_HRP_LENGTH = 83;

        // checksum is always 6 chars long
        const int CHECKSUM_LENGTH = 6;

        // entire bech32 string can only be a certain size (after invalid characters are stripped out)
        const int MIN_BECH32_LENGTH = 8;  // MIN_HRP_LENGTH + '1' + CHECKSUM_LENGTH
        const int MAX_BECH32_LENGTH = 90; // MAX_HRP_LENGTH + '1' + CHECKSUM_LENGTH

    }
}

#endif // #ifdef __cplusplus

// C bindings - structs and functions

#ifdef __cplusplus
extern "C" {
#endif

// Represents the payload within a bech32 string.
// hrp: the human-readable part
//  dp: the data part
struct bech32_HrpAndDp {
    char * hrp;
    size_t hrplen;
    unsigned char * dp;
    size_t dplen;
};

enum _bech32_error
{
    E_SUCCESS = 0,
    E_UNKNOWN_ERROR,
    E_NULL_ARGUMENT,
    E_LENGTH_TOO_SHORT,
    E_MAX_ERROR
};
typedef enum _bech32_error bech32_error_t;

extern const char *bech32_errordesc[];

const char * bech32_strerror(bech32_error_t error_code);

// TODO helper function to allocate bech32_HrpAndDp based on the input string

/**
 * clean a bech32 string of any stray characters not in the allowed charset, except for the
 * separator character, which is '1'
 *
 * dstlen should be at least as large as srclen
 *
 * @param dst pointer to memory to put the cleaned string.
 * @param src pointer to the string to be cleaned.
 * @return 0 on success, -1 on error (input/output is NULL, output not long enough for string)
 */
extern int bech32_stripUnknownChars(
        char *dst, size_t dstlen,
        const char *src, size_t srclen);

/**
 * encode a "human-readable part" (ex: "xyz") and a "data part" (ex: {1,2,3}), returning a
 * bech32 string
 *
 * @param bstr pointer to memory to put the bech32 string.
 * @param bstrlen number of bytes allocated at bstr
 * @param hrp pointer to the human-readable part"
 * @param hrplen the length of the "human-readable part" string
 * @param dp pointer to the "data part"
 * @param dplen the length of the "data part" array
 *
 * @return 0 on success, -1 on error (hrp/dp/bstr is NULL, bstr not long enough for bech32 string)
 */
extern int bech32_encode(
        char *bstr, size_t bstrlen,
        const char *hrp, size_t hrplen,
        const unsigned char *dp, size_t dplen);

/**
 * decode a bech32 string, returning the "human-readable part" and a "data part"
 *
 * @param output struct containing decoded "human-readable part" and "data part"
 * @param bstr the bech32 string to decode
 * @param bstrlen the length of the bech32 string
 *
 * @return 0 on success, -1 on error (hrp/dp/bstr is NULL, hrp/dp not long enough for decoded bech32 data)
 */
extern int bech32_decode(
        struct bech32_HrpAndDp *output,
        const char *bstr, size_t bstrlen);

#ifdef __cplusplus
}
#endif

#endif //TXREF_BECH32_H
