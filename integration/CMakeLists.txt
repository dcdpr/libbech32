include(CTest)

# Convenience targets for testing

add_custom_target(check
    COMMAND echo [----] Running tests
    USES_TERMINAL
    COMMAND ${CMAKE_CTEST_COMMAND}  --output-on-failure -C Debug
    DEPENDS txid2txref createBtcrDid)

add_custom_target(checkVerbose
    COMMAND echo [----] Running tests
    USES_TERMINAL
    COMMAND ${CMAKE_CTEST_COMMAND}  -V --output-on-failure -C Debug
    DEPENDS txid2txref createBtcrDid)


# Basic runnable tests

# no arguments
add_test(NAME "t2t_Is_Runnable "
    COMMAND $<TARGET_FILE:txid2txref>)

# ask for help
add_test(NAME "t2t_Is_Runnable-h"
    COMMAND $<TARGET_FILE:txid2txref> -h)

# get txref from a known txid. Requires an existing
# ~/.bitcoin/bitcoin.conf with rpc* parameters that points to a running
# bitcoind. See top-level README.md for details.

add_test(NAME "t2t_txref_from_known_txid"
    COMMAND $<TARGET_FILE:txid2txref>
    f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107 |
    grep -q txtest1-xyv2-xzyq-qqm5-tyke)

# get txrefext from a known txid. Requires an existing
# ~/.bitcoin/bitcoin.conf with rpc* parameters that points to a running
# bitcoind. See top-level README.md for details.

add_test(NAME "t2t_txrefext_from_known_txid_index_0"
    COMMAND $<TARGET_FILE:txid2txref> --txoIndex 0
    f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107 |
    grep -q txtest1-xyv2-xzyq-qqqq-qc5x2nf)

add_test(NAME "t2t_txrefext_from_known_txid_index_1"
    COMMAND $<TARGET_FILE:txid2txref> --txoIndex 1
    f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107 |
    grep -q txtest1-xyv2-xzyq-qqpq-q6k0a23)

# get txid from txref. Requires an existing
# ~/.bitcoin/bitcoin.conf with rpc* parameters that points to a running
# bitcoind. See top-level README.md for details.

add_test(NAME "t2t_txid_from_txrefext"
    COMMAND $<TARGET_FILE:txid2txref>
    txtest1-xyv2-xzyq-qqpq-q6k0a23 |
    grep -q f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107)


# TODO: Not sure yet how to make an integration test for createBtcrDid,
# since it requires new source and destination BTC addresses each
# time... Maybe I need to create a testnet txn with 100 BTC so I can pay
# out small amounts to make new test DIDs over and over.

# no arguments
add_test(NAME "cbd_Is_Runnable "
    COMMAND $<TARGET_FILE:createBtcrDid>)

# ask for help
add_test(NAME "cbd_Is_Runnable-h"
    COMMAND $<TARGET_FILE:createBtcrDid> -h)

