include(../cmake/FindBitcoinApiCpp.cmake)

find_package(CURL)


############################################################
# Target: txid2txref

add_executable(txid2txref
        txid2txref.h txid2txref.cpp
        t2tSupport.h t2tSupport.cpp
        bitcoinRPCFacade.h bitcoinRPCFacade.cpp)

target_compile_features(txid2txref PRIVATE cxx_std_11)
target_compile_options(txid2txref PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(txid2txref PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(txid2txref PUBLIC bech32 txref anyoption jsoncpp ${BITCOINAPICPP_LIBRARIES})

############################################################
# Target: createBtcrDid

add_executable(createBtcrDid
        createBtcrDid.cpp
        t2tSupport.h t2tSupport.cpp
        bitcoinRPCFacade.h bitcoinRPCFacade.cpp
        chainQuery.h chainQuery.cpp chainSoQuery.h chainSoQuery.cpp
        curlWrapper.h curlWrapper.cpp
        classifyInputString.h classifyInputString.cpp
        encodeOpReturnData.h encodeOpReturnData.cpp
        satoshis.h)

target_compile_features(createBtcrDid PRIVATE cxx_std_11)
target_compile_options(createBtcrDid PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(createBtcrDid PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(createBtcrDid PUBLIC bech32 txref anyoption jsoncpp ${BITCOINAPICPP_LIBRARIES} ${CURL_LIBRARIES})

############################################################
# Target: didResolver

add_executable(didResolver
        didResolver.cpp
        bitcoinRPCFacade.h bitcoinRPCFacade.cpp
        chainQuery.h chainQuery.cpp chainSoQuery.h chainSoQuery.cpp
        curlWrapper.h curlWrapper.cpp
        classifyInputString.h classifyInputString.cpp
        t2tSupport.h t2tSupport.cpp
        satoshis.h)

target_compile_features(didResolver PRIVATE cxx_std_11)
target_compile_options(didResolver PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(didResolver PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(didResolver PUBLIC bech32 txref anyoption jsoncpp ${BITCOINAPICPP_LIBRARIES} ${CURL_LIBRARIES})

############################################################
# Target: didVerifier

add_executable(didVerifier
        didVerifier.cpp
        bitcoinRPCFacade.h bitcoinRPCFacade.cpp
        classifyInputString.h classifyInputString.cpp
        t2tSupport.h t2tSupport.cpp
        satoshis.h)

target_compile_features(didVerifier PRIVATE cxx_std_11)
target_compile_options(didVerifier PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(didVerifier PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(didVerifier PUBLIC bech32 txref anyoption jsoncpp ${BITCOINAPICPP_LIBRARIES})


#install(TARGETS txid2txref createBtcrDid didResolver didVerifier DESTINATION bin)
