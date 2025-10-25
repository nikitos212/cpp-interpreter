cmake_minimum_required(VERSION 3.16)

project(
    itmoscript_interpreter
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

include_directories(lib)
add_subdirectory(lib)
add_subdirectory(bin)

enable_testing()
add_subdirectory(tests)