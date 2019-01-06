include(CTest)

# Turn off some warnings to silence issues coming from googletest code
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(DCD_CXX_FLAGS ${DCD_CXX_FLAGS} -Wno-global-constructors)
endif()

if(LIBBECH32_BUILD_GOOGLETEST)
  option(INSTALL_GTEST "Enable installation of googletest." OFF)
  add_subdirectory("googletest")
endif()

# Turn off some warnings to silence issues coming from rapidcheck code
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(DCD_CXX_FLAGS ${DCD_CXX_FLAGS} -Wno-c++98-compat-pedantic)
endif()

if(LIBBECH32_BUILD_RAPIDCHECK)
  option(RC_ENABLE_GTEST "Build Google Test integration" ON)
  add_subdirectory("rapidcheck")
endif()

add_subdirectory(testbech32)
