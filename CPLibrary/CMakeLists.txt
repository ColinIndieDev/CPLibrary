cmake_minimum_required(VERSION 3.31)
project(CPLibrary VERSION 1.0.0 LANGUAGES C CXX)

# C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#### CPL files ####

file(GLOB_RECURSE CPL_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE CPL_HEADER_FILES CONFIGURE_DEPENDS include/*.h)

add_library(CPLibrary STATIC ${CPL_SOURCE_FILES} ${CPL_HEADER_FILES})
target_compile_features(CPLibrary PUBLIC cxx_std_17)

target_include_directories(CPLibrary PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

#### Dependencies ####
include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
    SOURCE_SUBDIR none
)
FetchContent_MakeAvailable(glm)

# GLM interface (header-only)
add_library(glm_bundled INTERFACE)
target_include_directories(glm_bundled INTERFACE
    $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Freetype
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-14-1
)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(freetype)

# STB
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Miniaudio
set(MINIAUDIO_PATH "${CMAKE_BINARY_DIR}/_deps/miniaudio")
file(MAKE_DIRECTORY ${MINIAUDIO_PATH})
file(DOWNLOAD
    https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
    ${MINIAUDIO_PATH}/miniaudio.h
    SHOW_PROGRESS
)
add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE 
    $<BUILD_INTERFACE:${MINIAUDIO_PATH}>
    $<INSTALL_INTERFACE:include/miniaudio>
)

# GLAD
add_subdirectory(external/glad)

# FastNoiseLite
FetchContent_Declare(
    fastnoiselite
    GIT_REPOSITORY https://github.com/Auburn/FastNoiseLite.git
    GIT_TAG master
)
FetchContent_MakeAvailable(fastnoiselite)

# STB and FastNoiseLite
add_library(stb_interface INTERFACE)
target_include_directories(stb_interface INTERFACE
    $<BUILD_INTERFACE:${stb_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/stb>
)
add_library(fastnoise_interface INTERFACE)
target_include_directories(fastnoise_interface INTERFACE
    $<BUILD_INTERFACE:${fastnoiselite_SOURCE_DIR}/Cpp>
    $<INSTALL_INTERFACE:include/fastnoiselite>
)

#### Link Libraries ####
target_link_libraries(CPLibrary PUBLIC 
    glad 
    glfw 
    freetype 
    miniaudio 
    glm_bundled
)

target_link_libraries(CPLibrary PRIVATE
    stb_interface
    fastnoise_interface
)

if(MSVC)
    target_compile_options(CPLibrary PRIVATE /W4)
else()
    target_compile_options(CPLibrary PRIVATE -Wall -Wextra -Wpedantic)
endif()

#### Installation ####
include(GNUInstallDirs)

install(TARGETS 
    CPLibrary 
    glad 
    glfw 
    freetype 
    miniaudio 
    glm_bundled
    stb_interface 
    fastnoise_interface
    EXPORT CPLibraryTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# GLAD headers
install(DIRECTORY external/glad/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# GLM headers
install(DIRECTORY ${glm_SOURCE_DIR}/glm
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# STB headers
install(DIRECTORY ${stb_SOURCE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/stb
    FILES_MATCHING PATTERN "*.h"
)

# Miniaudio header
install(FILES ${MINIAUDIO_PATH}/miniaudio.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}#/miniaudio
)

# FastNoiseLite headers
install(DIRECTORY ${fastnoiselite_SOURCE_DIR}/Cpp/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fastnoiselite
    FILES_MATCHING PATTERN "*.h"
)

# Export targets
install(EXPORT CPLibraryTargets
    FILE CPLibraryTargets.cmake
    NAMESPACE CPL::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CPLibrary
)

# Config files
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPLibraryConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CPLibraryConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CPLibrary
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CPLibraryConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CPLibraryConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CPLibraryConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CPLibrary
)
