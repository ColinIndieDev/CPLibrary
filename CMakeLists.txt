cmake_minimum_required(VERSION 3.31)
project(Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
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

file(GLOB_RECURSE CPL_SOURCE_FILES CONFIGURE_DEPENDS CPLibrary/src/*.cpp)
file(GLOB_RECURSE CPL_HEADER_FILES CONFIGURE_DEPENDS CPLibrary/include/*.h)
file(GLOB_RECURSE SOURCE_FILES CONFIGURE DEPENDS src/*.cpp src/*.h)

message(STATUS "============================================")
message(STATUS "Included files from CPL framework")
message(STATUS "============================================")
message(STATUS "----- CPL source files (.cpp) -----")
foreach(f IN LISTS CPL_SOURCE_FILES)
    file(RELATIVE_PATH rel ${CMAKE_SOURCE_DIR} ${f})
    message(STATUS "  ${rel}")
endforeach()

message(STATUS "")
message(STATUS "----- CPL header files (.h) -----")
foreach(f IN LISTS CPL_HEADER_FILES)
    file(RELATIVE_PATH rel ${CMAKE_SOURCE_DIR} ${f})
    message(STATUS "  ${rel}")
endforeach()
message(STATUS "============================================")
message(STATUS "Included files from src (game)")
message(STATUS "============================================")
message(STATUS "----- Game source files (.cpp & .h) -----")
foreach(f IN LISTS SOURCE_FILES)
    file(RELATIVE_PATH rel ${CMAKE_SOURCE_DIR} ${f})
    message(STATUS "  ${rel}")
endforeach()

add_executable(Example
    ${CPL_SOURCE_FILES}
    ${CPL_HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories(Example PRIVATE ${stb_SOURCE_DIR} ${fastnoiselite_SOURCE_DIR}/Cpp)
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
