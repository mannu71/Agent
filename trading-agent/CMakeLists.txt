cmake_minimum_required(VERSION 3.20)
project(trading_agent CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(ta_core
  src/csv.cpp
  src/indicators.cpp
  src/risk.cpp
  src/screener.cpp
  src/backtest.cpp
)
target_include_directories(ta_core PUBLIC include)

add_executable(ta_backtest src/main.cpp)
target_link_libraries(ta_backtest PRIVATE ta_core)

enable_testing()
add_executable(ta_tests tests/test_main.cpp)
target_link_libraries(ta_tests PRIVATE ta_core)
add_test(NAME ta_tests COMMAND ta_tests)
