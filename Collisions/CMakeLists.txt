cmake_minimum_required (VERSION 3.8)

include_directories("${CMAKE_SOURCE_DIR}/Common")
include_directories("${CMAKE_SOURCE_DIR}/Collisions")
include_directories("${CMAKE_SOURCE_DIR}/external/glm/glm")

add_executable (Collisions "collisions.cpp")

set_property(TARGET Collisions PROPERTY CXX_STANDARD 17)