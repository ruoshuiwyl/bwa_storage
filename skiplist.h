//
// Created by ruoshui on 1/21/18.
//

#ifndef BASE_SKIPLIST_H
#define BASE_SKIPLIST_H

#include <random>
#include <atomic>
#include <cassert>
#include "arena.h"
namespace  gamtools {
    class Arena;
    using atomic_void_ptr = std::atomic<void *> ;
    template < typename  Key, typename Comparator>
    class SkipList {
    private :
        struct Node;

    public:
        explicit SkipList(Comparator cmp, Arena* arena);
        void Insert(const Key &key);
        bool Contains(const Key &key) const ;
        class Iterator {
        public:
            explicit Iterator(const SkipList *list);
            bool Valid() const ;
            const Key& key() const;
            void Next();
            void Prev();
            void Seek(const Key &target);
            void SeekToFirst();
            void SeekToLast();

        private:
            const SkipList *skiplist_;
            Node * node_;
        };

    private:
        enum { kMaxHeight = 12 };

        Node *NewNode(const Key& key, int height);

        Node *FindLessThan(const Key& key) const;
        Node *FindLast() const ;
        bool KeyIsAfterNode(const Key& key, Node *n) const;
        Node *FindGreaterOrEqual(const Key &key, Node **prev) const;

        inline bool Equal(const Key& a, const Key& b) const  {
            return (compare_(a, b) == 0);
        }

        int RandowHeight();
        inline int GetMaxHeight() const {
            return max_height_.load(std::memory_order_relaxed);
        }


        Comparator const compare_;
        Arena *const arena_;
        Node *const head_;
        std::atomic<int> max_height_;
        std::default_random_engine generator_;
        std::uniform_int_distribution<int> rnd_;
    };




    template <typename  Key, typename  Comparator>
    struct SkipList<Key, Comparator>::Node {
        explicit Node(const Key &key): key(key){}
        Key const key;
        Node *Next(int n) {
            assert(n >= 0);
            return reinterpret_cast<Node*>(next_[n].load(std::memory_order_acquire));
        }
        void SetNext(int n, Node *x) {
            assert(n >= 0);
            next_[n].store(x, std::memory_order_release);
        }

    private:
        atomic_void_ptr next_[1];
    };

    template <typename Key, typename Comparator>
    typename SkipList<Key, Comparator>::Node*
    SkipList<Key, Comparator>::NewNode(const Key &key, int height) {
        char *mem = arena_->AllocateAligned( sizeof(Node) + sizeof(atomic_void_ptr) * (height - 1));
        return new (mem) Node(key);
    }


    template <typename Key, typename Comparator>
    inline SkipList<Key, Comparator>::Iterator::Iterator(const SkipList *list) {
        skiplist_ = list;
        node_ = nullptr;
    }
    template <typename Key, typename Comparator>
    inline bool SkipList<Key, Comparator>::Iterator::Valid() const {
        return node_ != nullptr;
    }

    template <typename Key, typename Comparator>
    inline const Key& SkipList<Key, Comparator>::Iterator::key() const {
        assert(Valid());
        return node_->key;
    }

    template <typename Key, typename Comparator>
    inline void SkipList<Key, Comparator>::Iterator::Next()  {
        assert(Valid());
        node_ = node_->Next(0);
    }

    template <typename Key, typename Comparator>
    inline void SkipList<Key, Comparator>::Iterator::Prev()  {
        assert(Valid());
//        node_ = node_->Next(0);
        node_ = skiplist_->FindLessThan(node_->key);
        if (node_ == skiplist_->head_){
            node_ = nullptr;
        }
    }

    template <typename Key, typename Comparator>
    inline void SkipList<Key, Comparator>::Iterator::Seek(const Key &target) {
        node_ = skiplist_->FindGreaterOrEqual(target, nullptr);
    }

    template <typename Key, typename Comparator>
    inline void SkipList<Key, Comparator>::Iterator::SeekToLast() {
        node_ = skiplist_->FindLast();
        if (node_ == skiplist_->head_) {
            node_ = nullptr;
        }
    }

    template <typename Key, typename Comparator>
    inline void SkipList<Key, Comparator>::Iterator::SeekToFirst() {
        node_ = skiplist_->head_->Next(0);
    }


    template <typename Key, typename Comparator>
    int SkipList<Key, Comparator>::RandowHeight() {
        static const uint32_t  kBranching = 4;
        int height = 1;
        int rnd = rnd_(generator_);
        assert(rnd >= 0 && rnd <= 3);
        while (height < kMaxHeight && (rnd == 0)) {
            height++;
        }
        assert((height> 0));
        assert(height <= kMaxHeight);
        return height;
    };




    template <typename Key, typename Comparator>
    SkipList<Key, Comparator>::SkipList(Comparator cmp, Arena *arena):compare_(cmp),
                                                     arena_(arena),
                                                     head_(NewNode(0, kMaxHeight)),
                                                     max_height_(1),
                                                     generator_(0xdeadbeef),
                                                     rnd_(0,3){
        for (int i = 0; i < kMaxHeight; ++i){
            head_->SetNext(i, nullptr);
        }
    }

    template <typename Key, typename Comparator>
    void SkipList<Key, Comparator>::Insert(const Key &key) {
        Node *prev[kMaxHeight];
        Node *x = FindGreaterOrEqual(key, prev);
        assert(x == nullptr || !Equal(key, x->key));

        int height = RandowHeight();
        if (height > GetMaxHeight()) {
            for (int i = GetMaxHeight(); i < height ; ++i){
                prev[i] = head_;
            }
            max_height_.store(height, std::memory_order_relaxed);
        }
        x = NewNode(key, height);
        for (int i = 0 ; i < height; ++i){
            x->SetNext(i, prev[i]->Next(i));
            prev[i]->SetNext(i, x);
        }
    }

    template <typename Key, typename Comparator>
    bool SkipList<Key, Comparator>::Contains(const Key &key) const {
        Node *x = FindGreaterOrEqual(key, nullptr);
        if (x != nullptr && Equal(key, x->key) ){
            return true;
        } else {
            return false;
        }
    }

    template <typename Key, typename Comparator>
    typename SkipList<Key, Comparator>::Node *
    SkipList<Key, Comparator>::FindGreaterOrEqual(const Key &key, Node **prev) const {
        Node* x = head_;
        int level = GetMaxHeight() - 1;
        while(true){
            Node *next = x->Next(level);
            if (KeyIsAfterNode(key, next)) {
                x = next;
            } else {
                if (prev != nullptr) prev[level] = x;
                if (level == 0) {
                    return next;
                } else {
                    level--;
                }

            }
        }
    }
    template <typename Key, typename Comparator>
    bool SkipList<Key, Comparator>::KeyIsAfterNode(const Key &key, Node *n) const {
        bool result =  (n != nullptr) && (compare_(n->key, key) < 0);
        return result;
    }

    template <typename Key, typename Comparator>
    typename SkipList<Key, Comparator>::Node *
    SkipList<Key, Comparator>::FindLast() const {
        Node *x = head_;
        int level = GetMaxHeight() - 1;
        while (true){
            Node *next = x->Next(level);
            if (next == nullptr) {
                if (level == 0){
                    return x;
                } else {
                    level--;
                }
            } else {
                x = next;
            }
        }
    }

    template <typename Key, typename Comparator>
    typename SkipList<Key, Comparator>::Node *
    SkipList<Key, Comparator>::FindLessThan(const Key &key) const {
        Node *x = head_;
        int level = GetMaxHeight() - 1;
        while (true) {
            assert(x == head_ || compare_(x->key, key) < 0);
            Node *next = x->Next(level);
            if (next == NULL || compare_(next->key, key) >= 0){
                if (level == 0){
                    return x;
                } else {
                    level--;
                }
            } else {
                x = next;
            }
        }
    }


}

#endif //BASE_SKIPLIST_H
