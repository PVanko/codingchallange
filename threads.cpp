#include <iostream>
#include <thread>
void thread_function()
{
    std::cout << "Running thread " << std::endl;
}

int main()
{
    std::thread t1(&thread_function);
    std::thread t2(&thread_function);
    std::thread t3(&thread_function);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "main thread\n";

    return 0;
}
