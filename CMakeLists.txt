cmake_minimum_required(VERSION 3.14)

project(BBX_AUDIO VERSION 0.1.0 LANGUAGES CXX)

add_library(bbx_audio STATIC
    src/sample.cpp
)

set_target_properties(bbx_audio PROPERTIES PUBLIC
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED
)

# Include directories
target_include_directories(bbx_audio PUBLIC src)
