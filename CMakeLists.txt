cmake_minimum_required(VERSION 3.9)
project(base)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER g++-5)

add_executable(base main.cpp thread_pool.cc thread_pool.h blocking_queue.h )
target_link_libraries(base pthread)