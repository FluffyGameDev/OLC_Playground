cmake_minimum_required (VERSION 3.8)

include_directories("${CMAKE_SOURCE_DIR}/common")
include_directories("${CMAKE_SOURCE_DIR}/SortingVisualizer")

add_executable (SortingVisualizer "sortingvisualizer.cpp")
target_link_libraries(SortingVisualizer PUBLIC Common)

set_property(TARGET SortingVisualizer PROPERTY CXX_STANDARD 17)