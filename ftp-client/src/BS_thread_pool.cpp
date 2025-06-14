//#define BS_THREAD_POOL_MAIN
#include "BS_thread_pool.hpp" // BS::thread_pool
#include "BS_thread_pool.h" // BS::thread_pool
#include <chrono>             // std::chrono
#include <future>             // std::future
#include <iostream>           // std::cout
#include <thread>             // std::this_thread

BS::thread_pool pool;


void BS_thread_pool_post(void (*pf)(void*), void* vp)
{
    const std::future<void> my_future = pool.submit_task(
        [pf,vp]
        {
            if (pf)pf(vp);
        });
}


#if 0
int main()
{
    const std::future<void> my_future = pool.submit_task(
        []
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        });
    std::cout << "Waiting for the task to complete... ";
    my_future.wait();
    std::cout << "Done." << '\n';
}
#endif

#ifdef BS_THREAD_POOL_MAIN
static void func(void* vp)
{
    printf("vp=%d\n", (int)vp);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}
int main()
{
    BS_thread_pool_post(func, (void*)123);
}
#endif
