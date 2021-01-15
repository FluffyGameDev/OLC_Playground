cmake_minimum_required (VERSION 3.8)

include_directories("${CMAKE_SOURCE_DIR}/common")
include_directories("${CMAKE_SOURCE_DIR}/MazeGen")

add_executable (MazeGen "MazeGen.cpp" "ui/maze.cpp")
target_link_libraries(MazeGen PUBLIC Common)