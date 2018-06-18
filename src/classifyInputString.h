#ifndef TXREF_CLASSIFYINPUTSTRING_H
#define TXREF_CLASSIFYINPUTSTRING_H


#include <bech32.h>
#include <txref.h>


enum InputParam { unknown_param, address_param, txid_param, txref_param, txrefext_param };

// This function will determine if the input string is a Bitcoin address, txid,
// txref, or txrefext_param. This is not meant to be an exhaustive test--should only be
// used as a first pass to see what sort of string might be passed in as input.
inline InputParam classifyInputString(const std::string & str) {

    if(str.empty())
        return unknown_param;

    // if exactly 64 chars in length, it is likely a transaction id
    if(str.length() == 64)
        return txid_param;

    // if it starts with certain chars, and is of a certain length, it may be a bitcoin address
    if(str[0] == '1' || str[0] == '3' || str[0] == 'm' || str[0] == 'n' || str[0] == '2')
        if(str.length() >= 26 && str.length() < 36)
            return address_param;

    std::string s = bech32::stripUnknownChars(str);

    using namespace txref::limits;

    // may be be txref (with or without leading HRP)
    if(s.length() == TXREF_STRING_NO_HRP_MIN_LENGTH ||
       s.length() == TXREF_STRING_MIN_LENGTH_TESTNET ||
       s.length() == TXREF_STRING_NO_HRP_MIN_LENGTH_TESTNET)
        return txref_param;

    // may be be txrefext (with or without leading HRP)
    if(s.length() == TXREF_EXT_STRING_MIN_LENGTH ||
       s.length() == TXREF_EXT_STRING_MIN_LENGTH_TESTNET ||
       s.length() == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH_TESTNET)
        return txrefext_param;

    // special case: mainnet min txref with HRP is same length as mainnet min txrefext without HRP
    if(s.length() == TXREF_STRING_MIN_LENGTH ||
       s.length() == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH ) {
        if (s[0] == 't' && s[1] == 'x' && s[2] == '1')
            return txref_param;
        else
            return txrefext_param;
    }

    return unknown_param;
}

#endif //TXREF_CLASSIFYINPUTSTRING_H
