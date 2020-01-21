#include "libbech32.h"
#include <algorithm>
#include <stdexcept>

namespace {

    using namespace bech32::limits;

    // exponent used in checksum generation, taken from recommendation
    // in: https://gist.github.com/sipa/a9845b37c1b298a7301c33a04090b2eb
    const unsigned int M = 0x3FFFFFFF;

    /** The Bech32 character set for encoding. The index into this string gives the char
     * each value is mapped to, i.e., 0 -> 'q', 10 -> '2', etc. This comes from the table
     * in BIP-0173 */
    const char charset[VALID_CHARSET_SIZE] = {
            'q', 'p', 'z', 'r', 'y', '9', 'x', '8', 'g', 'f', '2', 't', 'v', 'd', 'w', '0',
            's', '3', 'j', 'n', '5', '4', 'k', 'h', 'c', 'e', '6', 'm', 'u', 'a', '7', 'l'
    };

    /** The Bech32 character set for decoding. This comes from the table in BIP-0173
     *
     * This will help map both upper and lowercase chars into the proper code (or index
     * into the above charset). For instance, 'Q' (ascii 81) and 'q' (ascii 113)
     * are both set to index 0 in this table. Invalid chars are set to -1 */
    const int REVERSE_CHARSET_SIZE = 128;
    const int8_t charset_rev[REVERSE_CHARSET_SIZE] = {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            15, -1, 10, 17, 21, 20, 26, 30,  7,  5, -1, -1, -1, -1, -1, -1,
            -1, 29, -1, 24, 13, 25,  9,  8, 23, -1, 18, 22, 31, 27, 19, -1,
            1,  0,  3, 16, 11, 28, 12, 14,  6,  4,  2, -1, -1, -1, -1, -1,
            -1, 29, -1, 24, 13, 25,  9,  8, 23, -1, 18, 22, 31, 27, 19, -1,
            1,  0,  3, 16, 11, 28, 12, 14,  6,  4,  2, -1, -1, -1, -1, -1
    };

    // bech32 string can not mix upper and lower case
    void rejectBStringMixedCase(const std::string &bstring) {
        bool atLeastOneUpper = std::any_of(bstring.begin(), bstring.end(), &::isupper);
        bool atLeastOneLower = std::any_of(bstring.begin(), bstring.end(), &::islower);
        if(atLeastOneUpper && atLeastOneLower) {
            throw std::runtime_error("bech32 string is mixed case");
        }
    }

    // bech32 string values must be in range ASCII 33-126
    void rejectBStringValuesOutOfRange(const std::string &bstring) {
        if(std::any_of(bstring.begin(), bstring.end(), [](char ch){
            return ch < MIN_BECH32_CHAR_VALUE || ch > MAX_BECH32_CHAR_VALUE; } )) {
            throw std::runtime_error("bech32 string has value out of range");
        }
    }

    // bech32 string can be at most 90 characters long
    void rejectBStringTooLong(const std::string &bstring) {
        if (bstring.size() > MAX_BECH32_LENGTH)
            throw std::runtime_error("bech32 string too long");
    }

    // bech32 string must be at least 8 chars long: HRP (min 1 char) + '1' + 6-char checksum
    void rejectBStringTooShort(const std::string &bstring) {
        if (bstring.size() < MIN_BECH32_LENGTH)
            throw std::runtime_error("bech32 string too short");
    }

    // bech32 string must contain the separator character
    void rejectBStringWithNoSeparator(const std::string &bstring) {
        if(!std::any_of(bstring.begin(), bstring.end(), [](char ch) { return ch == bech32::separator; })) {
            throw std::runtime_error("bech32 string is missing separator character");
        }
    }

    // bech32 string must conform to rules laid out in BIP-0173
    void rejectBStringThatIsntWellFormed(const std::string &bstring) {
        rejectBStringTooShort(bstring);
        rejectBStringTooLong(bstring);
        rejectBStringMixedCase(bstring);
        rejectBStringValuesOutOfRange(bstring);
        rejectBStringWithNoSeparator(bstring);
    }

    // return the position of the separator character
    uint64_t findSeparatorPosition(const std::string &bstring) {
        return bstring.find_last_of(bech32::separator);
    }


    // split the hrp from the dp
    bech32::HrpAndDp splitString(const std::string & bstring) {
        auto pos = findSeparatorPosition(bstring);
        std::string hrp = bstring.substr(0, pos);
        std::string dpstr = bstring.substr(pos+1);
        // convert dpstr to dp vector
        std::vector<unsigned char> dp(bstring.size() - (pos + 1));
        for(std::string::size_type i = 0; i < dpstr.size(); ++i) {
            dp[i] = static_cast<unsigned char>(dpstr[i]);
        }
        return {hrp, dp};
    }

    void convertToLowercase(std::string & str) {
        std::transform(str.begin(), str.end(), str.begin(), &::tolower);
    }

    // dp needs to be mapped using the charset_rev table
    void mapDP(std::vector<unsigned char> &dp) {
        for(unsigned char &c : dp) {
            if(c > REVERSE_CHARSET_SIZE - 1)
                throw std::runtime_error("data part contains character value out of range");
            int8_t d = charset_rev[c];
            if(d == -1)
                throw std::runtime_error("data part contains invalid character");
            c = static_cast<unsigned char>(d);
        }
    }

    // using the charset of valid chars, map the incoming data
    std::string mapToCharset(std::vector<unsigned char> &data) {
        std::string ret;
        ret.reserve(data.size());
        for (unsigned char c : data) {
            if(c > VALID_CHARSET_SIZE - 1)
                throw std::runtime_error("data part contains invalid character");
            ret += charset[c];
        }
        return ret;
    }

    // "expand" the HRP -- adapted from example in BIP-0173
    //
    // To expand the chars of the HRP means to create a new collection of
    // the high bits of each character's ASCII value, followed by a zero,
    // and then the low bits of each character. See BIP-0173 for rationale.
    std::vector<unsigned char> expandHrp(const std::string & hrp) {
        std::string::size_type sz = hrp.size();
        std::vector<unsigned char> ret(sz * 2 + 1);
        for(std::string::size_type i=0; i < sz; ++i) {
            auto c = static_cast<unsigned char>(hrp[i]);
            ret[i] = c >> 5u;
            ret[i + sz + 1] = c & static_cast<unsigned char>(0x1f);
        }
        ret[sz] = 0;
        return ret;
    }

    // Concatenate two vectors
    std::vector<unsigned char> cat(const std::vector<unsigned char> & x, const std::vector<unsigned char> & y) {
        std::vector<unsigned char> ret(x);
        ret.insert(ret.end(), y.begin(), y.end());
        return ret;
    }

    // Find the polynomial with value coefficients mod the generator as 30-bit.
    // Adapted from Pieter Wuille's code in BIP-0173
    uint32_t polymod(const std::vector<unsigned char> &values) {
        uint32_t chk = 1;
        for (unsigned char value : values) {
            auto top = static_cast<uint8_t>(chk >> 25u);
            chk = static_cast<uint32_t>(
                    (chk & 0x1ffffffu) << 5u ^ value ^
                    (-((top >> 0) & 1u) & 0x3b6a57b2UL) ^
                    (-((top >> 1) & 1u) & 0x26508e6dUL) ^
                    (-((top >> 2) & 1u) & 0x1ea119faUL) ^
                    (-((top >> 3) & 1u) & 0x3d4233ddUL) ^
                    (-((top >> 4) & 1u) & 0x2a1462b3UL));
        }
        return chk;
    }

    bool verifyChecksum(const std::string &hrp, const std::vector<unsigned char> &dp) {
        return polymod(cat(expandHrp(hrp), dp)) == M;
    }

    void stripChecksum(std::vector<unsigned char> &dp) {
        dp.erase(dp.end() - CHECKSUM_LENGTH, dp.end());
    }

    std::vector<unsigned char>
    createChecksum(const std::string &hrp, const std::vector<unsigned char> &dp) {
        std::vector<unsigned char> c = cat(expandHrp(hrp), dp);
        c.resize(c.size() + CHECKSUM_LENGTH);
        uint32_t mod = polymod(c) ^ M;
        std::vector<unsigned char> ret(CHECKSUM_LENGTH);
        for(std::vector<unsigned char>::size_type i = 0; i < CHECKSUM_LENGTH; ++i) {
            ret[i] = static_cast<unsigned char>((mod >> (5 * (5 - i))) & 31u);
        }
        return ret;
    }

    void rejectHRPTooShort(const std::string &hrp) {
        if(hrp.size() < MIN_HRP_LENGTH)
            throw std::runtime_error("HRP must be at least one character");
    }

    void rejectHRPTooLong(const std::string &hrp) {
        if(hrp.size() > MAX_HRP_LENGTH)
            throw std::runtime_error("HRP must be less than 84 characters");
    }

    void rejectDPTooShort(const std::vector<unsigned char> &dp) {
        if(dp.size() < CHECKSUM_LENGTH)
            throw std::runtime_error("data part must be at least six characters");
    }

    // data values must be in range ASCII 0-31 in order to index into the charset
    void rejectDataValuesOutOfRange(const std::vector<unsigned char> &dp) {
        if(std::any_of(dp.begin(), dp.end(), [](char ch){ return ch > VALID_CHARSET_SIZE-1; } )) {
            throw std::runtime_error("data value is out of range");
        }
    }

    // length of human part plus length of data part plus separator char plus 6 char
    // checksum must be less than 90
    void rejectBothPartsTooLong(const std::string &hrp, const std::vector<unsigned char> &dp) {
        if(hrp.length() + dp.size() + 1 + CHECKSUM_LENGTH > MAX_BECH32_LENGTH) {
            throw std::runtime_error("length of hrp + length of dp is too large");
        }
    }

    // return true if the arg c is within the allowed charset
    bool isAllowedChar(std::string::value_type c) {
        return std::find(std::begin(charset), std::end(charset), c) !=
                std::end(charset);
    }

}


namespace bech32 {

    // clean a bech32 string of any stray characters not in the allowed charset, except for
    // the separator character, which is '1'
    std::string stripUnknownChars(const std::string &bstring) {
        std::string ret(bstring);
        ret.erase(
                std::remove_if(
                        ret.begin(), ret.end(),
                        [](char x){return (!isAllowedChar(static_cast<char>(::tolower(x))) && x!=separator);}),
                ret.end());
        return ret;
    }

    // encode a "human-readable part" and a "data part", returning a bech32 string
    std::string encode(const std::string &hrp, const std::vector<unsigned char> &dp) {
        rejectHRPTooShort(hrp);
        rejectHRPTooLong(hrp);
        rejectBothPartsTooLong(hrp, dp);
        rejectDataValuesOutOfRange(dp);

        std::string hrpCopy = hrp;
        convertToLowercase(hrpCopy);
        std::vector<unsigned char> checksum = createChecksum(hrpCopy, dp);
        std::string ret = hrpCopy + '1';
        std::vector<unsigned char> combined = cat(dp, checksum);
        ret.reserve(ret.size() + combined.size());
        for (unsigned char c : combined) {
            if(c > limits::VALID_CHARSET_SIZE - 1)
                throw std::runtime_error("data part contains invalid character");
            ret += charset[c];
        }
        return ret;
    }

    // decode a bech32 string, returning the "human-readable part" and a "data part"
    HrpAndDp decode(const std::string & bstring) {
        rejectBStringThatIsntWellFormed(bstring);
        HrpAndDp b = splitString(bstring);
        rejectHRPTooShort(b.hrp);
        rejectHRPTooLong(b.hrp);
        rejectDPTooShort(b.dp);
        convertToLowercase(b.hrp);
        mapDP(b.dp);
        if (verifyChecksum(b.hrp, b.dp)) {
            stripChecksum(b.dp);
            return b;
        }
        else {
            return HrpAndDp();
        }
    }

}

// C bindings - functions

const char *bech32_errordesc[] = {
        "Success",
        "Unknown error",
        "Function argument was null",
        "Function argument length was too short",
        "Invalid Checksum",
        "Max error"
};

/**
 * Returns error message string corresponding to the error code
 *
 * @param error_code the error code to convert
 *
 * @return error message string corresponding to the error code
 */
extern "C"
const char * bech32_strerror(bech32_error error_code) {
    const char * result = "";
    if(error_code >= E_BECH32_SUCCESS && error_code < E_BECH32_MAX_ERROR) {
        result = bech32_errordesc[error_code];
    }
    else
        result = bech32_errordesc[E_BECH32_UNKNOWN_ERROR];
    return result;
}

/**
 * Allocates memory for a bech32_HrpAndDp struct based on the size of the bech32 string passed in.
 *
 * This memory must be freed using the free_HrpAndDp_storage function.
 *
 * @param bstr the bech32 string to be decoded by bech32_decode()
 *
 * @return a pointer to a new bech32_HrpAndDp struct, or NULL if error
 */
extern "C"
bech32_HrpAndDp * create_HrpAndDp_storage(const char *bstr) {
    // the storage needed for a decoded bech32 string can be easily determined by the
    // length of the input string
    std::string inputStr(bstr);
    size_t index_of_separator = inputStr.find_first_of('1');
    size_t number_of_hrp_characters = index_of_separator;
    size_t number_of_data_characters =
            inputStr.length() - (index_of_separator+1) - bech32::limits::CHECKSUM_LENGTH; // +1 for zero-based index

    auto hrpdp = static_cast<bech32_HrpAndDp *>(malloc(sizeof (bech32_HrpAndDp)));
    if(hrpdp == nullptr)
        return nullptr;
    hrpdp->hrplen = number_of_hrp_characters + 1; // +1 for '\0'
    hrpdp->hrp = static_cast<char *>(calloc(hrpdp->hrplen, 1));
    if(hrpdp->hrp == nullptr) {
        free(hrpdp);
        return nullptr;
    }
    hrpdp->dplen = number_of_data_characters + 1; // +1 for '\0'
    hrpdp->dp = static_cast<unsigned char *>(calloc(hrpdp->dplen, 1));
    if(hrpdp->dp == nullptr) {
        free(hrpdp->hrp);
        free(hrpdp);
        return nullptr;
    }

    return hrpdp;
}

/**
 * Frees memory for a txref_LocationData struct.
 */
extern "C"
void free_HrpAndDp_storage(bech32_HrpAndDp *hrpdp) {
    free(hrpdp->dp);
    free(hrpdp->hrp);
    free(hrpdp);
}

/**
 * clean a bech32 string of any stray characters not in the allowed charset, except for the
 * separator character, which is '1'
 *
 * dstlen should be at least as large as srclen
 *
 * @param dst pointer to memory to put the cleaned string
 * @param src pointer to the string to be cleaned
 *
 * @return E_BECH32_SUCCESS on success, others on error (input/output is NULL, output not
 * long enough for string)
 */
extern "C"
bech32_error bech32_stripUnknownChars(
        char *dst, size_t dstlen,
        const char *src, size_t srclen) {

    if(src == nullptr)
        return E_BECH32_NULL_ARGUMENT;
    if(dst == nullptr)
        return E_BECH32_NULL_ARGUMENT;
    if(dstlen > srclen)
        return E_BECH32_LENGTH_TOO_SHORT;

    std::string inputStr(src);
    std::string result = bech32::stripUnknownChars(inputStr);
    if(dstlen < result.size()+1)
        return E_BECH32_LENGTH_TOO_SHORT;

    std::copy_n(result.begin(), result.size(), dst);
    dst[result.size()] = '\0';

    return E_BECH32_SUCCESS;
}

/**
 * encode a "human-readable part" (ex: "xyz") and a "data part" (ex: {1,2,3}), returning a
 * bech32 string
 *
 * @param bstr pointer to memory to copy the output encoded bech32 string.
 * @param bstrlen number of bytes allocated at bstr
 * @param hrp pointer to the "human-readable part"
 * @param hrplen the length of the "human-readable part" string
 * @param dp pointer to the "data part"
 * @param dplen the length of the "data part" array
 *
 * @return E_BECH32_SUCCESS on success, others on error (hrp/dp/bstr is NULL, bstr not
 * long enough for bech32 string)
 */
extern "C"
bech32_error bech32_encode(
        char *bstr, size_t bstrlen,
        const char *hrp, size_t hrplen,
        const unsigned char *dp, size_t dplen) {

    if(bstr == nullptr)
        return E_BECH32_NULL_ARGUMENT;
    if(hrp == nullptr)
        return E_BECH32_NULL_ARGUMENT;
    if(dp == nullptr)
        return E_BECH32_NULL_ARGUMENT;

    std::string hrpStr(hrp);
    if(hrpStr.size() > hrplen-1)
        return E_BECH32_LENGTH_TOO_SHORT;
    std::vector<unsigned char> dpVec(dp, dp + dplen);

    std::string b;
    try {
        b = bech32::encode(hrpStr, dpVec);
    }
    catch (std::exception &) {
        // todo: convert exception message
        return E_BECH32_UNKNOWN_ERROR;
    }
    if(b.size() > bstrlen-1)
        return E_BECH32_LENGTH_TOO_SHORT;

    std::copy_n(b.begin(), b.size(), bstr);
    bstr[b.size()] = '\0';

    return E_BECH32_SUCCESS;
}

/**
 * decode a bech32 string, returning the "human-readable part" and a "data part"
 *
 * @param output pointer to struct to copy the decoded "human-readable part" and "data part"
 * @param bstr the bech32 string to decode
 * @param bstrlen the length of the bech32 string
 *
 * @return E_BECH32_SUCCESS on success, others on error (hrp/dp/bstr is NULL, hrp/dp not
 * long enough for decoded bech32 data)
 */
extern "C"
bech32_error bech32_decode(bech32_HrpAndDp *output, char const *bstr, size_t bstrlen) {

    if(output == nullptr)
        return E_BECH32_NULL_ARGUMENT;
    if(output->hrp == nullptr)
        return E_BECH32_NULL_ARGUMENT;
    if(output->dp == nullptr)
        return E_BECH32_NULL_ARGUMENT;
    if(bstr == nullptr)
        return E_BECH32_NULL_ARGUMENT;

    std::string inputStr(bstr);
    if(inputStr.size() > bstrlen-1)
        return E_BECH32_LENGTH_TOO_SHORT;

    bech32::HrpAndDp hrpAndDp;
    try {
        hrpAndDp = bech32::decode(inputStr);
    } catch (std::exception &) {
        // todo: convert exception message
        return E_BECH32_UNKNOWN_ERROR;
    }

    if(hrpAndDp.hrp.empty() && hrpAndDp.dp.empty())
        return E_BECH32_INVALID_CHECKSUM;

    if(hrpAndDp.hrp.size() > output->hrplen-1)
        return E_BECH32_LENGTH_TOO_SHORT;
    if(hrpAndDp.dp.size() > output->dplen)
        return E_BECH32_LENGTH_TOO_SHORT;

    std::copy_n(hrpAndDp.hrp.begin(), hrpAndDp.hrp.size(), output->hrp);
    output->hrp[hrpAndDp.hrp.size()] = '\0';
    std::copy_n(hrpAndDp.dp.begin(), hrpAndDp.dp.size(), output->dp);

    return E_BECH32_SUCCESS;
}
