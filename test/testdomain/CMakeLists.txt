include(../../cmake/FindBitcoinApiCpp.cmake)

add_executable(UnitTests_domain main.cpp test_Txid.cpp test_Vout.cpp test_BlockHeight.cpp test_TransactionPosition.cpp test_Txref.cpp test_Did.cpp ../mock_bitcoinRPCFacade.cpp ../mock_bitcoinRPCFacade.h)

target_compile_features(UnitTests_domain PRIVATE cxx_std_11)
target_compile_options(UnitTests_domain PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(UnitTests_domain PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(UnitTests_domain
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src/domain)

target_link_libraries(UnitTests_domain bech32 txref jsoncpp ${BITCOINAPICPP_LIBRARIES} gtest gmock rapidcheck_gtest)

add_test(NAME UnitTests_domain
         COMMAND UnitTests_domain)
