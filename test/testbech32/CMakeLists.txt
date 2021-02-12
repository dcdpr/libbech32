
add_executable(UnitTests_bech32 main.cpp test_Bech32.cpp)

target_compile_features(UnitTests_bech32 PRIVATE cxx_std_11)
target_compile_options(UnitTests_bech32 PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(UnitTests_bech32 PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(UnitTests_bech32
    PUBLIC
        ${PROJECT_SOURCE_DIR}/libbech32)

target_link_libraries(UnitTests_bech32 bech32 gtest gmock rapidcheck_gtest)

add_test(NAME UnitTests_bech32
         COMMAND UnitTests_bech32)


add_executable(bech32_c_api_tests
        bech32_c_api_tests.c
        )

target_compile_features(bech32_c_api_tests PRIVATE c_std_99)
target_compile_options(bech32_c_api_tests PRIVATE "-fPIC")
set_target_properties(bech32_c_api_tests PROPERTIES C_EXTENSIONS OFF)

target_link_libraries(bech32_c_api_tests PUBLIC bech32)

add_test(NAME UnitTests_C_api_bech32
        COMMAND bech32_c_api_tests)


add_executable(bech32_api_tests
        bech32_api_tests.cpp
        )

target_compile_features(bech32_api_tests PRIVATE cxx_std_11)
target_compile_options(bech32_api_tests PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(bech32_api_tests PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(bech32_api_tests PUBLIC bech32)

add_test(NAME UnitTests_api_bech32
        COMMAND bech32_api_tests)
