cmake_minimum_required(VERSION 3.16)
project(Three_d_engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ──────────────────────────────────────────────
# Output directories
# ──────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)

# ──────────────────────────────────────────────
# GLAD
# ──────────────────────────────────────────────
add_library(glad STATIC
    thirdparty/glad/glad/src/glad.c
)
target_include_directories(glad PUBLIC
    thirdparty/glad/glad/include
)

# ──────────────────────────────────────────────
# GLFW (pre-built / header-only layout)
# ──────────────────────────────────────────────
add_library(glfw STATIC IMPORTED)

if(WIN32)
    set(GLFW_LIB_NAME glfw3.lib)
elseif(APPLE)
    set(GLFW_LIB_NAME libglfw3.a)
else()
    set(GLFW_LIB_NAME libglfw3.a)
endif()

set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/thirdparty/glfw/glfw/lib/${GLFW_LIB_NAME}
    INTERFACE_INCLUDE_DIRECTORIES
        ${CMAKE_SOURCE_DIR}/thirdparty/glfw/glfw/include
)

# ──────────────────────────────────────────────
# GLM
# ──────────────────────────────────────────────
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE
    thirdparty/glm
)

# ──────────────────────────────────────────────
# Dear ImGui
# ──────────────────────────────────────────────
add_library(imgui STATIC
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_demo.cpp
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui/backends
    ${CMAKE_SOURCE_DIR}/thirdparty/glfw/glfw/include
    ${CMAKE_SOURCE_DIR}/thirdparty/glad/glad/include
)

# ──────────────────────────────────────────────
# Main Application Executable
# ──────────────────────────────────────────────
add_executable(${PROJECT_NAME}
    source/main.cpp
    "Rendering/Mesh.h"
    "Rendering/Mesh.cpp"
    "Rendering/Shader.h"
    "Rendering/Shader.cpp"
    "Rendering/Camera.h"
    "Rendering/Camera.cpp"      
    "Rendering/Renderer.h"
    "Rendering/Renderer.cpp"    
    "source/Physic.h" 
    "source/Physic.cpp" 
    "source/Shape.h" 
    "source/Shape.cpp" 
    "source/Application.h" 
    "source/Application.cpp" 
    "source/Track.h" 
    "source/Track.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    thirdparty/glad/glad/include
    thirdparty/glfw/glfw/include
    thirdparty/glm
    thirdparty/imgui
    thirdparty/imgui/backends
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glad
    glfw
    glm
    imgui
)

# Platform-specific system libs needed by GLFW / OpenGL
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif(APPLE)
    find_library(OPENGL_LIB OpenGL)
    find_library(COCOA_LIB Cocoa)
    find_library(IOKIT_LIB IOKit)
    find_library(COREVIDEO_LIB CoreVideo)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OPENGL_LIB} ${COCOA_LIB} ${IOKIT_LIB} ${COREVIDEO_LIB}
    )
else()   # Linux
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGL::GL dl pthread X11
    )
endif()

# Copy Resources folder to output directory after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Resources"
    "${CMAKE_SOURCE_DIR}/out/Resources"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

# ──────────────────────────────────────────────
# Post-Build: Copy Resources folder
# ──────────────────────────────────────────────
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Resources"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources"
    COMMENT "Copying assets to build output directory..."
)