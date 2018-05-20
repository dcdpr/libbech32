include(../cmake/FindBitcoinApiCpp.cmake)
include(../cmake/FindJSONCPP.cmake)

find_package(curl)

file(GLOB SRCS *.cpp)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(txid2txref txid2txref.cpp t2tSupport.cpp t2tSupport.h bitcoinRPCFacade.cpp bitcoinRPCFacade.h txid2txref.h)
target_link_libraries(txid2txref libtxref libbech32 anyoption ${JSONCPP_LIBRARIES} ${BITCOINAPICPP_LIBRARIES})

add_executable(createBtcrDid createBtcrDid.cpp t2tSupport.cpp t2tSupport.h bitcoinRPCFacade.cpp bitcoinRPCFacade.h chainQuery.h chainSoQuery.cpp chainSoQuery.h CurlWrapper.cpp CurlWrapper.h encodeOpReturnData.h satoshis.h classifyInputString.h)
target_link_libraries(createBtcrDid libtxref libbech32 anyoption ${JSONCPP_LIBRARIES} ${BITCOINAPICPP_LIBRARIES} ${CURL_LIBRARIES})

install(TARGETS txid2txref createBtcrDid DESTINATION bin)
