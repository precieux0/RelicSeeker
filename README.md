Relic Seeker - Multi-target build

This repository contains the Android game (NativeActivity + OpenGL ES) and a desktop port using GLFW+GLEW.

How to build

- Android: open the `app/` project in Android Studio (NDK) or use your existing Gradle workflow.
- Desktop (Linux/Windows/macOS via CMake):

  ```bash
  cd desktop
  cmake .
  cmake --build . --config Release
  ```

Notes

- I added `desktop/` with a minimal desktop renderer and a GitHub Actions workflow to build the desktop binary.
- For the desktop build to compile, `utils.h` must not include Android headers. I prepared `app/src/main/cpp/utils.h.fixed` which replaces the Android logging with stdio when `DESKTOP_BUILD` is defined. Replace `utils.h` with this file for desktop builds.

If you want, I can replace `utils.h` directly in the repo and commit the changes for you, or prepare a clean patch to apply locally.``