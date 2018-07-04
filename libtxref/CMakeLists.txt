############################################################
# Target: txref

add_library(txref STATIC txref.cpp)

target_include_directories(txref
    PUBLIC 
        $<INSTALL_INTERFACE:include/libtxref>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/libtxref>
)

target_compile_features(txref PRIVATE cxx_std_11)
target_compile_options(txref PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(txref PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(txref PUBLIC bech32)
