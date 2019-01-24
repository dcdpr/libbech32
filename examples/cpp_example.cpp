#include "libbech32.h"
#include <iostream>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <cassert>

void encodeAndDecode() {

    // human-readable part
    std::string hrp = "example";
    // data values can be 0-31
    std::vector<unsigned char> data = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    // encode
    std::string bstr = bech32::encode(hrp, data);

    // will print "example1qpzry9x8gvmqvdw" ... last 6 characters are the checksum
    std::cout << R"(bech32 encoding of human-readable part 'example' and data part '[0, 1, 2, 3, 4, 5, 6, 7, 8]' is:)" << std::endl;
    std::cout << bstr << std::endl;

    // decode
    bech32::HrpAndDp b = bech32::decode(bstr);

    assert(hrp == b.hrp);
    assert(data == b.dp);
}

void decodeAndEncode() {

    // bech32 string with extra invalid characters
    std::string bstr = "tx1:yjk!0-uq#ay-z%u4x-nk6u&-pc";
    std::string expected = "tx1yjk0uqayzu4xnk6upc";
    // decode - make sure to strip invalid characters before trying to decode
    bech32::HrpAndDp b = bech32::decode(bech32::stripUnknownChars(bstr));

    // encode
    bstr = bech32::encode(b.hrp, b.dp);

    // encoding of "cleaned" decoded data should match expected string
    assert(bstr == expected);
}

int main() {

    encodeAndDecode();

    decodeAndEncode();

}
