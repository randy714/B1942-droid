cmake_minimum_required(VERSION 3.18.1)
project("b1942_engine")

# Optimasi Performa Maksimal: Anti-Lag, Fast-Math, & Multithreading
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -funroll-loops -std=c++17 -pthread")

add_library(b1942_engine SHARED
        refractor_engine.cpp
        bink_player.cpp
        gl_renderer.cpp)

find_library(log-lib log)
find_library(android-lib android)
find_library(gles-lib GLESv2) # Library Universal OpenGL ES 2.0 - 3.2

target_link_libraries(b1942_engine
        ${log-lib}
        ${android-lib}
        ${gles-lib})
