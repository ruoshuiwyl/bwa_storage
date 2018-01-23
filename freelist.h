//
// Created by ruoshui on 1/23/18.
//

#ifndef BASE_FREELIST_H
#define BASE_FREELIST_H

#include <cstdlib>
#include <cassert>

namespace gamtools {
    enum {__ALIGN = 8};
    enum {__MAX_SIZE = 128};
    enum {__NFREELISTS = __MAX_SIZE / __ALIGN};
    template <bool threads, int ints>
    class SmallAlloc {
    private :
        static size_t ROUND_UP(size_t bytes) {
            return ((bytes + (__ALIGN - 1) ) & (~(__ALIGN - 1)));
        }

    private:
        union obj{
            union obj *free_list_link;
            char client_data[1];
        };
    private:
        static obj * volatile free_list[__NFREELISTS];
        static size_t FREELIST_INDEX(size_t bytes) {
            return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
        }

        static void *refill(size_t n);
        static char *chunk_alloc(size_t size, int &nobjs);

        static char *start_free;
        static char *end_free;
        static size_t heap_size;

    public:
        static void *allocate( size_t n);
        static void deallocate(void *p, size_t n);
        static void *reallocate(void *p, size_t old_sz, size_t new_sz);
    };


    template <bool threads, int ints>
    char *SmallAlloc<threads, ints>::start_free = nullptr;

    template <bool threads, int ints>
    char *SmallAlloc<threads, ints>::end_free = nullptr;

    template <bool threads, int ints>
    size_t SmallAlloc<threads, ints>::heap_size = 0;

    template <bool threads, int ints>
    SmallAlloc<threads, ints>::obj *volatile
    SmallAlloc<threads, ints>::free_list[__NFREELISTS] =
            {nullptr, nullptr, nullptr, nullptr, nullptr,
             nullptr, nullptr, nullptr, nullptr, nullptr,
             nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

    template <bool threads, int ints>
    void* SmallAlloc<threads, ints>::allocate(size_t n) {
        assert (n <= (size_t)__MAX_SIZE);
        obj *volatile *my_free_list;
        obj *result;
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == nullptr) {
            void *r = refill(ROUND_UP(n));
            return r;
        }
        *my_free_list = result->free_list_link;
        return result;
    }

    template <bool threads, int ints>
    void SmallAlloc<threads, ints>::deallocate(void *p, size_t n) {
        obj *q = (obj*)p;
        obj *volatile *my_free_list = free_list + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    };

    template <bool threads, int ints>
    void *SmallAlloc<threads, ints>::refill(size_t n) {
        int nobjs = 20;
        char *chunk = chunk_alloc(n, nobjs);
        obj *volatile *my_free_list;

        obj *result;
        obj * current_obj, *next_obj;
        int i;

        if (1 == nobjs) {
            return (chunk);
        }
        result = (obj *)chunk;
        *my_free_list = next_obj = (obj*)chunk + n;

        for ( i = 1; ; ++i) {
            current_obj = next_obj;
            next_obj = (obj*) ((char*)current_obj + n);
            if (nobjs - 1 == i) {
                current_obj->free_list_link = 0;
                break;
            } else {
                current_obj->free_list_link = next_obj;
            }
        }
        return result;
    }

    template <bool threads, int ints>
    char* SmallAlloc<threads, ints>::chunk_alloc(size_t size, int &nobjs) {
        char *result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;

        if (bytes_left >= total_bytes)  {
            result = start_free;
            start_free += total_bytes;
            return result;
        } else if (bytes_left >= size) {
            nobjs = bytes_left / size;
            total_bytes = nobjs * size;
            result = start_free;
            start_free += total_bytes;
            return result;
        } else {
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
            if (bytes_left > 0 ) {
                obj *volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
                ((obj *)start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj*)start_free;
            }
            start_free = (char*) malloc(bytes_to_get);
            assert(start_free != 0);
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return (chunk_alloc(size, nobjs));
        }
    }


    }

#endif //BASE_FREELIST_H
