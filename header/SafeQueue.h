#ifndef _INCLUDE_SafeQueue_H
#define _INCLUDE_SafeQueue_H

#include <mutex>
#include <condition_variable>
#include <deque>
#include <optional>
#include <stdexcept>

template<typename T>
class SafeQueue {
private:
    std::deque<T> q;
    std::mutex m;
    std::condition_variable cv;
    bool closed = false;

public:
    SafeQueue() = default;
    ~SafeQueue() = default;

    // Disable copy/move
    SafeQueue(const SafeQueue&) = delete;
    SafeQueue& operator=(const SafeQueue&) = delete;

    // Push item into queue
    void push(T item) {
        {
            std::lock_guard<std::mutex> lk(m);
            if (closed) throw std::runtime_error("push on closed queue");
            q.push_back(std::move(item));
        }
        cv.notify_one();
    }

    // Pop item: waits until item available or queue is closed & empty
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&] { return !q.empty() || closed; });

        if (!q.empty()) {
            T item = std::move(q.front());
            q.pop_front();
            return item;
        }
        return std::nullopt;
    }

    // Close the queue to prevent further pushes
    void close() {
        {
            std::lock_guard<std::mutex> lk(m);
            closed = true;
        }
        cv.notify_all();
    }

    // Check if queue is empty
    bool empty() {
        std::lock_guard<std::mutex> lk(m);
        return q.empty();
    }

    // Check if queue is closed
    bool is_closed() {
        std::lock_guard<std::mutex> lk(m);
        return closed;
    }
};

#endif // _INCLUDE_SafeQueue_H
