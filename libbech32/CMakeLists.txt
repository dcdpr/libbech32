############################################################
# Target: bech32

add_library(bech32 STATIC bech32.cpp)

target_include_directories(bech32
    PUBLIC
        $<INSTALL_INTERFACE:include/libbech32>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/libbech32>
)

target_compile_features(bech32 PRIVATE cxx_std_11)
target_compile_options(bech32 PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(bech32 PROPERTIES CXX_EXTENSIONS OFF)

