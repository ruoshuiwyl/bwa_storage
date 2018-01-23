cmake_minimum_required(VERSION 3.9)
project(base)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER g++)

link_directories(
        /home/ruoshui/local/googletest-1.8.0/lib
)

add_executable(base main.cpp thread_pool.cc thread_pool.h blocking_queue.h channel.h skiplist.h arena.h arena.cc freelist.h)
target_link_libraries(base  pthread)

add_executable(skiptest skiplist.h arena.cc arena.h skiplist_test.cc)
target_link_libraries(skiptest gtest.a pthread)
