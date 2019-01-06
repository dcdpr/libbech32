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
$ sudo apt-get install make gcc g++
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

