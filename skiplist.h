//
// Created by ruoshui on 1/21/18.
//

#ifndef BASE_SKIPLIST_H
#define BASE_SKIPLIST_H

#include <random>

namespace  gamtools {
    class Arena;
    template < typename  Key, typename Comparator>
    class SkipList {
    private :
        struct Node;

    public:
        explicit SkipList(Comparator cmp, Arena* arena);


    private:
        Comparator const compare_;
        Arena *const arena_;
        Node *const head_;
        std::atomic<void *> max_height_;
        std::uniform_int_distribution<int> rnd_;
    };


    template < typename  Key, typename Comparator>
    SkipList::SkipList(Comparator cmp, Arena *arena) : {

    }

}

#endif //BASE_SKIPLIST_H
