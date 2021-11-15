#include <iostream>
#include <functional>
#include <any>
#include <vector>

class StopIter : public std::runtime_error {
public:
    explicit StopIter(const std::string& s) : runtime_error(s) {
    }
};

class Range {
public:
    class iterator {
    public:
        iterator(std::function<int()>& f) : f(f) {
        }

        int operator * () {
            return f();
        }

        bool operator == (const iterator& other) {
            auto a = f;
            auto b = other.f;
            bool exception_a = false;
            bool exception_b = false;
            int val_a;
            int val_b;
            try {
                val_a = a();
            } catch (StopIter& e) {
                exception_a = true;
            }
            try {
                val_b = b();
            } catch (StopIter& e) {
                exception_b = true;
            }
            return ((exception_a && exception_b) || val_a == val_b);
        }

        bool operator != (const iterator& other) {
            return !(*this == other);
        }

        iterator& operator ++ () {
            return *this;
        }
    private:
        std::function<int()>& f;
    };

    Range(int min, int max, int step) : stepper(
    [min, max, step, direction=(min < max)]() mutable -> int {
        min += step;
        if (direction && min >= max) {
            throw StopIter("range ended");
        }
        if (!direction && min <= max) {
            throw StopIter("range ended");
        }
        return min;
       }
    ) {
    }

    iterator begin() {
        return iterator(stepper);
    }

    iterator end() {
        return iterator(endf);
    }

private:
    std::function<int()> stepper;
    std::function<int()> endf{[]() -> int {
        throw StopIter("range ended");
    }};
};

struct kekw {
    int x;
    operator std::string  () {
        return "hihg";
    }
};

int main() {
    for (auto&& i : Range(100, 0, -1)) {
        std::cout << i << std::endl;
    }

    return 0;
}
