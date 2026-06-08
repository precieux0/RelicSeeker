# Relic Seeker - Desktop Build

Cette version desktop est une adaptation simple du jeu Android pour Windows/Linux/macOS avec GLFW et GLEW.

## Contenu

- `main.cpp` : point d'entrée desktop et boucle de jeu minimale.
- `renderer_desktop.h` / `renderer_desktop.cpp` : renderer OpenGL 3.3 simple.
- `desktop/CMakeLists.txt` : configuration CMake pour compiler le jeu desktop.

## Build local

Sur une machine avec CMake, GLFW et GLEW installés :

```bash
cd desktop
cmake .
cmake --build . --config Release
```

## Exécution

```bash
./RelicSeekerDesktop
```

## GitHub Actions

Le workflow `.github/workflows/desktop-build.yml` compile automatiquement le binaire sur chaque push/pull request vers `main`.
