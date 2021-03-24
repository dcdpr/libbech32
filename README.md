# libbech32

This is a C++ implementation of "Bech32:" a checksummed base32 data
encoding format. It is primarily used as a new bitcoin address format
specified by [BIP 0173](https://github.com/bitcoin/bips/blob/master/bip-0173.mediawiki). 

## Building libbech32

To build libbech32, you will need:

* cmake
* g++, clang or Visual Studio (community edition)

libbech32 uses a pretty standard cmake build system:

```
mkdir build
cd build
cmake ..
make
```

You can also run all the tests:

```
make test
```

### Installing prerequisites

If the above doesn't work, you probably need to install some
prerequisites. For example, on a fresh Debian 10 ("buster") system:

```
sudo apt-get install make gcc g++
```

It is worth getting the latest cmake, so install that the hard way:

```
wget https://cmake.org/files/v3.19/cmake-3.19.4.tar.gz
tar xzf cmake-3.19.4.tar.gz
cd cmake-3.19.4
./configure
make 
sudo make install
```

Now you can again try to build libbech32.

## Example Code

### C++ Encoding Example

```cpp
#include "libbech32.h"
#include <iostream>
#include <cassert>

int main() {
    // simple human readable part with some data
    std::string hrp = "hello";
    std::vector<unsigned char> data = {14, 15, 3, 31, 13};

    // encode
    std::string bstr = bech32::encode(hrp, data);

    // prints "hello1w0rldjn365x" : "hello" + Bech32.separator + encoded data + 6 char checksum
    std::cout << bstr << std::endl;
}
```

### C++ Decoding Example

```cpp
#include "libbech32.h"
#include <iostream>
#include <cassert>

int main() {
    bech32::DecodedResult decodedResult = bech32::decode("hello1w0rldjn365x");

    assert(hrp == decodedResult.hrp);
    assert(data == decodedResult.dp);
    assert(bech32::Encoding::Bech32m == decodedResult.encoding);
}
```

For more C++ examples, see `examples/cpp_example.cpp`

### C Encoding Example

```C
#include "libbech32.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

int main() {
    // simple human readable part with some data
    char hrp[] = "hello";
    unsigned char dp[] = {14, 15, 3, 31, 13};

    // create output for bech32 string
    bech32_bstring *bstring = bech32_create_bstring(strlen(hrp), sizeof(dp));

    // encode
    assert(bech32_encode(bstring, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);

    // prints "hello1w0rldjn365x" : "hello" + Bech32.separator + encoded data + 6 char checksum
    printf("bech32 encoding of human-readable part \'hello\' and data part \'[14, 15, 3, 31, 13]\' is:\n");
    printf("%s\n", bstring->string);

    // free memory
    bech32_free_bstring(bstring);
}
```

### C Decoding Example

```C
#include "libbech32.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

int main() {
    // allocate memory for decoded data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult("hello1w0rldjn365x");

    // decode
    assert(bech32_decode(decodedResult, "hello1w0rldjn365x") == E_BECH32_SUCCESS);
    assert(strcmp(decodedResult->hrp, "hello") == 0);
    assert(decodedResult->dp[0] == 14);
    assert(ENCODING_BECH32M == decodedResult->encoding);

    // free memory
    bech32_free_DecodedResult(decodedResult);
}
```

For more C examples, see `examples/c_example.cpp`


## Regarding bech32 checksums

The Bech32 data encoding format was first proposed by Pieter Wuille in early 2017 in
[BIP 0173](https://github.com/bitcoin/bips/blob/master/bip-0173.mediawiki). Later, in November 2019, Pieter published
some research regarding that an exponent used in the bech32 checksum algorithm (value = 1) may not be
optimal for the error detecting properties of bech32. In February 2021, Pieter published
[BIP 0350](https://github.com/bitcoin/bips/blob/master/bip-0350.mediawiki) reporting that "exhaustive analysis" showed the best possible exponent value is
0x2bc830a3. This improved variant of Bech32 is called "Bech32m".

When decoding a possible bech32 encoded string, libbech32 returns an enum value showing whether bech32m or bech32
was used to encode. This can be seen in the examples above.

When encoding data, libbech32 defaults to using the new exponent value of 0x2bc830a3. If the original exponent value
of 1 is desired, then the following functions may be used:

### C++ Usage Example

```cpp
    /// ... as above ...

    // encode
    std::string bstr = bech32::encodeUsingOriginalConstant(hrp, data);

    /// ... as above ...
```

### C Usage Example

```C
    /// ... as above ...

    // encode
    assert(bech32_encode_using_original_constant(bstring, hrp, dp, sizeof(dp)) == E_BECH32_SUCCESS);

    /// ... as above ...
```
