cmake_minimum_required(VERSION 3.18)
project(RelicSeeker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")

# Trouver GLM (inclus via les sources, ou télécharger)
# Pour simplifier, on inclut GLM en tant que sous-dossier (vous devrez ajouter les fichiers glm)
# Ici on suppose que glm est dans le répertoire (à télécharger manuellement)
# Mais pour faciliter, on utilisera un mini glm intégré? Non, on peut demander au système.
# Ajoutons GLM via un include externe. Je fournis un glm minimal dans le code.
# En pratique, vous devez avoir les headers GLM. Pour ce build, on va utiliser des maths maison simples.
# Pour éviter la dépendance, j'implémente des fonctions vectorielles basiques dans utils.h.

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
)

target_link_libraries(RelicSeeker
    GLESv3
    EGL
    android
    log
    OpenSLES
    z
)
