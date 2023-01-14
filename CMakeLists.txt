cmake_minimum_required(VERSION 3.5.1)

project(sdl-sandbox VERSION 1.0.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(NOT MSVC AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

option(BUILD_SHARED_LIBS "Build the library as a shared library" OFF)
option(BUILD_SAMPLES "Build the SDL2_image sample program(s)" OFF)
option(SDL2IMAGE_SAMPLES "Build the SDL2_image sample program(s)" OFF)
option(SDL2IMAGE_INSTALL "Enable SDL2_image install target" OFF)
option(SDL2MIXER_SAMPLES "Build the SDL2_mixer sample program(s)" OFF)
option(SDL2MIXER_INSTALL "Enable SDL2mixer install target" OFF)
option(SDL2TTF_SAMPLES "Build the SDL2_ttf sample program(s)" OFF)
option(SDL2TTF_INSTALL "Enable SDL2_ttf install target" OFF)
option(SDL_STATIC_PIC "Static version of the library should be built with Position Independent Code" ON)
option(SUPPORT_OGG "Support loading OGG Vorbis music via libvorbis" ON)

add_subdirectory(external/SDL)
add_subdirectory(external/SDL_image)
add_subdirectory(external/SDL_mixer)
add_subdirectory(external/SDL_ttf)
add_subdirectory(sandbox)
add_subdirectory(sample)
