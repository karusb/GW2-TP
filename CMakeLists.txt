# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.12)

project(gw2tp VERSION 1.0.0 DESCRIPTION "gw2tp")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

SET(DOWNLOAD_EXTRACT_TIMESTAMP ON)

include(FetchContent)
FetchContent_Declare(
  BazPO
  GIT_REPOSITORY https://github.com/karusb/BazPO.git
  GIT_TAG        v0.2.1
)

FetchContent_MakeAvailable(BazPO)

include(FetchContent)
FetchContent_Declare(
  CurlWrapper
  GIT_REPOSITORY https://github.com/karusb/CurlWrapper.git
  GIT_TAG        59814511b63330ae8b10da193a24fbb8d1eb6f34
)

FetchContent_MakeAvailable(CurlWrapper)

FetchContent_Declare(
  termcolor
  GIT_REPOSITORY https://github.com/ikalnytskyi/termcolor.git
  GIT_TAG        v2.1.0
)

FetchContent_MakeAvailable(termcolor)

## add_subdirectory(${termcolor_SOURCE_DIR} ${termcolor_BINARY_DIR} EXCLUDE_FROM_ALL)

add_executable (gw2tp
 "Main.cpp"
 "items/App.cpp"
 "items/App.hpp"
 "items/Api.cpp"
 "items/Api.hpp"
 "items/Item.cpp"
 "items/Item.hpp"
 "items/Database.cpp"
 "items/Database.hpp"
 "items/JsonHelper.cpp"
 "items/JsonHelper.hpp"
 "items/Menu.cpp"
 "items/Menu.hpp"
)

target_include_directories(gw2tp PRIVATE)
set_property(TARGET gw2tp PROPERTY CXX_STANDARD 17)
target_link_libraries(
  gw2tp
  BazPO
  curlwrapper
  termcolor
)

install(TARGETS gw2tp)
