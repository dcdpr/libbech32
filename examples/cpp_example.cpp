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

    // expected bech32 string output
    char expected[] = "example1qpzry9x8ge8sqgv";

    // encode
    std::string bstr = bech32::encode(hrp, data);
    assert(expected == bstr);

    // decode
    bech32::HrpAndDp hd = bech32::decode(bstr);

    assert(hrp == hd.hrp);
    assert(data == hd.dp);
    assert(bech32::Encoding::Bech32m == hd.encoding);
}

void decodeAndEncode() {

    // bech32 string with extra invalid characters
    std::string bstr = " example1:qpz!r--y9#x8&%&%ge-8-sqgv ";
    std::string expected = "example1qpzry9x8ge8sqgv";
    // decode - make sure to strip invalid characters before trying to decode
    bech32::HrpAndDp hd = bech32::decode(bech32::stripUnknownChars(bstr));

    // verify decoding
    assert(!hd.hrp.empty() && !hd.dp.empty());
    assert(bech32::Encoding::Bech32m == hd.encoding);

    // encode
    bstr = bech32::encode(hd.hrp, hd.dp);

    // encoding of "cleaned" decoded data should match expected string
    assert(bstr == expected);

}

void badEncoding() {

    // human-readable part
    std::string hrp = "example";
    // data values can be 0-31. "33" is invalid
    std::vector<unsigned char> data = {0, 1, 2, 3, 4, 5, 6, 7, 33};

    // encode
    try {
        std::string bstr = bech32::encode(hrp, data);
    }
    catch (std::exception &e) {
        assert(std::string(e.what()) == "data value is out of range");
    }
}

void badDecoding_corruptData() {

    // valid bech32 string
    std::string bstr = "example1qpzry9x8ge8sqgv";
    // simulate corrupted data--checksum verification will fail
    bstr[10] = 'x';

    // decode
    bech32::HrpAndDp hd = bech32::decode(bstr);

    // verify decoding failed
    assert(hd.hrp.empty() && hd.dp.empty());
    assert(bech32::Encoding::None == hd.encoding);

}

void badDecoding_corruptChecksum() {

    // valid bech32 string
    std::string bstr = "example1qpzry9x8ge8sqgv";
    // simulate corrupted checksum--verification will fail
    bstr[19] = 'q';

    // decode
    bech32::HrpAndDp hd = bech32::decode(bstr);

    // verify decoding failed
    assert(hd.hrp.empty() && hd.dp.empty());
    assert(bech32::Encoding::None == hd.encoding);

}

int main() {
    encodeAndDecode();
    decodeAndEncode();
    badEncoding();
    badDecoding_corruptData();
    badDecoding_corruptChecksum();
}
