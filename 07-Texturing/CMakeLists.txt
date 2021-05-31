cmake_minimum_required (VERSION 3.5)
project(Maze VERSION 0.1)

# library flags
set(CMAKE_CXX_STANDARD_LIBRARIES "-lepoxy -lGL -lglfw -pthread")
set (CMAKE_CXX_STANDARD 17)



include_directories(src)
add_subdirectory(src)

add_executable(main.out
"main.cpp"
)

target_link_libraries(
    main.out
    PUBLIC
    src_main
    AGL
    COMMON
)