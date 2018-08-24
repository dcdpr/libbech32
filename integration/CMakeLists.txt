include(CTest)
find_package(CURL)

# Almost all of these integration tests require an existing
# ~/.bitcoin/bitcoin.conf with rpc* parameters that point to a
# running testnet bitcoind. See top-level README.md for details.

############################################################
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


############################################################
# Integration tests for txid2txref

# no arguments
add_test(NAME "IntegrationTests_t2t_Is_Runnable "
    COMMAND $<TARGET_FILE:txid2txref>)

# ask for help
add_test(NAME "IntegrationTests_t2t_Is_Runnable-h"
    COMMAND $<TARGET_FILE:txid2txref> -h)

# get txref from a known txid.

add_test(NAME "IntegrationTests_t2t_txref_from_known_txid"
    COMMAND $<TARGET_FILE:txid2txref> f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107)
set_tests_properties("IntegrationTests_t2t_txref_from_known_txid" PROPERTIES
        PASS_REGULAR_EXPRESSION "txtest1-xyv2-xzyq-qqm5-tyke")

# get txrefext from a known txid.

add_test(NAME "IntegrationTests_t2t_txrefext_from_known_txid_index_0"
    COMMAND $<TARGET_FILE:txid2txref> --txoIndex 0 f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107)
set_tests_properties("IntegrationTests_t2t_txrefext_from_known_txid_index_0" PROPERTIES
        PASS_REGULAR_EXPRESSION "txtest1-xyv2-xzyq-qqqq-qc5x2nf")

add_test(NAME "IntegrationTests_t2t_txrefext_from_known_txid_index_1"
    COMMAND $<TARGET_FILE:txid2txref> --txoIndex 1 f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107)
set_tests_properties("IntegrationTests_t2t_txrefext_from_known_txid_index_1" PROPERTIES
        PASS_REGULAR_EXPRESSION "txtest1-xyv2-xzyq-qqpq-q6k0a23")

# get txid from txref.

add_test(NAME "IntegrationTests_t2t_txid_from_txrefext"
    COMMAND $<TARGET_FILE:txid2txref> txtest1-xyv2-xzyq-qqpq-q6k0a23)
set_tests_properties("IntegrationTests_t2t_txid_from_txrefext" PROPERTIES
        PASS_REGULAR_EXPRESSION "f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107")


############################################################
# Integration tests for createBtcrDid

# TODO: Not sure yet how to make an integration test for createBtcrDid,
# since it requires new source and destination BTC addresses each
# time... Maybe we need to do something with a bitcoind using regtest.

# no arguments
add_test(NAME "IntegrationTests_cbd_Is_Runnable "
    COMMAND $<TARGET_FILE:createBtcrDid>)

# ask for help
add_test(NAME "IntegrationTests_cbd_Is_Runnable-h"
    COMMAND $<TARGET_FILE:createBtcrDid> -h)


############################################################
# Integration tests for didResolver

# no arguments
add_test(NAME "IntegrationTests_didResolver_Is_Runnable "
        COMMAND $<TARGET_FILE:didResolver>)

# ask for help
add_test(NAME "IntegrationTests_didResolver_Is_Runnable-h"
        COMMAND $<TARGET_FILE:didResolver> -h)

# DID with zero updates/spends:
#
# did:btcr:txtest1-xxvh-jzg2-9qpq-qsepxw9 encodes txid
# f3b955b2db510ba2f60446de51a9e4bcfb96a0e2ad8154d39bfefaffb305e6b0,
# output index 1. This is unspent.
add_test(NAME "IntegrationTests_didResolver_followTip_zero_spends"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:txtest1-xxvh-jzg2-9qpq-qsepxw9)

set_tests_properties("IntegrationTests_didResolver_followTip_zero_spends" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: f3b955b2db510ba2f60446de51a9e4bcfb96a0e2ad8154d39bfefaffb305e6b0")

# DID with one update/spend:
#
# did:btcr:txtest1-xsvh-jzve-rqpq-q343ajh encodes txid
# 2e71684bba5c953c09c9f2363233af9938af4fe05d2b22df3df1c3dabdcc8200,
# output index 1. This is spent to create new DID
# did:btcr:txtest1-xjvh-jzcq-8qpq-qzauu62. That encodes txid
# 604a0cf5c5b794ca8b90a817b0624d5e6f1fab1ff4675f72b7a31679945ebdd9,
# output index 1. This is unspent. Both DIDs should have the same "tip" (604a...)
add_test(NAME "IntegrationTests_didResolver_followTip_one_spend_p1"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:txtest1-xsvh-jzve-rqpq-q343ajh)
set_tests_properties("IntegrationTests_didResolver_followTip_one_spend_p1" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: 604a0cf5c5b794ca8b90a817b0624d5e6f1fab1ff4675f72b7a31679945ebdd9")

add_test(NAME "IntegrationTests_didResolver_followTip_one_spend_p2"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:txtest1-xjvh-jzcq-8qpq-qzauu62)
set_tests_properties("IntegrationTests_didResolver_followTip_one_spend_p2" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: 604a0cf5c5b794ca8b90a817b0624d5e6f1fab1ff4675f72b7a31679945ebdd9")

# DID with two updates/spends:
#
# did:btcr:txtest1-xz4h-jzcl-rqpq-qjqxf09 encodes txid
# cb0252c5ea4e24bee19edd1ed1338ef077dc75d30383097d8c4bae3a9862b35a,
# output index 1. This is spent to create new DID
# did:btcr:txtest1-xx4h-jzsk-zqpq-ql05ak5. That encodes txid
# 3289a36a8ca15017269a6b6e3977732fc21c8106d85020d5493ee9f5c8e00958,
# output index 1. This is spent to create new DID
# did:btcr:txtest1-xg4h-jzgp-pqpq-q9r6sd9. That encodes txid
# 8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5,
# output index 1. This is unspent. All three DIDs should have the
# same "tip" (8a76...)
add_test(NAME "IntegrationTests_didResolver_followTip_two_spends_p1"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:txtest1-xz4h-jzcl-rqpq-qjqxf09)
set_tests_properties("IntegrationTests_didResolver_followTip_two_spends_p1" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: 8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5")

add_test(NAME "IntegrationTests_didResolver_followTip_two_spends_p2"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:txtest1-xx4h-jzsk-zqpq-ql05ak5)
set_tests_properties("IntegrationTests_didResolver_followTip_two_spends_p2" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: 8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5")

add_test(NAME "IntegrationTests_didResolver_followTip_two_spends_p3"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:txtest1-xg4h-jzgp-pqpq-q9r6sd9)
set_tests_properties("IntegrationTests_didResolver_followTip_two_spends_p3" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: 8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5")


############################################################
# Other small integration tests

add_executable(IntegrationTests_chainSoQuery itest_chainSoQuery.cpp ../src/chainQuery.cpp ../src/chainSoQuery.cpp ../src/curlWrapper.cpp)

target_compile_features(IntegrationTests_chainSoQuery PRIVATE cxx_std_11)
target_compile_options(IntegrationTests_chainSoQuery PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(IntegrationTests_chainSoQuery PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(IntegrationTests_chainSoQuery
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(IntegrationTests_chainSoQuery
        PUBLIC
        jsoncpp ${CURL_LIBRARIES})

add_test(NAME "IntegrationTests_chainSoQuery"
        COMMAND $<TARGET_FILE:IntegrationTests_chainSoQuery>)
