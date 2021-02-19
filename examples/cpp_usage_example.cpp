// This small example is mainly meant for easy copy/paste into the README.md
#include "libbech32.h"
#include <iostream>

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <cassert>

int main() {
    // simple human readable part with some data
    std::string hrp = "hello";
    std::vector<unsigned char> data = {14, 15, 3, 31, 13};

    // encode
    std::string bstr = bech32::encode(hrp, data);

    // prints "hello1w0rldjn365x" : "hello" + Bech32.separator + encoded data + 6 char checksum
    std::cout << R"(bech32 encoding of human-readable part 'hello' and data part '[14, 15, 3, 31, 13]' is:)" << std::endl;
    std::cout << bstr << std::endl;

    // decode
    bech32::DecodedResult decodedResult = bech32::decode(bstr);

    assert(hrp == decodedResult.hrp);
    assert(data == decodedResult.dp);
    assert(bech32::Encoding::Bech32m == decodedResult.encoding);
}
