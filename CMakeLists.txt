cmake_minimum_required(VERSION 3.31)
project(q3_map_processor)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")

add_executable(q3mp
    main.cpp
    point3f.cpp
    nodes.cpp
    map.cpp
    generate_triggers.cpp
    config.cpp
)
