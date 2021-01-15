cmake_minimum_required (VERSION 3.8)

include_directories("${CMAKE_SOURCE_DIR}/common")
add_library(Common STATIC "ui/button.cpp" "ui/scene.cpp")