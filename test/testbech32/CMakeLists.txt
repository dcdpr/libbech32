
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
