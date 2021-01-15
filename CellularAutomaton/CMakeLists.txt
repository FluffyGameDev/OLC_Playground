cmake_minimum_required (VERSION 3.8)

include_directories("${CMAKE_SOURCE_DIR}/common")
include_directories("${CMAKE_SOURCE_DIR}/CellularAutomaton")

add_executable (CellularAutomaton "CellularAutomatonApp.cpp" "ui/cellularautomaton.cpp")
target_link_libraries(CellularAutomaton PUBLIC Common)