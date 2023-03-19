//
//  Copyright © 2023-Present, Arkin Terli. All rights reserved.
//
//  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
//  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
//  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
//  trade secret or copyright law. Dissemination of this information or reproduction of this
//  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>


class ThreadPool
{
public:
    // Constructor
    explicit ThreadPool(size_t maxThreadCount)
    {
        // Create threads
        for (size_t i=0; i<maxThreadCount; ++i)
        {
            m_workers.emplace_back([this]() { ThreadFunc(); });
        }
    }

    // Destructor
    ~ThreadPool()
    {
        m_queueSync.lock();
        m_exitNow = true;
        m_queueSync.unlock();

        m_taskSignal.notify_all();

        for(auto & worker: m_workers)
        {
            worker.join();
        }
    }

    // Add new task item to the queue
    template<class F, class... Args>
    auto Enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>>
    {
        using return_type = typename std::invoke_result_t<F, Args...>;

        // Create a packaged_task with task and arguments.
        auto taskPack = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        // Get future of task function to track taskFunc return value and exception, if there is one.
        std::future<return_type> res = taskPack->get_future();
        {
            std::unique_lock<std::mutex>    lock(m_queueSync);

            if (m_exitNow)
            {
                throw std::runtime_error("Can't enqueue new task since ThreadPool is deleted.");
            }
            m_taskQueue.emplace([taskPack]() { (*taskPack)(); });
        }

        m_taskSignal.notify_one();

        return res;
    }

private:

    void ThreadFunc()
    {
        for (;;)
        {
            std::function<void()>  task;

            {
                std::unique_lock<std::mutex>   lock(m_queueSync);
                m_taskSignal.wait(lock, [this]() { return m_exitNow || !m_taskQueue.empty(); });

                if (m_exitNow && m_taskQueue.empty())
                    return;

                task = std::move(m_taskQueue.front());

                m_taskQueue.pop();
            }

            task();
        }
    }

private:
    std::queue<std::function<void()>> m_taskQueue;
    std::condition_variable   m_taskSignal;
    std::vector<std::thread>  m_workers;
    std::mutex  m_queueSync;
    bool        m_exitNow{false};
};


