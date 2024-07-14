# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.12)

project(GW2-TP VERSION 1.0.0 DESCRIPTION "GW2-TP")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

SET(requiredlibs)
SET(DOWNLOAD_EXTRACT_TIMESTAMP ON)

include(ExternalProject)
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

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
set_property(TARGET gtest PROPERTY CXX_STANDARD 17)

include(GoogleTest)
include(CTest)
enable_testing()

set(GW2-TPPath ${CMAKE_CURRENT_LIST_DIR})

add_executable (GW2-TP
 "GW2TPAPI.cpp"
 "items/Item.cpp"
 "items/Item.hpp"
 "items/Database.cpp"
 "items/Database.hpp"
 "items/JsonHelper.cpp"
 "items/JsonHelper.hpp"
)

target_include_directories(GW2-TP PRIVATE)
set_property(TARGET GW2-TP PROPERTY CXX_STANDARD 17)
target_link_libraries(
  GW2-TP
  BazPO
  curlwrapper
)


install(TARGETS GW2-TP)
