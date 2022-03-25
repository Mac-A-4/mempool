cmake_minimum_required(VERSION 3.10)

project(mempool)

add_library(mempool STATIC src/mempool.c)

target_include_directories(mempool PUBLIC include)

add_executable(mempool_main src/main.c)

target_link_libraries(mempool_main PRIVATE mempool)