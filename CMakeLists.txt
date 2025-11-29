cmake_minimum_required(VERSION 3.29)
project(the-guild)

set(exec_name the-guild)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

message(${CURSES_INCLUDE_DIR})
message(${CURSES_LIBRARIES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(${exec_name} src/main.cpp)

add_subdirectory(src)

target_link_libraries(${exec_name} ${CURSES_LIBRARIES})

