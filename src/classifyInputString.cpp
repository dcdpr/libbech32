#include "classifyInputString.h"
#include "libbech32.h"
#include "libtxref.h"
#include <cassert>

namespace {

    InputParam classifyInputStringBase(const std::string & str) {

        // before testing for various txrefs, get rid of any unknown
        // characters, ex: dashes, periods
        std::string s = bech32::stripUnknownChars(str);

        using namespace txref::limits;

        if(s.length() == TXREF_STRING_MIN_LENGTH || s.length() == TXREF_STRING_MIN_LENGTH_TESTNET )
            return txref_param;

        if(s.length() == TXREF_EXT_STRING_MIN_LENGTH || s.length() == TXREF_EXT_STRING_MIN_LENGTH_TESTNET)
            return txrefext_param;

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

    // special case: if baseResult is 'txref_param' and missingResult is 'txrefext_param' then
    // we need to dig deeper as TXREF_STRING_MIN_LENGTH == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH
    if (baseResult == txref_param && missingResult == txrefext_param) {
        if (str[0] == txref::BECH32_HRP_MAIN[0] && // 't'
            str[1] == txref::BECH32_HRP_MAIN[1] && // 'x'
            str[2] == bech32::separator)           // '1'
            return txref_param;
        else
            return txrefext_param;
    }

    // otherwise, just return
    assert(baseResult == unknown_param);
    return baseResult;
}
