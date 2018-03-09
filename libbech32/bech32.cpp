#include "bech32.h"
#include <algorithm>
#include <stdexcept>

namespace {

    /** The Bech32 separator character */
    const char separator = '1';

    /** The Bech32 character set for encoding. The index in this string gives the char
     * each value is mapped to, i.e., 0 -> q, 10 -> 2, etc. This comes from the table
     * in BIP-0173 */
    const char charset[] = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

    /** The Bech32 character set for decoding. This comes from the table in BIP-0173
     *
     * This will help map both upper and lowercase chars into the proper code (or index
     * into the above charset). For instance, 'Q' (ascii 81) and 'q' (ascii 113)
     * are both set to index 0 in this table. Invalid chars are set to -1 */
    const int8_t charset_rev[128] = {
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
        if(atLeastOneUpper == atLeastOneLower) {
            throw std::runtime_error("dp is mixed case");
        }
    }

    // bech32 string values must be in range ASCII 33-126
    void rejectBStringValuesOutOfRange(const std::string &bstring) {
        if(std::any_of(bstring.begin(), bstring.end(), [](char ch){ return ch < 33 || ch > 126; } )) {
            throw std::runtime_error("dp is out of range");
        }
    }

    // bech32 string can be at most 90 characters long
    void rejectBStringTooLong(const std::string &bstring) {
        if (bstring.size() > 90)
            throw std::runtime_error("dp too long");
    }

    // bech32 string must be at least 8 chars long: HRP (min 1 char) + '1' + 6-char checksum
    void rejectBStringTooShort(const std::string &bstring) {
        if (bstring.size() < 8)
            throw std::runtime_error("dp too short");
    }

    // bech32 string must contain the separator character
    void rejectBStringWithNoSeparator(const std::string &bstring) {
        if(!std::any_of(bstring.begin(), bstring.end(), [](char ch) { return ch == separator; })) {
            throw std::runtime_error("dp is missing separator character");
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
        return bstring.find_last_of(separator);
    }


    // split the hrp from the dp
    bech32::HrpAndDp splitString(const std::string & bstring) {
        auto pos = findSeparatorPosition(bstring);
        std::string hrp = bstring.substr(0, pos);
        std::string dpstr = bstring.substr(pos+1);
        // convert dpstr to dp vector
        std::vector<unsigned char> dp(bstring.size() - (pos + 1));
        for(std::string::size_type i = 0; i < dpstr.size(); ++i) {
            dp[i] = (unsigned char) dpstr[i];
        }
        return {hrp, dp};
    }

    void convertToLowercase(std::string & str) {
        std::transform(str.begin(), str.end(), str.begin(), &::tolower);
    }

    // dp needs to be mapped using the charset_rev table
    void mapDP(std::vector<unsigned char> &dp) {
        for(unsigned char &c : dp) {
            int8_t d = charset_rev[c];
            if(d == -1)
                throw std::runtime_error("dp contains invalid character");
            c = (unsigned char) d;
        }
    }

    // "expand" the HRP -- adapted from example in BIP-0173
    std::vector<unsigned char> expandHrp(const std::string & hrp) {
        std::string::size_type sz = hrp.size();
        std::vector<unsigned char> ret(sz * 2 + 1);
        for(std::string::size_type i=0; i < sz; ++i) {
            auto c = (unsigned char) hrp[i];
            ret[i] = c >> 5;
            ret[i + sz + 1] = c & 0x1f;
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

    // Find the polynomial with value coefficients mod the generator as 30-bit. Adapted from Pieter Wuille's code
    uint32_t polymod(const std::vector<unsigned char> &values) {
        uint32_t chk = 1;
        for (unsigned char value : values) {
            auto top = static_cast<uint8_t>(chk >> 25);
            chk = static_cast<uint32_t>(
                    (chk & 0x1ffffff) << 5 ^ value ^
                    (-((top >> 0) & 1) & 0x3b6a57b2UL) ^
                    (-((top >> 1) & 1) & 0x26508e6dUL) ^
                    (-((top >> 2) & 1) & 0x1ea119faUL) ^
                    (-((top >> 3) & 1) & 0x3d4233ddUL) ^
                    (-((top >> 4) & 1) & 0x2a1462b3UL));
        }
        return chk;
    }

    bool verifyChecksum(const std::string &hrp, const std::vector<unsigned char> &dp) {
        return polymod(cat(expandHrp(hrp), dp)) == 1;
    }

    void stripChecksum(std::vector<unsigned char> &dp) {
        dp.erase(dp.end() - 6, dp.end());
    }

    std::vector<unsigned char>
    createChecksum(const std::string &hrp, const std::vector<unsigned char> &dp) {
        std::vector<unsigned char> c = cat(expandHrp(hrp), dp);
        c.resize(c.size() + 6);
        uint32_t mod = polymod(c) ^ 1;
        std::vector<unsigned char> ret(6);
        for(std::vector<unsigned char>::size_type i = 0; i < 6; ++i) {
            ret[i] = (mod >> (5 * (5 - i))) & 31;
        }
        return ret;
    }

    void rejectHRPTooShort(const std::string &hrp) {
        if(hrp.empty())
            throw std::runtime_error("HRP must be at least one character");
    }

    void rejectHRPTooLong(const std::string &hrp) {
        if(hrp.size() > 83)
            throw std::runtime_error("HRP must be less than 84 characters");
    }

    void rejectDPTooShort(const std::vector<unsigned char> &dp) {
        if(dp.size() < 6)
            throw std::runtime_error("dp must be at least six characters");
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
                        [](char x){return (!isAllowedChar(::tolower(x)) && x!=separator);}),
                ret.end());
        return ret;
    }

    // encode a "human-readable part" and a "data part", returning a bech32 string
    std::string encode(const std::string &hrp, const std::vector<unsigned char> &dp) {
        rejectHRPTooShort(hrp);
        rejectHRPTooLong(hrp);
        std::string hrpCopy = hrp;
        convertToLowercase(hrpCopy);
        std::vector<unsigned char> checksum = createChecksum(hrpCopy, dp);
        std::string ret = hrpCopy + '1';
        std::vector<unsigned char> combined = cat(dp, checksum);
        ret.reserve(ret.size() + combined.size());
        for (unsigned char c : combined) {
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

