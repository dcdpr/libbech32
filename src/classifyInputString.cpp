#include "classifyInputString.h"
#include "bech32.h"
#include "txrefCodec.h"
#include <cassert>

namespace {

    InputParam classifyInputStringBase(const std::string & str) {

        // before testing for various txrefs, get rid of any unknown
        // characters, ex: dashes, periods
        std::string s = bech32::stripUnknownChars(str);

        using namespace txref::limits;

        if(s.length() == TXREF_STRING_MIN_LENGTH)
            return txref_param;

        if(s.length() == TXREF_EXT_STRING_MIN_LENGTH_TESTNET)
            return txrefext_param;

        // special case: since TXREF_STRING_MIN_LENGTH_TESTNET == TXREF_EXT_STRING_MIN_LENGTH
        // (22 == 22), then we need to dig deeper and check the first few characters
        if(s.length() == TXREF_STRING_MIN_LENGTH_TESTNET &&
           s.length() == TXREF_EXT_STRING_MIN_LENGTH) {
            if (s[0] == 't' && s[1] == 'x' && s[2] == '1')
                return txrefext_param;
            else
                return txref_param;
        }

        return unknown_param;
    }

    InputParam classifyInputStringMissingHRP(const std::string & str) {

        // before testing for various txrefs, get rid of any unknown
        // characters, ex: dashes, periods
        std::string s = bech32::stripUnknownChars(str);

        using namespace txref::limits;

        if(s.length() == TXREF_STRING_NO_HRP_MIN_LENGTH)
            return txref_param;

        if(s.length() == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH)
            return txrefext_param;

        return unknown_param;
    }

}

InputParam classifyInputString(const std::string & str) {

    if(str.empty())
        return unknown_param;

    // if exactly 64 chars in length, it is likely a transaction id
    if(str.length() == 64)
        return txid_param;

    // if it starts with certain chars, and is of a certain length, it may be a bitcoin address
    if(str[0] == '1' || str[0] == '3' || str[0] == 'm' || str[0] == 'n' || str[0] == '2')
        if(str.length() >= 26 && str.length() < 36)
            return address_param;

    // check if it could be a standard txref or txrefext
    InputParam baseResult = classifyInputStringBase(str);

    // check if it could be a truncated txref or txrefext (missing the HRP)
    InputParam missingResult = classifyInputStringMissingHRP(str);

    // if one result is 'unknown' and the other isn't, then return the good one
    if(baseResult != unknown_param && missingResult == unknown_param)
        return baseResult;
    if(baseResult == unknown_param && missingResult != unknown_param)
        return missingResult;


    // otherwise, just return
    assert(baseResult == unknown_param);
    return baseResult;
}
