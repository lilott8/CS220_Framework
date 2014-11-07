cmake_minimum_required(VERSION 2.8.4)
project(CS220_Framework)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -v")

add_subdirectory(Headers)
add_subdirectory(Source)

set(MAIN Source/main.cc)

add_executable(CS220_Framework ${MAIN})

target_link_libraries(CS220_Framework SOURCE_SOURCE)
target_link_libraries(CS220_Framework HEADER_SOURCE)