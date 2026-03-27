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
        # Render Helper
        source/render/renderHelper/Model.h
        source/render/renderHelper/Model.cpp
        source/render/renderHelper/stb_image.c
        source/render/renderHelper/stb_image.h

        #  ImGUI, the library does not contain a CMake file, so it just can be added
        thirdparty/imgui/imconfig.h
        thirdparty/imgui/imgui.cpp
        thirdparty/imgui/imgui.h
        thirdparty/imgui/imgui_draw.cpp
        thirdparty/imgui/imgui_impl_glfw.cpp
        thirdparty/imgui/imgui_impl_glfw.h
        thirdparty/imgui/imgui_impl_opengl3.cpp
        thirdparty/imgui/imgui_impl_opengl3.h
        thirdparty/imgui/imgui_impl_opengl3_loader.h
        thirdparty/imgui/imgui_internal.h
        thirdparty/imgui/imgui_tables.cpp
        thirdparty/imgui/imgui_widgets.cpp
        thirdparty/imgui/imstb_rectpack.h
        thirdparty/imgui/imstb_textedit.h
        thirdparty/imgui/imstb_truetype.h

        thirdparty/tinyobjloader-1.0.6/tiny_obj_loader.h


)
include_directories(source)

# Including the Engine as a library
add_library(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})


# --------------------------------------------------------------------- Add GLFW Library
# Configure and build GLFW from sources, placing build artifacts in /glfw_build
add_subdirectory(thirdparty/glfw-3.4 "${CMAKE_CURRENT_BINARY_DIR}/glfw_build") # This will call the CMake file inside this directory
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

# --------------------------------------------------------------------- Add ImGUI
# add_subdirectory(thirdparty/imgui)
# include_directories(thirdparty/imgui) NOT necessary, modern CMAKE uses the target directories

# Engine exports its include path itself
target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/include
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw-3.4/include
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm-1.0.1
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tinyobjloader-1.0.6
)

# ----------------- POST BUILD COMMANDS
add_custom_command(TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/source/render/renderHelper/debuggingRenderAssets"
        "${CMAKE_BINARY_DIR}/assets"
        COMMENT "Copying engine debug assets to build directory"
)
#  This puts assets at cmake-build-debug/assets/ alongside the executable — independently of the game target.