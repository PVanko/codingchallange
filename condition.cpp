#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;

void thread_function(int tid)
{
    std::cout << "thread" << tid <<": Starting, waiting " << std::endl;
    std::unique_lock<std::mutex> lk(m);
    auto time = rand() % 5 + 1;
    std::this_thread::sleep_for(std::chrono::seconds(time));
    cv.wait(lk, [] {return ready; });
    std::cout << "thread " << tid << ": signal received, doing work .... " << std::endl;
    std::cout << "thread " << tid << ": done with work, signal next thread" << std::endl;
    lk.unlock();
    cv.notify_one();
}

int main()
{
    std::cout << "Starting all threads " << std::endl;
    std::thread t1(&thread_function, 1);
    std::thread t2(&thread_function, 2);
    std::thread t3(&thread_function, 3);
    std::cout << "Starting main thread" << std::endl;
    data = "Example data";
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
    }
    cv.notify_one();

    // wait for the worker
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [] {return processed; });
    }

    t1.join();
    t2.join();
    t3.join();

    
    return 0;
}
