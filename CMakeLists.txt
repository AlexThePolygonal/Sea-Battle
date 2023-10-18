cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 17)

set(CXXFLAGS  "-Werror -Wall -std=c++17 -O2")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}")

project(SeaBattle)

add_executable(app main.cpp)