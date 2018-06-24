include(../../cmake/FindBitcoinApiCpp.cmake)
include(../../cmake/FindJSONCPP.cmake)

file(GLOB SRCS *.cpp)

include_directories(${CMAKE_SOURCE_DIR} ${BITCOINAPICPP_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS})

add_executable(testtxref ${SRCS})

target_link_libraries(testtxref libtxref libbech32 gtest gmock rapidcheck_gtest)

add_test(NAME testtxref
         COMMAND testtxref)
