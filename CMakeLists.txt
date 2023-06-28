cmake_minimum_required(VERSION 3.20)

project(cpp-raycaster LANGUAGES C)
set(CMAKE_C_STANDARD 99)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

include(FetchContent)

FetchContent_Declare(sdl2   GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG release-2.28.0)
FetchContent_MakeAvailable(sdl2)

message("SDL SourcDir: ${sdl2_SOURCE_DIR}")

set(SOURCES
  src/main.c
  src/ray.c
  src/sdl_context.c
)

add_executable(c-raycaster ${SOURCES})
target_link_libraries(c-raycaster PRIVATE SDL2)
target_include_directories(c-raycaster PRIVATE ${sdl2_SOURCE_DIR}/include/)
