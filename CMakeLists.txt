cmake_minimum_required(VERSION 3.8)
project(CChip8)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(SOURCE_FILES
        cpu.c
        cpu.h
        main.c
        rom.c
        rom.h lcd.c lcd.h)

add_executable(CChip8 ${SOURCE_FILES})

# Find and add Simple2D Libs
find_path(SIMPLE2D_INCLUDE_DIR simple2d.h
        /usr/local/include
        /usr/local/include/simple2d
        /usr/include
        DOC "The directory where simple2d.h resides.")


find_library(SIMPLE2D_LIB
        NAMES libsimple2d.a
        PATHS
        /usr/local/lib
        /usr/local/lib/simple2d
        /usr/lib
        DOC "The simple2d library")

INCLUDE(FindPkgConfig)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SIMPLE2D REQUIRED_VARS SIMPLE2D_INCLUDE_DIR SIMPLE2D_LIB)

find_package(OpenGL REQUIRED)

PKG_SEARCH_MODULE(SDL2TTF REQUIRED sdl2_ttf)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED sdl2_mixer)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${SDL2MIXER_LIBRARIES} ${SIMPLE2D_LIB} ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2IMAGE_LIBRARIES})