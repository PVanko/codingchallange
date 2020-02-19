#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <condition_variable>

#include "lockGuard.h"

static std::mutex cvMutex;
static std::mutex resourceMutex;
static std::condition_variable cv;
static unsigned volatile threadId = 0;
const static unsigned threadNumber = 3;

using guard = lckg::lock_guard<std::mutex>;

void foo(const unsigned i) {
    using namespace std::chrono_literals;
    {
        guard lock(resourceMutex);
        std::cerr << "thread" << i << ": starting, waiting.\n";
    }
    while (true) {
        std::unique_lock<std::mutex> ul(cvMutex);
        cv.wait(ul, [&] { return threadId == i; });
        {
            guard lock(resourceMutex);
            std::cerr << "thread" << i << ": signal received, doing work .... \n";
        }
        auto time = rand() % 5 + 1;
        std::this_thread::sleep_for(std::chrono::seconds(time));
        {
            /* Also inefficient. Same case as above. */
            guard lock(resourceMutex);
            std::cerr << "thread" << i << ": done with work, signal next thread\n";
        }
        threadId = threadId % threadNumber + 1;
        ul.unlock();
        cv.notify_all();
    }
}

int main()
{
    std::vector<std::thread> threads(threadNumber);
    std::cerr << "main: starting all threads\n";
    {
        std::unique_lock<std::mutex> ul;
        for (unsigned i = 0; i < threadNumber; ++i) {
            threads[i] = std::thread(foo, i + 1);
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    {
        guard lock(resourceMutex);
        std::cerr << "main: starting thread 1.\n";
    }
    threadId = 1;
    cv.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }
    return 0;
}

