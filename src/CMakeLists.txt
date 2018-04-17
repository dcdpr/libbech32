include(../cmake/FindBitcoinApiCpp.cmake)
include(../cmake/FindJSONCPP.cmake)

find_package(curl)

file(GLOB SRCS *.cpp)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(txid2txref txid2txref.cpp BitcoinRPCFacade.cpp BitcoinRPCFacade.h)
target_link_libraries(txid2txref libtxref libbech32 anyoption ${JSONCPP_LIBRARIES} ${BITCOINAPICPP_LIBRARIES})

add_executable(createBtcrDid createBtcrDid.cpp BitcoinRPCFacade.cpp BitcoinRPCFacade.h ChainQuery.h ChainSoQuery.cpp ChainSoQuery.h CurlWrapper.cpp CurlWrapper.h encodeOpReturnData.h satoshis.h)
target_link_libraries(createBtcrDid libtxref libbech32 anyoption ${JSONCPP_LIBRARIES} ${BITCOINAPICPP_LIBRARIES} ${CURL_LIBRARIES})

install(TARGETS txid2txref DESTINATION bin)
