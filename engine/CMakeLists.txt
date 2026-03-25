cmake_minimum_required(VERSION 3.10)

project(Engine)

set(PROJECT_SOURCE_FILES
    source/eng.h
    source/Engine.h
    source/Engine.cpp
    source/Application.h
    source/Application.cpp
    source/input/InputManager.h
    source/input/InputManager.cpp
    source/graphics/GraphicsAPI.h
    source/graphics/GraphicsAPI.cpp
    source/graphics/ShaderProgram.h
    source/graphics/ShaderProgram.cpp
    source/graphics/VertexLayout.h
    source/graphics/VertexLayout.cpp
    source/render/Material.h
    source/render/Material.cpp
    source/render/Mesh.h
    source/render/Mesh.cpp
    source/render/RenderQueue.h
    source/render/RenderQueue.cpp
)
include_directories(source)

# Including the Engine as a library
add_library(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})


# --------------------------------------------------------------------- Add GLFW Library
# Configure and build GLFW from sources, placing build artifacts in /glfw_build
add_subdirectory(thirdparty/glfw-3.4 "${CMAKE_CURRENT_BINARY_DIR}/glfw_build")
# Expose include/ so we can use #include <GLFW/glfw3.h> in the cpp files
include_directories(thirdparty/glfw-3.4/include)

# --------------------------------------------------------------------- Add Glew Library
set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
# Builds GLEW and places the artifacts in /glew_build
add_subdirectory(thirdparty/glew/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
# Adds GLEW headers to the search path
include_directories(thirdparty/glew/include)

# Disable GLEW utilities we don't need
set_property(TARGET glew_s PROPERTY FOLDER GLEW)
set_property(TARGET glew PROPERTY FOLDER GLEW)


# Link all thirdparty libraries, links the app with GLFW and static GLEW (hence glew_s)
target_link_libraries(${PROJECT_NAME} glfw glew_s)

# --------------------------------------------------------------------- Add GLM Library
add_subdirectory(thirdparty/glm-1.0.1)
include_directories(thirdparty/glm-1.0.1)

# Engine exports its include path itself
target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/include
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw-3.4/include
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm-1.0.1
)