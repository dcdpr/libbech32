include(../../cmake/FindBitcoinApiCpp.cmake)

add_executable(testtxref main.cpp test_Txref.cpp test_Txref_api.cpp)

target_compile_features(testtxref PRIVATE cxx_std_11)
target_compile_options(testtxref PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(testtxref PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(testtxref 
    PUBLIC 
        ${CMAKE_SOURCE_DIR} 
        ${PROJECT_SOURCE_DIR}/libtxref
        ${BITCOINAPICPP_INCLUDE_DIRS})

target_link_libraries(testtxref PUBLIC txref bech32 gtest gmock rapidcheck_gtest)

add_test(NAME testtxref
         COMMAND testtxref)
