cmake_minimum_required(VERSION 3.0)
project (Storm)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED on)
set (CMAKE_CXX_EXTENSIONS off)
set (CMAKE_CXX_COMPILER "g++")

add_executable(storm
    src/storm.cc
    src/util.cc
    src/file/read.cc
    src/parser/splice.cc
    src/parser/parser.cc
    src/parser/svalues.cc
    src/interpreter/interpreter.cc
)