#ifndef TXREF_CLASSIFYINPUTSTRING_H
#define TXREF_CLASSIFYINPUTSTRING_H


#include <bech32.h>

enum InputParam { unknown_param, address_param, txid_param, txref_param, txrefext_param };

// This function will determine if the input string is a Bitcoin address, txid,
// txref, or txrefext_param. This is not meant to be an exhaustive test--should only be
// used as a first pass to see what sort of string might be passed in as input.
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

    std::string s = bech32::stripUnknownChars(str);

    // may be be txref (with or without leading HRP)
    if(s.length() == 14 || s.length() == 16 || s.length() == 23)
        return txref_param;

    // may be be txrefext_param (with or without leading HRP)
    if(s.length() == 19 || s.length() == 20 || s.length() == 26)
        return txrefext_param;

    // special case: mainnet txref is same length as mainnet txrefext_param without HRP
    if(s.length() == 17) {
        if (s[0] == 't' && s[1] == 'x' && s[2] == '1')
            return txref_param;
        else
            return txrefext_param;
    }

    return unknown_param;
}

#endif //TXREF_CLASSIFYINPUTSTRING_H
