cmake_minimum_required(VERSION 3.10)

# Creates a variable called PROJECT_NAME
project(GameDevelopmentProject)

# Defines the app sources
set(PROJECT_SOURCE_FILES
    source/main.cpp
    source/Game.h
    source/Game.cpp
)

# ------------------------------------------------------- Shaders
file(GLOB_RECURSE SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/source/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/shaders/*.frag"
)

source_group("Shaders" FILES ${SHADERS})

# -------------------------------------------------------
# Adds source/ to the header search path
include_directories(source)
# Build executable target called GameDevelopmentProject
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES} ${SHADERS})

# Copy shaders to build output directory so the exe can find them at runtime
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/source/shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
        COMMENT "Copying shaders to build directory"
)
# -------- GLEW and GLFW are included through the Engine module, so we link the Engine library

# Add the directory
add_subdirectory(engine "${CMAKE_CURRENT_BINARY_DIR}/engine_build")

# Link the Engine
target_link_libraries(${PROJECT_NAME}
    Engine
)


