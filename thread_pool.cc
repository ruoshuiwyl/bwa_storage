//
// Created by ruoshui on 1/15/18.
//

#include "thread_pool.h"

#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <deque>
#include <atomic>
#include <cassert>

namespace gamtools {
    struct ThreadPool::Impl {
        using Task = std::function<void()> ;
        Impl();
        ~Impl();
        void Submit(std::function<void()> &&task);
        void BGThread(size_t thread_id);
    private:
        static void* BGThreadWrapper(void* arg);
        std::atomic<bool> exit_all_threads_;
        std::atomic<bool> wait_for_jobs_to_complete_;
        int total_threads_limit_;
        std::atomic<int> queue_len_;
        std::deque<Task> queue_;
        std::mutex               mu_;
        std::condition_variable  bgsignal_;
        std::vector<std::thread> bgthreads_;

    };


    ThreadPool::Impl::Impl() :exit_all_threads_(false),
                              wait_for_jobs_to_complete_(false),
                              queue_len_(0),
                              queue_(),
                              mu_(),
                              bgsignal_(),
                              bgthreads_(){}
    ThreadPool::Impl::~Impl() {
        assert(bgthreads_.size() == 0U);
    }
    struct BGThreadMetadata {
        ThreadPool::Impl* thread_pool_;
        size_t thread_id_;  // Thread count in the thread.
        BGThreadMetadata(ThreadPool::Impl* thread_pool, size_t thread_id)
                : thread_pool_(thread_pool), thread_id_(thread_id) {}
    };

//    void* ThreadPool::Impl::BGThreadWrapper(void *arg) {
//        BGThreadMetadata* meta = reinterpret_cast<BGThreadMetadata*>(arg);
//        size_t thread_id = meta->thread_id_;
//        ThreadPool::Impl* tp = meta->thread_pool_;
//        tp->BGThread(thread_id);
//    }



    void ThreadPool::Impl::BGThread(size_t thread_id) {
        while(true) {
            std::unique_lock<std::mutex> lock(mu_);
            while(!exit_all_threads_ && queue_.empty()) {
                bgsignal_.wait(lock);
            }
            if (exit_all_threads_) {
                if (!wait_for_jobs_to_complete_ || queue_.empty()) {
                    break;
                }
            }
            auto task = std::move(queue_.front());
            queue_.pop_front();
            queue_len_.store(static_cast<int>(queue_.size()));
            lock.unlock();
            task();
        }

    }
    ThreadPool::ThreadPool(): impl_(new Impl()){

    }

    void ThreadPool::Start(int num_thread) {
        impl_->

    }




}