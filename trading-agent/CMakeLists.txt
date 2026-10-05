cmake_minimum_required(VERSION 3.20)
project(trading_agent CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(OpenSSL REQUIRED)  # SHA-256 for the hash-chained journal

file(GLOB TA_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(FILTER TA_SOURCES EXCLUDE REGEX ".*/src/(main|paper_main|ingest_main)\\.cpp$")

add_library(ta_core ${TA_SOURCES})
target_include_directories(ta_core PUBLIC include)
target_link_libraries(ta_core PUBLIC OpenSSL::Crypto)

add_executable(ta_backtest src/main.cpp)
target_link_libraries(ta_backtest PRIVATE ta_core)
add_executable(ta_paper src/paper_main.cpp)
target_link_libraries(ta_paper PRIVATE ta_core)
add_executable(ta_ingest src/ingest_main.cpp)
target_link_libraries(ta_ingest PRIVATE ta_core)

enable_testing()
file(GLOB TA_TESTS CONFIGURE_DEPENDS tests/*.cpp)
add_executable(ta_tests ${TA_TESTS})
target_link_libraries(ta_tests PRIVATE ta_core)
add_test(NAME ta_tests COMMAND ta_tests)
