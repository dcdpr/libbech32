include(CTest)
include(../cmake/DownloadProject/DownloadProject.cmake)
include(../cmake/FindJSONCPP.cmake)
include(../cmake/FindBitcoinApiCpp.cmake)

find_package(curl)

# Turn off some warnings to silence issues coming from googletest code
set(DCD_CXX_FLAGS "${DCD_CXX_FLAGS} -Wno-global-constructors")

download_project(
    PROJ googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG master
    UPDATE_DISCONNECTED 1
)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

enable_testing()

include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/libbech32
        ${PROJECT_SOURCE_DIR}/libtxref
    )

file(GLOB SRCS *.cpp */test_*.cpp)

add_executable(txref_test ${SRCS})

target_link_libraries(txref_test ${project_library_target_name}
        ${REQUIRED_LIBRARIES} ${JSONCPP_LIBRARIES} ${BITCOINAPICPP_LIBRARIES} ${CURL_LIBRARIES} gtest gmock)

add_test(NAME all_tests
        COMMAND txref_test)

add_subdirectory(testbech32)
add_subdirectory(testtxref)

