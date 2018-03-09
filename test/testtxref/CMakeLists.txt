file(GLOB SRCS *.cpp)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${DCD_CXX_FLAGS})

add_executable(testtxref ${SRCS})

target_link_libraries(testtxref libtxref libbech32 gtest gmock)

add_test(NAME testtxref
         COMMAND testtxref)
