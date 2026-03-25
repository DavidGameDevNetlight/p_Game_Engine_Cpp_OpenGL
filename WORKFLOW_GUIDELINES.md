# Workflow Guidelines

How to add files, manage dependencies, and keep the build working.

---

## Project Structure

```
p-Game_Engine_Cpp_OpenGL/
├── CMakeLists.txt              # Root: defines the executable, links Engine
├── source/
│   ├── Game.h / .cpp
│   └── main.cpp                # Application entry point
├── engine/
│   ├── CMakeLists.txt          # Engine library: defines sources, links GLEW/GLFW
│   ├── source/                 # Engine .h and .cpp files
│   │   ├── eng.h               # Public facade header (include this from main)
│   │   ├── Engine.h / .cpp
│   │   ├── Application.h / .cpp
│   │   └── input/
│   │        └── InputManager.h / .cpp
│   └── thirdparty/
│       ├── glfw-3.4/           # GLFW (built from source)
│       └── glew/               # GLEW (built from source)
└── build/                      # Generated build artifacts (gitignored)
```

---

## Adding New Source Files

### Engine files (inside `engine/source/`)

1. Create the `.h` and `.cpp` files in `engine/source/`
2. Register them in `engine/CMakeLists.txt` under `PROJECT_SOURCE_FILES`:
   ```cmake
   set(PROJECT_SOURCE_FILES
       source/Engine.h
       source/Engine.cpp
       source/Application.h
       source/Application.cpp
       source/eng.h
       source/YourNewFile.h      # <-- add here
       source/YourNewFile.cpp    # <-- add here
   )
   ```
3. If the new header should be available to consumers (main.cpp), add it to `eng.h`:
   ```cpp
   #include "YourNewFile.h"
   ```
4. Re-run CMake (see "CMake Rebuild Pipeline" below)

### Application files (inside `source/`)

1. Create the `.h` and `.cpp` files in `source/`
2. Register them in the root `CMakeLists.txt` under `PROJECT_SOURCE_FILES`:
   ```cmake
   set(PROJECT_SOURCE_FILES
       source/main.cpp
       source/Game.h             # <-- add here
       source/Game.cpp           # <-- add here
   )
   ```
3. Re-run CMake

---

## CMake Rebuild Pipeline

### When to re-run CMake

You **must** re-run CMake when you:
- Add or remove source files from a `CMakeLists.txt`
- Add a new library or dependency
- Change any CMake variable or option

### When you only need to Build (no CMake re-run)

You only need to build (not re-run CMake) when you:
- Edit existing `.h` or `.cpp` files
- Change code within already-registered files

---

## Visual Studio 2022 (Windows)

### Re-run CMake (regenerate the VS project)

```bash
cmake -S . -B build -G "Visual Studio 17 2022"
```

`-S` stands for source directory, and `.` refers to the current folder, which translates to "Look for the CMake file in the current folder".
`-B` stands for build directory, and `build` will be the name of the folder. And lastly `-G` stands for generator and the quotes are used because `"Visual Studio 17 2022"` has spaces in it.

### Build

```bash
cmake --build build
```
Or just press **Ctrl+B** / **Build Solution** in Visual Studio.

### Full clean rebuild

If things get into a bad state:
```bash
rm -rf build
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build
```

---

## CLion (macOS)

CLion reads `CMakeLists.txt` directly — it does not use a separate generator step.
When you add a file to `CMakeLists.txt` and save, CLion detects the change and reloads
the project automatically.

### First-time setup

Open CLion and choose **Open**, then select the project root folder
(`p_Game_Engine_Cpp_OpenGL/`). CLion will detect `CMakeLists.txt` and configure itself.

If it does not configure automatically:
```
CLion menu → File → Reload CMake Project
```

### Re-run CMake (after editing CMakeLists.txt)

CLion usually triggers this automatically on save. To force it manually:
```
CLion menu → File → Reload CMake Project
```
Or click the **CMake** tab at the bottom of the IDE and press the reload (cycle) icon.

### Build

Press **Ctrl+F9** (or **Cmd+F9** on Mac) to build the current target.
Alternatively, click the green hammer icon in the top toolbar.

From the terminal (inside the project root):
```bash
cd build
make -j$(sysctl -n hw.logicalcpu)
```

`make` reads the `Makefile` that CLion (or CMake) generated.
`-j$(sysctl -n hw.logicalcpu)` runs the build in parallel using all available CPU cores.
`sysctl -n hw.logicalcpu` is a macOS command that returns the logical core count.

### Run

Press **Shift+F10** (or **Ctrl+R** on Mac) to run the selected target, or click the green
play button next to `GameDevelopmentProject` in the top toolbar.

From the terminal:
```bash
./build/GameDevelopmentProject
```

### Full clean rebuild

```bash
rm -rf build
```

Then in CLion:
```
File → Reload CMake Project
```

CLion will recreate the `build/` directory and reconfigure. Then build as normal with **Ctrl+F9**.

Or entirely from the terminal:
```bash
rm -rf build
mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
make -j$(sysctl -n hw.logicalcpu)
```

`rm -rf build` — removes the entire build directory and all its contents.
`mkdir build` — creates a fresh empty build directory.
`cd build` — moves into it.
`cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..` — configures the project.
`-G "Unix Makefiles"` selects the Makefile generator (used by `make`).
`-DCMAKE_BUILD_TYPE=Debug` enables debug symbols and disables optimizations.
`..` points CMake to the root `CMakeLists.txt` one directory up.
`make -j$(sysctl -n hw.logicalcpu)` — compiles using all CPU cores.

---

## Where to Create Files

**Recommended: Create files from the terminal or file explorer**, then register them in CMakeLists.txt, then re-run CMake. This is the most reliable method across all IDEs.

**Creating files from Visual Studio 2022:**
- Files created via VS "Add New Item" may be placed in the `build/` directory or marked as "Not included" because VS doesn't know to put them in `source/` or `engine/source/`.
- If you create a file from VS, make sure it's saved to the correct source directory, add it to the relevant CMakeLists.txt, then re-run CMake.

**Creating files from CLion:**
- Use **File → New → C/C++ Source File** or **C/C++ Header File** and make sure the destination path is `source/` or `engine/source/` — not inside `build/`.
- CLion may offer to add the file to CMakeLists.txt automatically. Accept this if prompted, but verify the entry was placed in the correct `set(PROJECT_SOURCE_FILES ...)` block.
- If CLion does not prompt, add the entry manually and save — CLion will reload CMake automatically.

**Summary:** Always ensure new files end up in the right source folder (`source/` or `engine/source/`) and are listed in the corresponding CMakeLists.txt.

---

## Adding a Third-Party Library

Follow the existing pattern used by GLFW and GLEW in `engine/CMakeLists.txt`:

1. Place the library source in `engine/thirdparty/<library-name>/`
2. Add to `engine/CMakeLists.txt`:
   ```cmake
   # Add the library
   add_subdirectory(thirdparty/<library-name> "${CMAKE_CURRENT_BINARY_DIR}/<library>_build")

   # If Engine source files need its headers internally:
   include_directories(thirdparty/<library-name>/include)

   # Link it to the Engine target
   target_link_libraries(${PROJECT_NAME} <library-target-name>)
   ```
3. Re-run CMake

---

## How the Engine Library is Exposed

The Engine uses `target_include_directories` with `PUBLIC` scope:

```cmake
target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)
```

This means any target that links against `Engine` (like the main executable) automatically gets the engine headers in its include path. You do **not** need to add `include_directories(engine/source)` in the root CMakeLists.txt — the `target_link_libraries(GameDevelopmentProject Engine)` call handles it.
