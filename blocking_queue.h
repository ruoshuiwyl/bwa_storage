//
// Created by ruoshui on 1/15/18.
//

#ifndef BASE_BLOCKING_QUEUE_H
#define BASE_BLOCKING_QUEUE_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <cassert>

namespace gamtools {
    template <typename T>
    class BlockingQueue {
    public:
        BlockingQueue():queue_(), mtx_(), cv_(){}
        ~BlockingQueue(){ assert(queue_.empty());}

        void Put( T&& t) {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push_back(t);
            cv_.notify_one();
        }
        T Take(){
            std::unique_lock<std::mutex> lock(mtx_);
            while(queue_.empty()){
                cv_.wait(lock);
            }
            T t = std::move(queue_.front());
            queue_.pop_front();
            return t;

        }
        size_t size() {
            std::lock_guard<std::mutex> lock(mtx_);
            return queue_.size();
        }

    private:
        std::deque<T> queue_;
        std::mutex mtx_;
        std::condition_variable cv_;

    };
}


#endif //BASE_BLOCKING_QUEUE_H
