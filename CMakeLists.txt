cmake_minimum_required(VERSION 3.14)

project(FLORA VERSION 0.1.0 LANGUAGES CXX)

add_library(flora STATIC
    src/flora.h
    src/sample.cpp
)

set_target_properties(flora PROPERTIES PUBLIC
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED
)

target_include_directories(flora PUBLIC src)
