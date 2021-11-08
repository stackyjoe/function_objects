# CMakeList.txt : CMake project for function_objects, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 20)

# Add source to this project's executable.
add_library(function_objects INTERFACE "function_object.hpp" "copyable.hpp" "concepts.hpp" "move_only.hpp"  "copy_only.hpp" )

# TODO: Add tests and install targets if needed.
add_executable(function_objects_tests "tests.cpp")
target_link_libraries(function_objects_tests PRIVATE function_objects)
find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(function_objects_tests PRIVATE Catch2::Catch2)