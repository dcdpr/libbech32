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

    // will print "example1qpzry9x8gnylnjs" ... last 6 characters are the checksum
    std::cout << R"(bech32 encoding of human-readable part 'example' and data part '[0, 1, 2, 3, 4, 5, 6, 7, 8]' is:)" << std::endl;
    std::cout << bstr << std::endl;

    // decode
    bech32::HrpAndDp hd = bech32::decode(bstr);

    assert(hrp == hd.hrp);
    assert(data == hd.dp);
}

void decodeAndEncode() {

    // bech32 string with extra invalid characters
    std::string bstr = " example1:qpz!r--y9#x8&%&%gn-y-lnjs ";
    std::string expected = "example1qpzry9x8gnylnjs";
    // decode - make sure to strip invalid characters before trying to decode
    bech32::HrpAndDp hd = bech32::decode(bech32::stripUnknownChars(bstr));

    // verify decoding
    assert(!hd.hrp.empty() && !hd.dp.empty());

    // encode
    bstr = bech32::encode(hd.hrp, hd.dp);

    // encoding of "cleaned" decoded data should match expected string
    assert(bstr == expected);

    // simulate corrupted data--checksum verification will fail
    bstr[10] = 'x';

    // decode - make sure to strip invalid characters before trying to decode
    hd = bech32::decode(bech32::stripUnknownChars(bstr));

    // verify decoding failed
    assert(hd.hrp.empty() && hd.dp.empty());

}

void badEncoding() {

    // human-readable part
    std::string hrp = "example";
    // data values can be 0-31
    std::vector<unsigned char> data = {0, 1, 2, 3, 4, 5, 6, 7, 33};

    // encode
    try {
        std::string bstr = bech32::encode(hrp, data);
    }
    catch (std::exception &e) {
        assert(strcmp(e.what(), "data value is out of range") == 0);
    }
}

void badDecoding() {

    // valid bech32 string
    std::string bstr = "example1qpzry9x8gnylnjs";
    // simulate corrupted data--checksum verification will fail
    bstr[10] = 'x';

    // decode
    bech32::HrpAndDp hd = bech32::decode(bstr);

    // verify decoding failed
    assert(hd.hrp.empty() && hd.dp.empty());

}

int main() {
    encodeAndDecode();
    decodeAndEncode();
    badEncoding();
    badDecoding();
}
