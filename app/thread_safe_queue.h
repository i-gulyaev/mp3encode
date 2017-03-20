#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>

template <typename T>
class ThreadSafeQueue
{

private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable cv;
public:
    ThreadSafeQueue() = default;

    ThreadSafeQueue(const ThreadSafeQueue& other) {
        std::lock_guard<std::mutex> lock(other.mutex);
        queue = other.queue;
    }

    void push(const T& data) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(data);
        cv.notify_one();
    }

    void pop(T& outData) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this](){return !queue.empty();});
        outData = queue.front();
        queue.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    unsigned int size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }
};

