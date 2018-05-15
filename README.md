# btcr-DID-method

To support the development of our DID method, we first needed to create a
command-line application that would generate a txref from a txid.

## txid2txref

The txid2txref application is based on the txid2bech32 utility created by XXX
during the 2017 BTCR Virtual Hackathon.

txid2txref is run as `txid2txref [txid]` and given a valid bitcoin
transaction id (aka `txid`) will output a txref along with some other
info. txid2txref will communicate with an instance of bitcoind via RPC
to retrieve information about that transaction so it can create the `txref`.

## Building txid2txref

This README assumes that you have bitcoind running somewhere, locally
or remotely. bitcoind needs to be a full node--either on mainnet or
testnet--and needs to have `—txindex` and RPC turned on.

We have developed and tested txid2txref on MacOS, Debian and Ubuntu-based
systems. The following instructions should work with either Debian jessie
or stretch. Building and running txid2txref on different systems should
be pretty similar.

### Install some pre-requirements

First you need to install a basic C++ setup and several dependent
packages. Example:
```
$ sudo apt-get install make cmake gcc g++ libcurl4-openssl-dev libjsoncpp-dev libjansson-dev uuid-dev libjsonrpccpp-dev libjsonrpccpp-tools libboost-dev
$ sudo apt-get install curl wget unzip git
```

### Build and Install libbitcoinrpc

Next you need to build and install `libbitcoinrpc`. (The instructions
below on installing `libbitcoinrpc` are adapted from a draft tutorial: [Learning-Bitcoin-from-the-Command-Line](https://github.com/ChristopherA/Learning-Bitcoin-from-the-Command-Line/) from section [12.2_Accessing Bitcoind with C](https://github.com/ChristopherA/Learning-Bitcoin-from-the-Command-Line/blob/master/12_2_Accessing_Bitcoind_with_C.md).

Download [libbitcoinrpc from Github](https://github.com/gitmarek/libbitcoinrpc/). Clone it or grab a zip file, as you prefer.

```
$ git clone https://github.com/gitmarek/libbitcoinrpc.git
$ # or
$ wget https://github.com/gitmarek/libbitcoinrpc/archive/master.zip && unzip master.zip && rm master.zip
```

Before you can compile and install the package, you'll probably need to
adjust your `$PATH`, so that you can access `/sbin/ldconfig`:
```
$ export PATH="/sbin:$PATH"
```

For a debian or Ubuntu system, you'll want to adjust the `INSTALL_HEADERPATH`
and `INSTALL_LIBPATH` in the `libbitcoinrpc` `Makefile` to install to
`/usr` instead of `/usr/lib`:
```
$ cd libbitcoinrpc-master
$ vi Makefile
# edit the following two lines:
INSTALL_LIBPATH    := $(INSTALL_PREFIX)/usr/lib
INSTALL_HEADERPATH := $(INSTALL_PREFIX)/usr/include
```
(If you prefer not to sully your `/usr/lib`, the alternative is to
change your `/etc/ld.so.conf` or its dependent files appropriately ...
but for a test setup on a test machine, this is probably fine.)

Then you can compile:
```
$ make
```
If that works, you can install the package:
```
$ sudo make install
```

### Build and install libbitcoin-api-cpp

Download [libbitcoin-api-cpp from Github](https://github.com/minium/bitcoin-api-cpp/). Clone it or grab a zip file, as you prefer.

```
$ git clone https://github.com/minium/bitcoin-api-cpp.git
$ # or
$ wget https://github.com/minium/bitcoin-api-cpp/archive/master.zip && unzip master.zip && rm master.zip
```

This is a cmake-based project, so the standard process can be done here:

```
cd bitcoin-api-cpp-master
mkdir build
cd build
cmake ..
make
sudo make install
```

### Build txid2txref

If you don't already have it, download [btcr-DID-method from Github](https://github.com/dcdpr/btcr-DID-method/). Clone it or grab a zip file, as you prefer.

```
$ git clone git@github.com:dcdpr/btcr-DID-method.git
$ # or
$ wget https://github.com/dcdpr/btcr-DID-method/archive/master.zip && unzip master.zip && rm master.zip
```

This is a cmake-based project, so the standard process can be done here:

```
cd btcr-DID-method-master
mkdir build
cd build
cmake ..
make
```

If all goes well, you should be able to run txid2txref. You can either
supply RPC credentials on the command line, or txid2txref will extract
them from the .bitcoin/bitcoin.conf file in your home directory:

```
$ ./src/txid2txref --help
Usage: txid2txref [options] <txid>

 -h  --help                 Print this help
 --rpchost [rpchost or IP]  RPC host (default: 127.0.0.1)
 --rpcuser [user]           RPC user
 --rpcpassword [pass]       RPC password
 --rpcport [port]           RPC port (default: try both 8332 and 18332)
 --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf)

$ # specify all credentials
$ ./src/txid2txref --rpcuser bitcoinrpc --rpcpassword super-secret-passwd --rpchost 127.0.0.1 --rpcport 18333 f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107
txid: f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107
txref: txtest1-xyv2-xzyq-qqm5-tyke

$ # no credentials...will look in <homedir>/.bitcoin/bitcoin.conf
$ ./src/txid2txref f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107
txid: f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107
txref: txtest1-xyv2-xzyq-qqm5-tyke

$ # use different config file
$ ./src/txid2txref --config /tmp/bitcoin.conf f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107
txid: f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107
txref: txtest1-xyv2-xzyq-qqm5-tyke
```
