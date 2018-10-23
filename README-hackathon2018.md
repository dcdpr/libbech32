# btcr-DID-method

# BTCR Virtual Hackathon, July 2018

As part of this hackathon we will attempt to build a prototype
didResolver. We are also trying to get a blocksci instance set up so we
can query it for useful info instead of going to a public blockchain
explorer.

Please be sure to see README.md for instructions on how to build the apps
in this repo.

# didResolver

The `didResolver` application will take a BTCR DID (ex:
`did:btcr:xpkm-90zy-zqqq-qcvh-xvx`) and use it to "resolve" the DID Document
(aka diddo).

It supports many of the same command line options as `createBtcrDid` for
connecting to a local or remote bitcoind.

# Running didResolver

You can ask didResolver for help and it will show all the runtime options
available:

```
$ ./src/didResolver --help
Usage: didResolver [options] <did>

 -h  --help                 Print this help
 --rpchost [rpchost or IP]  RPC host (default: 127.0.0.1)
 --rpcuser [user]           RPC user
 --rpcpassword [pass]       RPC password
 --rpcport [port]           RPC port (default: try both 8332 and 18332)
 --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf)

<did>                       the BTCR DID to resolve. Could be txref or txref-ext based
```

When given a DID, didResolver will decode the txref within, connect to
bitcoind to verify the transaction info, then it will resolve the diddo
that the DID represents. It will then dump to the diddo to std::cout.

non-complete example:
```
$ ./src/didResolver did:btcr:xpkm-90zy-zqqq-qcvh-xvx
Valid txref found:
  txref: txtest1-xkm9-0zsg-qqeg-q5qs
  block height: 1297339
  transaction position: 68
  txoIndex: 0
```
