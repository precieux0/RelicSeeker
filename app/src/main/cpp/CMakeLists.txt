cmake_minimum_required(VERSION 3.18)
project(RelicSeeker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

# IMPORTANT : sans ce flag, le linker (ld.lld) strippe ANativeActivity_onCreate
# car aucun appel C++ ne le référence -> NativeActivity ne trouve plus le
# point d'entrée et l'app crashe juste après le lancement.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# native_app_glue est fourni par le NDK.
set(NATIVE_APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${NATIVE_APP_GLUE_DIR})

add_library(native_app_glue STATIC
    ${NATIVE_APP_GLUE_DIR}/android_native_app_glue.c)

add_library(RelicSeeker SHARED
    main.cpp
    game.cpp
    renderer.cpp
    shader.cpp
    mesh.cpp
    camera.cpp
    input.cpp
    player.cpp
    enemy.cpp
    world.cpp
    story.cpp
    sound.cpp
    utils.cpp
    assets.cpp
    splash.cpp
    credits.cpp
    font.cpp
)

target_link_libraries(RelicSeeker
    native_app_glue
    GLESv3
    EGL
    android
    log
    OpenSLES
    z
)
