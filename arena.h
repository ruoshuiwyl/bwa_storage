//
// Created by ruoshui on 1/21/18.
//

#ifndef BASE_AREAN_H
#define BASE_AREAN_H

#include <cstdio>
#include <vector>
#include <cassert>

namespace gamtools {
    class Arena {
    public:
        Arena();
        Arena(const Arena &) = delete;
        Arena& operator=(const Arena& ) = delete;
        ~Arena();

        char *Allocate(size_t bytes);
        char *AllocateAligned(size_t bytes);

        size_t MemoryUsage() const {
            return  blocks_memory_ + blocks_.capacity() * sizeof(char*);
        }


    private:
        char *AllocateFallback(size_t bytes);
        char *AllocateNewBlock(size_t block_bytes);
        char * alloc_ptr_;
        size_t alloc_bytes_remaining_;
        std::vector<char*> blocks_;
        size_t blocks_memory_;
    };


    inline char* Arena::Allocate(size_t bytes) {
        assert(bytes > 0);
        if (bytes <= alloc_bytes_remaining_) {
            char *result = alloc_ptr_;
            alloc_ptr_ += bytes;
            alloc_bytes_remaining_ -= bytes;
            return result;
        }
        return AllocateFallback(bytes);
    }
}



#endif //BASE_AREAN_H
