cmake_minimum_required(VERSION 3.9)
project(base)

set(CMAKE_CXX_STANDARD 11)

add_executable(base main.cpp thread_pool.cc thread_pool.h blocking_queue.h)