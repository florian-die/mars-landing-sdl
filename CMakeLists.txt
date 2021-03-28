cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_C_STANDARD 11)

project(marslanding)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
    src/draw.c
    src/dynamics.c
    src/game.c
    src/sdl_utils.c
    src/state_list.c
    src/main.c
)

# Add an executable with the above sources
add_executable(marslanding ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(marslanding
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(marslanding ${SDL2_LIBRARIES})


if (UNIX)
  target_link_libraries(marslanding m)
endif (UNIX)