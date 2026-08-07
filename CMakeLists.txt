cmake_minimum_required(VERSION 4.0.0)
project(Templeano VERSION 0.0.1 LANGUAGES CXX)

# Enforce strict C++20 standard compliance
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(templeano)

option(Templeano_ENABLE_TESTING "Enable testing" ${PROJECT_IS_TOP_LEVEL})

if(Templeano_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
