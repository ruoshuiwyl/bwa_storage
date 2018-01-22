cmake_minimum_required(VERSION 3.9)
project(base)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER g++-5)

add_executable(base main.cpp thread_pool.cc thread_pool.h blocking_queue.h channel.h skiplist.h arena.h arena.cc skiplist_test.cc)
target_link_libraries(base pthread)

add_executable(skiptest skiplist.h arena.cc arena.h skiplist_test.cc)
target_link_libraries(skiptest gtest.a pthread)



add_executable(level_skiptest
        test/atomic_pointer.h
        test/port.h
        test/hash.cc
        test/hash.h
        test/skiplist.h
        test/arena.cc
        test/arena.h
        test/coding.cc
        test/coding.h
        test/random.h
        test/slice.h
        test/port_posix.h
        test/port_posix.cc
        test/logging.cc
        test/logging.h
        test/testharness.cc
        test/env.cc
        test/status.cc
        test/env_posix.cc
        test/mutexlock.h
        test/skiplist_test.cc)
target_link_libraries(level_skiptest gtest.a pthread rt)