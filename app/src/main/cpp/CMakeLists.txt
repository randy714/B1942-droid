cmake_minimum_required(VERSION 3.18.1)
project("b1942_engine")

# Optimasi Performa Maksimal
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -funroll-loops")

add_library(b1942_engine SHARED
        refractor_engine.cpp
        bink_player.cpp
        gl_renderer.cpp)

find_library(log-lib log)
find_library(android-lib android)
find_library(gles-lib GLESv2)

target_link_libraries(b1942_engine
        ${log-lib}
        ${android-lib}
        ${gles-lib})
