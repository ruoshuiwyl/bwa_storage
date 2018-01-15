//
// Created by ruoshui on 1/15/18.
//

#ifndef BASE_THREAD_POOL_H
#define BASE_THREAD_POOL_H

#include <functional>
#include <memory>

namespace gamtools{

    class ThreadPool {
    public:
        ThreadPool();
        ~ThreadPool();
        void Start(int num_thread);
        void SubmitJob(std::function<void ()> && f);
        void ShutDown();
        void QueueSize();
        struct Impl;
    private:
        std::unique_ptr<Impl> impl_;

    };

}

#endif //BASE_THREAD_POOL_H
