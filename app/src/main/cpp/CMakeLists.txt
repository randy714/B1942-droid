cmake_minimum_required(VERSION 3.18.1)
project("b1942_engine")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -funroll-loops -pthread")

add_library(
    b1942_engine
    SHARED
    refractor_engine.cpp
    bink_player.cpp
    gl_renderer.cpp
)

find_library(glesv2-lib GLESv2)
find_library(log-lib log)
find_library(android-lib android)

target_link_libraries(
    b1942_engine
    ${glesv2-lib}
    ${log-lib}
    ${android-lib}
    z
)
