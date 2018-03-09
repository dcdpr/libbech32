file(GLOB SRCS *.cpp)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${DCD_CXX_FLAGS})

add_library(libtxref STATIC ${SRCS})

# include directories exported by this library
target_include_directories(libtxref PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
