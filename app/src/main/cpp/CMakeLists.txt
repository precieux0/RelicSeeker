cmake_minimum_required(VERSION 3.18)
project(RelicSeeker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")

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
    # Chemin absolu vers native_app_glue (valable sur GitHub Actions)
    /usr/local/lib/android/sdk/ndk/25.2.9519653/sources/android/native_app_glue/android_native_app_glue.c
)

target_link_libraries(RelicSeeker
    GLESv3
    EGL
    android
    log
    OpenSLES
    z
)
