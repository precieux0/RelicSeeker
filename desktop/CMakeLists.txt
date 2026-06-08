cmake_minimum_required(VERSION 3.18)
project(RelicSeekerDesktop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/../app/src/main/cpp
)

add_executable(RelicSeekerDesktop
    main.cpp
    renderer_desktop.cpp
    ../app/src/main/cpp/font.cpp
    ../app/src/main/cpp/splash.cpp
    ../app/src/main/cpp/menu.cpp
    ../app/src/main/cpp/utils.cpp
)

target_compile_definitions(RelicSeekerDesktop PRIVATE DESKTOP_BUILD)

target_link_libraries(RelicSeekerDesktop
    GLEW::GLEW
    glfw
    OpenGL::GL
)
