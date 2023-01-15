cmake_minimum_required(VERSION 3.5.1)

project(sdl-sandbox VERSION 1.0.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(NOT MSVC AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    message(STATUS "Running 'git submodule update --init --recursive'")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    else()
        message(STATUS "git submodule update --init --recursive finished successfully")
    endif()
endif()

option(BUILD_SHARED_LIBS "Build the library as a shared library" OFF)
option(BUILD_SAMPLES "Build the SDL2_image sample program(s)" OFF)
option(SDL2IMAGE_SAMPLES "Build the SDL2_image sample program(s)" OFF)
option(SDL2IMAGE_INSTALL "Enable SDL2_image install target" OFF)
option(SDL2IMAGE_VENDORED "Use vendored third-party libraries" ON)
option(SDL2MIXER_SAMPLES "Build the SDL2_mixer sample program(s)" OFF)
option(SDL2MIXER_INSTALL "Enable SDL2mixer install target" OFF)
option(SDL2MIXER_VENDORED "Use vendored third-party libraries" ON)
option(SDL2TTF_SAMPLES "Build the SDL2_ttf sample program(s)" OFF)
option(SDL2TTF_INSTALL "Enable SDL2_ttf install target" OFF)
option(SDL2TTF_VENDORED "Use vendored third-party libraries" ON)
option(SDL_STATIC_PIC "Static version of the library should be built with Position Independent Code" ON)
option(SUPPORT_OGG "Support loading OGG Vorbis music via libvorbis" ON)

add_subdirectory(external/SDL)
add_subdirectory(external/SDL_image)
add_subdirectory(external/SDL_mixer)
add_subdirectory(external/SDL_ttf)
add_subdirectory(sandbox)
add_subdirectory(sample)
