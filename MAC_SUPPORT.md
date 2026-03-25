# macOS Support

This document records every change made to port the engine from Windows (VS2021) to macOS,
and the steps required to build and run it.

---

## Prerequisites

Xcode Command Line Tools must be installed. They provide the C++ compiler (Clang),
the macOS SDK, the system OpenGL framework, and `make`.

```bash
xcode-select --install
```

A dialog will appear asking you to install the tools. Follow the prompts.
You do not need the full Xcode IDE.

Verify the installation:

```bash
xcode-select -p
# Expected output: /Library/Developer/CommandLineTools
```

CMake must also be installed. If you have Homebrew:

```bash
brew install cmake
```

Verify:

```bash
cmake --version
# Expected: cmake version 3.x.x
```

---

## Code Changes

### 1. Preprocessor directives — `engine/source/Engine.cpp`

**Why:** macOS requires two additional settings before an OpenGL context can be created:

- `GLFW_OPENGL_FORWARD_COMPAT GL_TRUE` — required by the macOS OpenGL driver when using a
  core profile. Without it, `glfwCreateWindow` returns `nullptr` and the engine exits immediately.

- `glewExperimental = GL_TRUE` — GLEW queries extensions using a method that is illegal in
  an OpenGL core profile on macOS. Setting this flag before `glewInit()` makes GLEW use
  `glGetStringi` instead, which is the correct core-profile method.

Both additions are wrapped in `#ifdef __APPLE__` so the Windows build is unaffected.

**Location:** `Engine::CreateWindow()`, after the three existing `glfwWindowHint` calls.

```cpp
// Before (Windows only)
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// After (cross-platform)
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glewExperimental = GL_TRUE;
#endif
```

`__APPLE__` is a macro automatically defined by Clang on all Apple platforms (macOS, iOS, etc.).
It is never defined on Windows or Linux, so this block compiles to nothing on those platforms.

---

### 2. Missing `#include <cstddef>` — `engine/source/render/RenderQueue.h`

**Why:** `size_t` is defined in `<cstddef>`. MSVC on Windows pulls it in transitively through
other system headers, so the omission went unnoticed. Clang on macOS is stricter and does not
inject it implicitly, causing a compile error: `unknown type name 'size_t'`.

```cpp
// Added at the top of the file
#include <cstddef>
```

---

### 3. Missing `#include <cstddef>` — `engine/source/input/InputManager.h`

**Why:** Same reason as above. `InputManager` stores a `const size_t m_keysSize` member,
which also requires `<cstddef>`.

```cpp
// Added at the top of the file
#include <cstddef>
```

---

## New Generated Files

### `engine/thirdparty/glfw-3.4/CMake/GenerateMappings.cmake` (placeholder)

**Why it was needed:** GLFW's `src/CMakeLists.txt` declares a CMake custom target called
`update_mappings` and lists `GenerateMappings.cmake` as one of its `SOURCES`. CMake validates
that all listed source files exist on disk at configure time. The script was missing from the
vendored copy of GLFW in this repository, so CMake aborted with:

```
CMake Error: Cannot find source file: .../CMake/GenerateMappings.cmake
```

**What the real script does:** It downloads the latest SDL gamepad button mappings from the
internet and regenerates `src/mappings.h`. Since `mappings.h` is already pre-generated and
committed in the repo, the script itself is not needed for the build — only its presence on
disk is required to satisfy CMake.

**Contents of the placeholder:**
```cmake
# Placeholder — mappings.h is already pre-generated in the source tree.
# This script is only needed when updating gamepad mappings from upstream SDL.
# Run the real script from the GLFW repository if you need to regenerate mappings.h.
```

The `update_mappings` target is never part of the default build. It only runs if you explicitly
call `make update_mappings`, so the placeholder is safe to leave in place indefinitely.

---

## Build Steps

All commands are run from the project root directory:
`/path/to/p_Game_Engine_Cpp_OpenGL/`

### Step 1 — Create the build directory

```bash
mkdir build
```

`mkdir` — make directory.
`build` — the name of the new directory. CMake will write all generated files and compiled
objects here, keeping the source tree clean.

### Step 2 — Move into the build directory

```bash
cd build
```

`cd` — change directory. All subsequent commands run from inside `build/`.

### Step 3 — Configure with CMake

```bash
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
```

`cmake` — invokes the CMake tool.
`-G "Unix Makefiles"` — selects the generator. Tells CMake to produce standard `Makefile`s
that the `make` tool can read. On macOS this uses Apple's Clang compiler automatically.
`-DCMAKE_BUILD_TYPE=Debug` — sets a CMake variable (`-D`). `Debug` means the compiler will
include debug symbols (`-g`) and disable optimizations, which makes debugging easier.
`..` — the path to the root `CMakeLists.txt`. Two dots means "one directory up", i.e. the
project root.

On success you will see:
```
-- Configuring done
-- Generating done
-- Build files have been written to: .../build
```

### Step 4 — Compile

```bash
make -j$(sysctl -n hw.logicalcpu)
```

`make` — reads the `Makefile` generated by CMake and compiles the project.
`-j` — run jobs in parallel (speeds up compilation).
`$(sysctl -n hw.logicalcpu)` — a shell command substitution. `sysctl -n hw.logicalcpu` prints
the number of logical CPU cores on your Mac. Passing that number to `-j` tells `make` to use
all available cores. For example, on an M2 with 8 cores this becomes `make -j8`.

On success the last lines will read:
```
[100%] Linking CXX executable GameDevelopmentProject
[100%] Built target GameDevelopmentProject
```

### Step 5 — Run

```bash
./GameDevelopmentProject
```

`./` — run an executable in the current directory (the `build/` folder).
`GameDevelopmentProject` — the name of the compiled binary, as defined by `project()` in the
root `CMakeLists.txt`.

Expected result: a window opens showing a rectangle with colored corners (red, green, blue,
yellow). Pressing the `A` key prints `Key[A]: Pressed!` to the terminal. Closing the window
exits cleanly.

---

## Quick Reference

```bash
# From the project root — run once
mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
make -j$(sysctl -n hw.logicalcpu)
./GameDevelopmentProject

# Subsequent builds after code changes — run from build/
make -j$(sysctl -n hw.logicalcpu)
./GameDevelopmentProject
```
