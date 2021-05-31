file(GLOB LIBS_CPP "${CMAKE_CURRENT_LIST_DIR}/Main/*.cpp")
file(GLOB LIBS_HPP "${CMAKE_CURRENT_LIST_DIR}/Main/*.hpp")

add_library(
    src_main
    STATIC
    ${LIBS_HPP}
    ${LIBS_CPP}
)
target_include_directories(src_main PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")


file(GLOB LIBS_CPP "${CMAKE_CURRENT_LIST_DIR}/AGL/*.cpp")
file(GLOB LIBS_HPP "${CMAKE_CURRENT_LIST_DIR}/AGL/*.hpp")

add_library(
    AGL
    STATIC
    ${LIBS_HPP}
    ${LIBS_CPP}
)
target_include_directories(AGL PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")


file(GLOB LIBS_CPP "${CMAKE_CURRENT_LIST_DIR}/common/*.cpp")
file(GLOB LIBS_HPP "${CMAKE_CURRENT_LIST_DIR}/common/*.hpp")

add_library(
    COMMON
    STATIC
    ${LIBS_HPP}
    ${LIBS_CPP}
)
target_include_directories(COMMON PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")