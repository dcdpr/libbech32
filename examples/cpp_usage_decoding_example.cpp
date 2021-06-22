#include "libbech32.h"

// make sure we can check these examples even when building a release version
#undef NDEBUG
#include <cassert>

int main() {

    // decode
    bech32::DecodedResult decodedResult = bech32::decode("hello1w0rldjn365x");

    // check for expected values (see cpp_usage_encoding_example.cpp)
    std::string hrp = "hello";
    std::vector<unsigned char> data = {14, 15, 3, 31, 13};
    assert(hrp == decodedResult.hrp);
    assert(data == decodedResult.dp);
    assert(bech32::Encoding::Bech32m == decodedResult.encoding);
}
