cmake_minimum_required(VERSION 3.30)
project(DockerTemplate)

set(CMAKE_CXX_STANDARD 20)

add_executable(DockerTemplate main.cpp Server.cpp Server.hpp)
