#include "libbech32.h"
#include <iostream>


int main() {
    // simple human readable part with some data
    std::string hrp = "hello";
    std::vector<unsigned char> data = {14, 15, 3, 31, 13};

    // encode
    std::string bstr = bech32::encode(hrp, data);

    std::cout << R"(bech32 encoding of human-readable part 'hello' and data part '[14, 15, 3, 31, 13]' is:)" << std::endl;
    std::cout << bstr << std::endl;
    // prints "hello1w0rldjn365x"
    // ... "hello" + Bech32.separator ("1") + encoded data ("w0rld") + 6 char checksum ("jn365x")
}
