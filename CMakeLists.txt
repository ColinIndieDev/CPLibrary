cmake_minimum_required(VERSION 3.31)
project(Example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

if (EMSCRIPTEN)
    set(GLFW_BUILD_WSI_EMSCRIPTEN ON CACHE BOOL "" FORCE)
    set(GLFW_USE_POSIX_TIME OFF CACHE BOOL "" FORCE)
    set(GLFW_USE_EMSCRIPTEN ON CACHE BOOL "" FORCE)

    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_COCOA OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WIN32 OFF CACHE BOOL "" FORCE)
    set(GLFW_USE_OSMESA OFF CACHE BOOL "" FORCE)
endif()

# GLFW
if(NOT EMSCRIPTEN)
    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG master
    )
    FetchContent_MakeAvailable(glfw)
endif()

# GLM
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG master
)
FetchContent_MakeAvailable(glm)

# Freetype
FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG VER-2-14-1
)
FetchContent_MakeAvailable(freetype)

# STB
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Miniaudio
set(MINIAUDIO_PATH "${CMAKE_BINARY_DIR}/_deps/miniaudio/miniaudio.h")
file(DOWNLOAD
        https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
        ${MINIAUDIO_PATH}
        SHOW_PROGRESS
)
add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE "${CMAKE_BINARY_DIR}/_deps/miniaudio")

# GLAD
add_subdirectory(external/glad)

# Fast Noise Lite
FetchContent_Declare(
        fastnoiselite
        GIT_REPOSITORY https://github.com/Auburn/FastNoiseLite.git
        GIT_TAG master
)
FetchContent_MakeAvailable(fastnoiselite)

add_executable(Example 
    src/main.cpp
    src/TextureLoader.cpp
    src/TextureLoader.h
    src/WorldGen.cpp
    src/WorldGen.h
    src/Block.cpp
    src/Block.h
    src/Chunk.cpp
    src/Chunk.h
    src/ChunkManager.cpp
    src/ChunkManager.h
    src/Game.cpp
    src/Game.h

    CPLibrary/Shader.cpp
    CPLibrary/Shader.h
    CPLibrary/shapes2D/Triangle.cpp
    CPLibrary/shapes2D/Triangle.h
    CPLibrary/Engine.cpp
    CPLibrary/Engine.h
    CPLibrary/CPL.cpp
    CPLibrary/CPL.h
    CPLibrary/shapes2D/Rectangle.cpp
    CPLibrary/shapes2D/Rectangle.h
    CPLibrary/shapes2D/Circle.cpp
    CPLibrary/shapes2D/Circle.h
    CPLibrary/Colors.h
    CPLibrary/KeyInputs.h
    CPLibrary/Text.cpp
    CPLibrary/Text.h
    CPLibrary/shapes2D/Line.cpp
    CPLibrary/shapes2D/Line.h
    CPLibrary/shapes2D/Texture2D.cpp
    CPLibrary/shapes2D/Texture2D.h
	CPLibrary/shapes2D/ScreenQuad.cpp
	CPLibrary/shapes2D/ScreenQuad.h
    CPLibrary/shapes2D/GlobalLight.h
	CPLibrary/shapes2D/PointLight.h
    CPLibrary/shapes2D/Tilemap.cpp
    CPLibrary/shapes2D/Tilemap.h
    CPLibrary/shapes2D/ParticleSystem.h 
    CPLibrary/shapes2D/ParticleSystem.cpp
    CPLibrary/shapes3D/Cube.cpp
    CPLibrary/shapes3D/Cube.h
    CPLibrary/shapes3D/Sphere.cpp
    CPLibrary/shapes3D/Sphere.h
    CPLibrary/shapes3D/CubeTex.cpp
    CPLibrary/shapes3D/CubeTex.h
    CPLibrary/shapes3D/PlaneTex.cpp
    CPLibrary/shapes3D/PlaneTex.h
    CPLibrary/shapes3D/ShadowMap.cpp
    CPLibrary/shapes3D/ShadowMap.h
    CPLibrary/shapes3D/PointLight3D.h
    CPLibrary/shapes3D/DirectionalLight.h
    CPLibrary/shapes3D/Frustum.h
    CPLibrary/shapes3D/CubeMap.cpp
    CPLibrary/shapes3D/CubeMap.h
    CPLibrary/timers/Timer.h
    CPLibrary/timers/TimerManager.cpp
    CPLibrary/timers/TimerManager.h
    CPLibrary/CPLibrary.h
    CPLibrary/utils/Logging.h
    CPLibrary/utils/ScopedTimer.h
    CPLibrary/Audio.cpp
    CPLibrary/Audio.h
)

# Include directories
target_include_directories(Example PRIVATE ${stb_SOURCE_DIR} ${fastnoiselite_SOURCE_DIR}/Cpp)

# Link libraries
target_link_libraries(Example PRIVATE glad freetype miniaudio glm)

if (NOT EMSCRIPTEN)
    set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)

    target_link_libraries(Example PRIVATE glfw)
    
    # Linux-specific: Link required system libraries
    if (UNIX AND NOT APPLE)
        target_link_libraries(Example PRIVATE
            dl          # Dynamic loading
            pthread     # Threading
            m           # Math library
        )
    endif()    
endif()

if(EMSCRIPTEN)
    set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
    target_link_options(Example PRIVATE
        "--preload-file" "${ASSETS_DIR}@/assets"
	    "-sMIN_WEBGL_VERSION=2"
	    "-sMAX_WEBGL_VERSION=2"
        "-sUSE_GLFW=3"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFORCE_FILESYSTEM=1"
    )

    # Debugging
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(Example PRIVATE
            "-sSAFE_HEAP=1"
            "-gsource-map"
            "-sASSERTIONS=2"
            "-sSTACK_SIZE=1mb"
            "-sDISABLE_EXCEPTION_CATCHING=0"
            "-fexceptions"
            "-sGL_DEBUG=1"
            "-g"
        )
    endif()
    set_target_properties(Example PROPERTIES SUFFIX ".html")
endif()
