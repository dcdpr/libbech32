include(../cmake/FindBitcoinApiCpp.cmake)
include(../cmake/FindJSONCPP.cmake)

file(GLOB SRCS *.cpp)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(txid2txref txid2txref.cpp BitcoinRPCFacade.cpp BitcoinRPCFacade.h)

target_link_libraries(txid2txref libtxref libbech32 anyoption ${JSONCPP_LIBRARIES} ${BITCOINAPICPP_LIBRARIES})

install(TARGETS txid2txref DESTINATION bin)
