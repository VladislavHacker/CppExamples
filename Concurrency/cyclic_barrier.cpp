#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>
#include <vector>

namespace sync {

    class CyclicBarrier {
        public:
            CyclicBarrier(size_t count) : count_(count) {
            }

            void arrive() {
                std::unique_lock lk(m_);
                size_t index = index_;
                ++arrived_[index];
                if (arrived_[index] == count_) {
                    arrived_cv_.notify_all();
                    index_ ^= 1;
                } else {
                    while (index_ == index) {
                        arrived_cv_.wait(lk);
                    }
                }
                --arrived_[index];
            }

        private:
            std::mutex m_;
            std::condition_variable arrived_cv_;
            size_t arrived_[2]{0};
            size_t index_{0};
            size_t count_;
    };

}

int main() {
    sync::CyclicBarrier barr(2);

    std::vector<std::thread> v;
    std::atomic<int> s{0};
    std::atomic<int> steps{0};
    for (size_t i = 0; i < 2; ++i) {
        v.emplace_back([&barr, &s, &steps]() {
            size_t i = 0;
            while (i < 4) {
                barr.arrive();
                for (size_t i = 0; i < 1000000; ++i) {
                    ++s;
                }
                barr.arrive();
                ++steps;
                std::cout << steps.load() << " " << s.load() << std::endl;
                ++i;
            }
        });
    }

    std::cout << s.load();

    for (auto&& th : v) {
        th.join();
    }

    return 0;
}
