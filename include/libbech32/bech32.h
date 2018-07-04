#ifndef TXREF_BECH32_H
#define TXREF_BECH32_H

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

#endif //TXREF_BECH32_H
