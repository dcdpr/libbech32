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

### Installing prerequirements

If the above doesn't work, you probably need to install some
prerequirements. For example, on a fresh Debian Stretch system:

```
sudo apt-get install make gcc g++
```

It is worth getting the latest cmake, so install that the hard way:

```
wget https://cmake.org/files/v3.13/cmake-3.13.2.tar.gz
tar xzf cmake-3.13.2.tar.gz
cd cmake-3.13.2
./configure
make 
sudo make install
```

Now you can again try to build libbech32.

### C++ Usage Example

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

    // decode
    bech32::HrpAndDp hd = bech32::decode(bstr);

    assert(hrp == hd.hrp);
    assert(data == hd.dp);
    assert(bech32::Encoding::Bech32m == hd.encoding);
}
```

### C Usage Example

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

    // allocate memory for decoded data
    bech32_HrpAndDp * hrpdp = bech32_create_HrpAndDp(bstring->string);

    // decode
    assert(bech32_decode(hrpdp, bstring->string) == E_BECH32_SUCCESS);
    assert(strcmp(hrpdp->hrp, hrp) == 0);
    assert(hrpdp->dp[0] == dp[0]);
    assert(hrpdp->dp[4] == dp[4]);
    assert(ENCODING_BECH32M == hrpdp->encoding);

    // free memory
    bech32_free_HrpAndDp(hrpdp);
    bech32_free_bstring(bstring);
}
```
