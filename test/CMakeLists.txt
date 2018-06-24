include(CTest)
include(../cmake/DownloadProject/DownloadProject.cmake)
include(../cmake/FindJSONCPP.cmake)
include(../cmake/FindBitcoinApiCpp.cmake)

find_package(CURL)

# Turn off some warnings to silence issues coming from googletest code
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(DCD_CXX_FLAGS "${DCD_CXX_FLAGS} -Wno-global-constructors")
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
  set(DCD_CXX_FLAGS "${DCD_CXX_FLAGS} -Wno-c++98-compat-pedantic")
endif()


option(RC_ENABLE_GTEST "Build Google Test integration" ON)
add_subdirectory("rapidcheck")

enable_testing()

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/libbech32
        ${PROJECT_SOURCE_DIR}/libtxref
        ${BITCOINAPICPP_INCLUDE_DIRS}
        ${JSONCPP_INCLUDE_DIRS}
    )

file(GLOB SRCS *.cpp */test_*.cpp)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${DCD_CXX_FLAGS})

add_executable(txref_test ${SRCS})

target_link_libraries(txref_test ${project_library_target_name}
        ${REQUIRED_LIBRARIES} ${JSONCPP_LIBRARIES} ${BITCOINAPICPP_LIBRARIES} ${CURL_LIBRARIES} gtest gmock rapidcheck_gtest)

add_test(NAME all_tests
        COMMAND txref_test)

add_subdirectory(testbech32)
add_subdirectory(testtxref)

