#ifndef TXREF_CLASSIFYINPUTSTRING_H
#define TXREF_CLASSIFYINPUTSTRING_H


#include "bech32.h"
#include "txref.h"


enum InputParam { unknown_param, address_param, txid_param, txref_param, txrefext_param };

// This function will determine if the input string is a Bitcoin address, txid,
// txref, or txrefext_param. This is not meant to be an exhaustive test--should only be
// used as a first pass to see what sort of string might be passed in as input.
InputParam classifyInputString(const std::string & str);

#endif //TXREF_CLASSIFYINPUTSTRING_H
