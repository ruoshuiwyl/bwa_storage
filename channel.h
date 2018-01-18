//
// Created by ruoshui on 1/17/18.
//

#ifndef BASE_CHANNEL_H
#define BASE_CHANNEL_H

#include <condition_variable>
#include <queue>

namespace gamtools {
    template<typename T>
    class Channel {
    public:
        explicit Channel():eof_(false){}
        ~Channel() { assert(eof_); }
        void SendEof() {
            std::lock_guard<std::mutex> lock(lock_);
            eof_ = true;
            cv_.notify_all();
        }
        bool Eof() {
            std::lock_guard<std::mutex> lock(lock_);
            return buffer_.empty() && eof_;
        }
        void Write(T&& t) {
            std::lock_guard<std::mutex> lock(lock_);
            buffer_.emplace(std::forward<T>(t));
            cv_.notify_one();
        }
        bool Read(T &t) {
            std::unique_lock<std::mutex> lock(lock_);
            cv_.wait(lock, [&]{return eof_ || !buffer_.empty();});
            if (eof_ && buffer_.empty()){
                return false;
            }
            t = std::move(buffer_.front());
            buffer_.pop();
            return true;
        }
        size_t size() const {
            std::lock_guard<std::mutex> lk(lock_);
            return buffer_.size();
        }
    private:
        std::condition_variable cv_;
        std::mutex lock_;
        std::queue<T> buffer_;
        bool eof_;
    };
}

#endif //BASE_CHANNEL_H
