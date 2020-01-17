project (StormScript)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED on)
set (CMAKE_CXX_EXTENSIONS off)

add_executable(stormscript
    src/stormscript.cc
    src/file/read.cc
)