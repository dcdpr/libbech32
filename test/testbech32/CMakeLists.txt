
add_executable(testbech32 main.cpp test_Bech32.cpp)

target_compile_features(testbech32 PRIVATE cxx_std_11)
target_compile_options(testbech32 PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(testbech32 PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(testbech32
    PUBLIC
        ${PROJECT_SOURCE_DIR}/libbech32)

target_link_libraries(testbech32 bech32 gtest gmock rapidcheck_gtest)

add_test(NAME testbech32
         COMMAND testbech32)
