include(CTest)
include(../cmake/DownloadProject/DownloadProject.cmake)
include(../cmake/FindBitcoinApiCpp.cmake)

find_package(CURL)

# Turn off some warnings to silence issues coming from googletest code
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(DCD_CXX_FLAGS ${DCD_CXX_FLAGS} -Wno-global-constructors)
endif()

download_project(
    PROJ googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG master
    UPDATE_DISCONNECTED 1
)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})


# Turn off some warnings to silence issues coming from rapidcheck code
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(DCD_CXX_FLAGS ${DCD_CXX_FLAGS} -Wno-c++98-compat-pedantic)
endif()

option(RC_ENABLE_GTEST "Build Google Test integration" ON)
add_subdirectory("rapidcheck")



############################################################
# Target: txref_test

add_executable(UnitTests_src main.cpp test_chainSoQuery.cpp test_classifyInputString.cpp test_encodeOpReturnData.cpp test_satoshis.cpp jsonTestData.h)

target_compile_features(UnitTests_src PRIVATE cxx_std_11)
target_compile_options(UnitTests_src PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(UnitTests_src PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(UnitTests_src
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/libbech32
        ${PROJECT_SOURCE_DIR}/libtxref)

target_link_libraries(UnitTests_src
    PUBLIC
        txref bech32 jsoncpp ${BITCOINAPICPP_LIBRARIES} ${CURL_LIBRARIES} gtest gmock rapidcheck_gtest)

# CTest targets

add_test(NAME UnitTests_src
        COMMAND UnitTests_src)

add_subdirectory(testbech32)
add_subdirectory(testtxref)

