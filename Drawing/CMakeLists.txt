cmake_minimum_required (VERSION 3.8)

include_directories("${CMAKE_SOURCE_DIR}/common")
include_directories("${CMAKE_SOURCE_DIR}/Drawing")
include_directories("${CMAKE_SOURCE_DIR}/external/glm/glm")

add_executable (Drawing "drawing.cpp")
target_link_libraries(Drawing PUBLIC Common)

set_property(TARGET Drawing PROPERTY CXX_STANDARD 17)