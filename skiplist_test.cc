//
// Created by ruoshui on 1/22/18.
//

#include "gtest/gtest.h"
#include "skiplist.h"
#include "arena.h"
#include <set>

using namespace testing::internal;
namespace gamtools {
    typedef uint64_t Key;

    struct Comparator {
        int operator()(const Key &a, const Key &b) const {
            if (a < b) {
                return -1;
            } else if (a > b) {
                return 1;
            } else {
                return 0;
            }
        }
    };



    TEST(SkipTest, Empty) {
        Arena arena;
        Comparator cmp;
        SkipList<Key, Comparator> list(cmp, &arena);
        ASSERT_TRUE(!list.Contains(10));
        SkipList<Key, Comparator>::Iterator iter(&list);
        ASSERT_TRUE(!iter.Valid());
        iter.SeekToFirst();
        ASSERT_TRUE(!iter.Valid());
        iter.Seek(100);
        ASSERT_TRUE(!iter.Valid());
        iter.SeekToLast();
        ASSERT_TRUE(!iter.Valid());
    }
    TEST(SkipList, InsertAndLookup) {
        const int N = 20;
        const int R = 50;
//        Random rnd(1000);
        std::default_random_engine generator;
        std::uniform_int_distribution<int> rnd(0, 4999);
        std::set<Key> keys;
        Arena arena;
        Comparator cmp;
        SkipList<Key, Comparator> list(cmp, &arena);
        for (int i = 0; i < N; i++) {
//            Key key = rnd(generator) % R;
            Key key = i;
            if (keys.insert(key).second) {
                list.Insert(key);
                assert(list.Contains(key));
            }
            SkipList<Key, Comparator>::Iterator iter(&list);
            for (iter.SeekToFirst(); iter.Valid(); iter.Next()){
                std::cout << iter.key() << " ";
            }
            std::cout << std::endl;
        }

        for (int i = 0; i < R; i++) {
            if (list.Contains(i)) {
                ASSERT_EQ(keys.count(i), 1);
            } else {
                ASSERT_EQ(keys.count(i), 0);
            }
        }

    }
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
