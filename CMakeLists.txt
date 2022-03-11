# CMake for Bra's timer code
cmake_minimum_required(VERSION 3.10)
project(Timer VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)

# Include googletest framework
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)


include_directories(include)
add_subdirectory(src)
add_subdirectory(gtest)
