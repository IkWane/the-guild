cmake_minimum_required(VERSION 3.29)
project(the-guild)

set(CMAKE_CXX_STANDARD 20)

set(exec_name the-guild)
set(tag_getter_name the-gatherer)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(${exec_name} src/main.cpp)
add_executable(${tag_getter_name} src/gatherer.cpp)

add_subdirectory(src)

target_link_libraries(${exec_name} ${CURSES_LIBRARIES})

