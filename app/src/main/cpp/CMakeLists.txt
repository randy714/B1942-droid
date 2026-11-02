cmake_minimum_required(VERSION 3.22.1)
project("b1942_engine")

add_library(
    b1942_engine
    SHARED
    engine_main.cpp
    rfa_loader.cpp
    gl_renderer.cpp
)

find_library(log-lib log)
find_library(gles2-lib GLESv2)
find_library(egl-lib EGL)
find_library(android-lib android)

target_link_libraries(
    b1942_engine
    ${log-lib}
    ${gles2-lib}
    ${egl-lib}
    ${android-lib}
)
