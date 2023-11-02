cmake_minimum_required(VERSION 3.25)
project(lab5)

set(CMAKE_CXX_STANDARD 17)

link_libraries(ws2_32)

add_executable(server server.cpp client.h client.cpp)


