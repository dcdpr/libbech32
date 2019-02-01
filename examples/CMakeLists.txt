
add_executable(bech32_cpp_example cpp_example.cpp)

target_compile_features(bech32_cpp_example PRIVATE cxx_std_11)
target_compile_options(bech32_cpp_example PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(bech32_cpp_example PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(bech32_cpp_example bech32)


add_executable(bech32_c_example c_example.c)

target_compile_features(bech32_c_example PRIVATE c_std_99)
set_target_properties(bech32_c_example PROPERTIES C_EXTENSIONS OFF)

target_link_libraries(bech32_c_example bech32)
