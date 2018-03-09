#ifndef TXREF_BECH32_H
#define TXREF_BECH32_H

#include <string>
#include <vector>


namespace bech32 {

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

}

#endif //TXREF_BECH32_H
