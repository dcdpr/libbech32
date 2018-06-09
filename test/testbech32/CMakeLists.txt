file(GLOB SRCS *.cpp)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${DCD_CXX_FLAGS})

add_executable(testbech32 ${SRCS})

target_link_libraries(testbech32 libbech32 gtest gmock rapidcheck_gtest)

add_test(NAME testbech32
         COMMAND testbech32)
