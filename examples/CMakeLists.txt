
add_executable(cpp_example cpp_example.cpp)

target_compile_features(cpp_example PRIVATE cxx_std_11)
target_compile_options(cpp_example PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(cpp_example PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(cpp_example bech32)


add_executable(c_example c_example.c)

target_compile_features(c_example PRIVATE c_std_99)
set_target_properties(c_example PROPERTIES C_EXTENSIONS OFF)

target_link_libraries(c_example bech32)
