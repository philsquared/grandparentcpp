cmake_minimum_required(VERSION 3.14)
project(GrandParent)

set(CMAKE_CXX_STANDARD 17)

add_executable(GrandParent main.cpp vector-int-string.cpp memory.cpp constexpr.cpp string_conversions.cpp multiple_returns.cpp printer.cpp)