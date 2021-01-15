cmake_minimum_required (VERSION 3.8)

include_directories("${CMAKE_SOURCE_DIR}/common")
include_directories("${CMAKE_SOURCE_DIR}/RayTracing")
include_directories("${CMAKE_SOURCE_DIR}/external/glm/glm")

add_executable (RayTracing "raytracing.cpp")
target_link_libraries(RayTracing PUBLIC Common)

set_property(TARGET RayTracing PROPERTY CXX_STANDARD 17)