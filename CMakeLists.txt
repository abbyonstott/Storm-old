project (StormScript)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED on)
set (CMAKE_CXX_EXTENSIONS off)
set (CMAKE_CXX_COMPILER "g++")

add_executable(stormscript
    src/stormscript.cc
    src/util.cc
    src/file/read.cc
    src/parser/splice.cc
    src/parser/compile.cc
)