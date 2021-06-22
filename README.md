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
sudo apt-get update
sudo apt-get install make gcc g++
```

It is worth getting the latest cmake (check [cmake.org](https://cmake.org/download/) for 
the latest release), so install that the hard way:

```
sudo apt-get install libssl-dev # recent cmake needs ssl
wget https://github.com/Kitware/CMake/releases/download/v3.20.1/cmake-3.20.1.tar.gz
wget https://github.com/Kitware/CMake/releases/download/v3.20.1/cmake-3.20.1-SHA-256.txt
shasum -a 256 --ignore-missing -c cmake-3.20.1-SHA-256.txt # make sure this says "OK"
tar xzf cmake-3.20.1.tar.gz
cd cmake-3.20.1
./configure
make 
sudo make install
```

Now you can again try to build libbech32.

## Usage Examples

### C++ Encoding Example

```cpp
#include "libbech32.h"
#include <iostream>

int main() {
    // simple human readable part with some data
    std::string hrp = "hello";
    std::vector<unsigned char> data = {14, 15, 3, 31, 13};

    // encode
    std::string bstr = bech32::encode(hrp, data);

    std::cout << bstr << std::endl;
    // prints "hello1w0rldjn365x"
    // ... "hello" + Bech32.separator ("1") + encoded data ("w0rld") + 6 char checksum ("jn365x")
}
```

### C++ Decoding Example

```cpp
#include "libbech32.h"

int main() {
    bech32::DecodedResult decodedResult = bech32::decode("hello1w0rldjn365x");

    // decodedResult.hrp == "hello"
    // decodedResult.dp[0] == 14
    // decodedResult.encoding == bech32::Encoding::Bech32m
}
```

For more C++ examples, see [examples/cpp_other_examples.cpp](examples/cpp_other_examples.cpp)

### C Encoding Example

```C
#include "libbech32.h"
#include <string.h>
#include <stdio.h>

int main() {
    // simple human readable part with some data
    char hrp[] = "hello";
    unsigned char dp[] = {14, 15, 3, 31, 13};

    // create storage for bech32 string
    bech32_bstring *bstring = bech32_create_bstring(strlen(hrp), sizeof(dp));
    if(!bstring) {
        printf("bech32 string can not be created");
        return E_BECH32_NO_MEMORY;
    }

    // encode
    bech32_error err = bech32_encode(bstring, hrp, dp, sizeof(dp));
    if(err != E_BECH32_SUCCESS) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_bstring(bstring);
        return err;
    }

    printf("bech32 encoding of human-readable part \'hello\' and data part \'[14, 15, 3, 31, 13]\' is:\n");
    printf("%s\n", bstring->string);
    // prints "hello1w0rldjn365x"
    // ... "hello" + Bech32.separator ("1") + encoded data ("w0rld") + 6 char checksum ("jn365x")

    // free memory
    bech32_free_bstring(bstring);

    return E_BECH32_SUCCESS;
}
```

### C Decoding Example

```C
#include "libbech32.h"
#include <string.h>
#include <stdio.h>

int main() {

    char str[] = "hello1w0rldjn365x";

    // create storage for decoded bech32 data
    bech32_DecodedResult * decodedResult = bech32_create_DecodedResult(str);
    if(!decodedResult) {
        printf("bech32 DecodedResult can not be created");
        return E_BECH32_NO_MEMORY;
    }

    // decode
    bech32_error err = bech32_decode(decodedResult, str);
    if(err != E_BECH32_SUCCESS) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_DecodedResult(decodedResult);
        return err;
    }

    // decodedResult->hrp == "hello"
    // decodedResult->dp[0] == 14
    // decodedResult->encoding == ENCODING_BECH32M

    // free memory
    bech32_free_DecodedResult(decodedResult);
}
```

For more C examples, see [examples/c_other_examples.c](examples/c_other_examples.c)


## Regarding bech32 checksums

The Bech32 data encoding format was first proposed by Pieter Wuille in early 2017 in
[BIP 0173](https://github.com/bitcoin/bips/blob/master/bip-0173.mediawiki). Later, in November 2019, Pieter published
some research that a constant used in the bech32 checksum algorithm (value = 1) may not be
optimal for the error detecting properties of bech32. In February 2021, Pieter published
[BIP 0350](https://github.com/bitcoin/bips/blob/master/bip-0350.mediawiki) reporting that "exhaustive analysis" showed the best possible constant value is
0x2bc830a3. This improved variant of Bech32 is called "Bech32m".

When decoding a possible bech32 encoded string, libbech32 returns an enum value showing whether bech32m or bech32
was used to encode. This can be seen in the examples above.

When encoding data, libbech32 defaults to using the new constant value of 0x2bc830a3. If the original constant value
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
    bech32_error err = bech32_encode_using_original_constant(bstring, hrp, dp, sizeof(dp));

    /// ... as above ...
```
