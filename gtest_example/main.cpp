#include "threadpool.hpp"
#include <iostream>

int main() {
    
    std::cout << "lol1";
    tp::ThreadPool pool(4);
    pool.Submit([]() {
        std::cout << "kek";
    });
    std::cout << "lol2";

    pool.Submit([]() {
        std::cout << "kek";
    });

    pool.Submit([]() {
        std::cout << "kek";
    });

    pool.Submit([]() {
        std::cout << "kek";
    });

    pool.Submit([]() {
        std::cout << "kek";
        tp::GetCurrentThreadPool()->Submit([]{
            std::cout << "Continue1";
            tp::GetCurrentThreadPool()->Submit([]{
                std::cout << "Continue2";
                tp::GetCurrentThreadPool()->Submit([]{
                    std::cout << "Continue3";
                });
            });
        });
    });
    std::cout << "lol";
    pool.Join();

    return 0;
}
