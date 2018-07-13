include(../cmake/FindBitcoinApiCpp.cmake)
include(../cmake/FindJSONCPP.cmake)

find_package(CURL)


############################################################
# Target: txid2txref

add_executable(txid2txref
        txid2txref.h txid2txref.cpp
        t2tSupport.h t2tSupport.cpp
        bitcoinRPCFacade.h bitcoinRPCFacade.cpp)

target_include_directories(txid2txref
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${BITCOINAPICPP_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS})

target_compile_features(txid2txref PRIVATE cxx_std_11)
target_compile_options(txid2txref PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(txid2txref PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(txid2txref PUBLIC bech32 txref anyoption ${JSONCPP_LIBRARIES} ${BITCOINAPICPP_LIBRARIES})

############################################################
# Target: createBtcrDid

add_executable(createBtcrDid
        createBtcrDid.cpp
        t2tSupport.h t2tSupport.cpp
        bitcoinRPCFacade.h bitcoinRPCFacade.cpp
        chainQuery.h chainSoQuery.h chainSoQuery.cpp
        CurlWrapper.h CurlWrapper.cpp
        classifyInputString.h classifyInputString.cpp
        encodeOpReturnData.h encodeOpReturnData.cpp)

target_compile_features(createBtcrDid PRIVATE cxx_std_11)
target_compile_options(createBtcrDid PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(createBtcrDid PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(createBtcrDid PUBLIC bech32 txref anyoption ${JSONCPP_LIBRARIES} ${BITCOINAPICPP_LIBRARIES} ${CURL_LIBRARIES})


#install(TARGETS txid2txref createBtcrDid DESTINATION bin)
