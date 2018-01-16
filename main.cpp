#include <iostream>
#include <atomic>
#include "thread_pool.h"
#include <thread>
#include "blocking_queue.h"

gamtools::BlockingQueue<int> queue;
void foo()
{
    std::cout << "hello, foo" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    int x = 10;
    queue.Put(std::move(x));
}

void bar()
{
    auto x = queue.Take();
    std::cout << "hello, " << x << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main() {
    gamtools::ThreadPool thread_pool;
    thread_pool.Start(2);
    for (int i = 0; i < 10; ++i) {
        std::function<void()> f = std::bind(foo);
        int x = 10;
        std::function<void()> b = std::bind(bar);
        thread_pool.SubmitJob(std::move(f));
        thread_pool.SubmitJob(std::move(b));
        std::cout << "queue size" << thread_pool.QueueSize()<< std::endl;
    }
    std::cout<< "before shutdown" << thread_pool.GetBackgroundThreads()<<std::endl;
    std::cout<<thread_pool.GetBackgroundThreads()<<std::endl;

    thread_pool.ShutDown();
    std::cout<< "after shutdown" << thread_pool.GetBackgroundThreads()<<std::endl;
    return 0;
}