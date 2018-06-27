#include "classifyInputString.h"

namespace {

    InputParam classifyInputStringBase(const std::string & str) {

        if(str.empty())
            return unknown_param;

        // if exactly 64 chars in length, it is likely a transaction id
        if(str.length() == 64)
            return txid_param;

        // if it starts with certain chars, and is of a certain length, it may be a bitcoin address
        if(str[0] == '1' || str[0] == '3' || str[0] == 'm' || str[0] == 'n' || str[0] == '2')
            if(str.length() >= 26 && str.length() < 36)
                return address_param;

        // before testing for various txrefs, get rid of any unknown characters
        std::string s = bech32::stripUnknownChars(str);

        using namespace txref::limits;

        // may be be txref (with leading HRP)
        if(s.length() == TXREF_STRING_MIN_LENGTH ||
           s.length() == TXREF_STRING_MIN_LENGTH_TESTNET)
            return txref_param;

        // may be be txrefext (with leading HRP)
        if(s.length() == TXREF_EXT_STRING_MIN_LENGTH ||
           s.length() == TXREF_EXT_STRING_MIN_LENGTH_TESTNET)
            return txrefext_param;

        return unknown_param;
    }

    InputParam classifyInputStringMissingHRP(const std::string & str) {

        // before testing for various txrefs, get rid of any unknown characters
        std::string s = bech32::stripUnknownChars(str);

        using namespace txref::limits;

        // may be be txref (without leading HRP)
        if(s.length() == TXREF_STRING_NO_HRP_MIN_LENGTH ||
           s.length() == TXREF_STRING_NO_HRP_MIN_LENGTH_TESTNET)
            return txref_param;

        // may be be txrefext (without leading HRP)
        if(s.length() == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH ||
           s.length() == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH_TESTNET)
            return txrefext_param;

        return unknown_param;
    }

}

InputParam classifyInputString(const std::string & str) {

    InputParam baseResult = classifyInputStringBase(str);
    InputParam missingResult = classifyInputStringMissingHRP(str);

    // if one result is 'unknown' and the other isn't, then return the good one
    if(baseResult != unknown_param && missingResult == unknown_param)
        return baseResult;
    if(baseResult == unknown_param && missingResult != unknown_param)
        return missingResult;

    // special case: if baseResult is 'txref' and the other is 'txrefext' then we need to dig
    // deeper as mainnet min txref with HRP is same length as mainnet min txrefext without HRP
    if (baseResult == txref_param && missingResult == txrefext_param) {
        if (str[0] == 't' && str[1] == 'x' && str[2] == '1')
            return txref_param;
        else
            return txrefext_param;
    }

    // otherwise, just return (should be unknown)
    return baseResult;
}
