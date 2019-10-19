cmake_minimum_required(VERSION 3.15)
project(asmCompiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(asmCompiler main.cpp stack.h commands.h commandsList.h)