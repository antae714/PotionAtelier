#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

// std::concurrent_queue �����(?) ���� std::queue ���

namespace Utility
{
    template<typename T>
    class ConcurrentQueue
    {
    public:
        ConcurrentQueue() = default;
        ~ConcurrentQueue() = default;

    public:
        ConcurrentQueue(const ConcurrentQueue&) = delete;
        ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;
        ConcurrentQueue(ConcurrentQueue&&) = delete;
        ConcurrentQueue& operator=(ConcurrentQueue&&) = delete;

    public:
        void Push(const T& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(value);
            condVar.notify_one();
        }

        void Push(T&& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::forward<T>(value));
            condVar.notify_one();
        }

        void WaitPop(T& value)
        {
            std::unique_lock<std::mutex> lock(mutex);
            condVar.wait(lock, [this] {return !queue.empty(); });
            value = std::move(queue.front());
            queue.pop();
        }

        // ť���� ���� ������ (�񵿱�, ť�� ����� ��� ����)
        bool TryPop(T& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (queue.empty()) return false;

            value = std::move(queue.front());
            queue.pop();
            return true;
        }

        bool Empty() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.empty();
        }

        size_t Size() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.size();
        }

    private:
        std::queue<T> queue;
        mutable std::mutex mutex;
        std::condition_variable condVar;

    };
}