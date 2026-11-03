cmake_minimum_required(VERSION 3.18.1)
project("b1942_engine")

# OPTIMALISASI KELAS BERAT: Anti Frame Drop, LTO, dan Multithreading
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -funroll-loops -flto -std=c++17 -pthread")

add_library(b1942_engine SHARED
        refractor_engine.cpp
        bink_player.cpp
        gl_renderer.cpp)

find_library(log-lib log)
find_library(android-lib android)
find_library(gles3-lib GLESv3)
find_library(vulkan-lib vulkan)

target_link_libraries(b1942_engine
        ${log-lib}
        ${android-lib}
        ${gles3-lib}
        ${vulkan-lib})
