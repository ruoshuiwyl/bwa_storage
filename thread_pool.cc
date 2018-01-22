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
        void SetBackgroundThreads(int num);
        void JoinThreads();
        int QueueSize() const;
        int GetBackgroundThreads() ;
    private:
        void BGThread();
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
    void ThreadPool::Impl::JoinThreads() {
        std::unique_lock<std::mutex> lock(mu_);
        assert(!exit_all_threads_);
        wait_for_jobs_to_complete_ = true;
        exit_all_threads_ = true;
        total_threads_limit_ = 0;
        lock.unlock();
        bgsignal_.notify_all();
        for (auto& th : bgthreads_) {
            th.join();
        }
        bgthreads_.clear();
        exit_all_threads_ = false;
        wait_for_jobs_to_complete_ = false;
    }

    int ThreadPool::Impl::QueueSize() const {
        return queue_len_.load();
    }
    int ThreadPool::Impl::GetBackgroundThreads()  {
        return total_threads_limit_;
    }

    void ThreadPool::Impl::Submit(std::function<void()> &&task) {
        std::lock_guard<std::mutex> lock(mu_);
        if (exit_all_threads_){
            return ;
        }
        queue_.push_back(task);
        queue_len_.store(static_cast<int>(queue_.size()));
        bgsignal_.notify_one();
    }

    void ThreadPool::Impl::SetBackgroundThreads(int num) {
        std::unique_lock<std::mutex> lock(mu_);
        if (num > 0 ) {
            total_threads_limit_ = num;
        } else {
            lock.unlock();
            return ;
        }
        while((int)bgthreads_.size() < total_threads_limit_) {
            bgthreads_.push_back(std::thread(&Impl::BGThread, std::move(this)));
        }
    }

    void ThreadPool::Impl::BGThread() {
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

    ThreadPool::~ThreadPool() {
    }

    void ThreadPool::Start(int num_thread) {
        impl_->SetBackgroundThreads(num_thread);

    }
    void ThreadPool::SubmitJob(std::function<void()> &&f) {
        impl_->Submit(std::move(f));
    }

    void ThreadPool::ShutDown() {
        impl_->JoinThreads();
    }
    int ThreadPool::QueueSize() const {
        return impl_->QueueSize();
    }
    int ThreadPool::GetBackgroundThreads()  {
        return impl_->GetBackgroundThreads();
    }




}