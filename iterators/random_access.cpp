
#include <iostream>
#include <memory>
#include <algorithm>
#include <numeric>

template <typename T>
class Vector {
public:
    class iterator {
    public:
        friend Vector<T>;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = long long;
        using value_type        = T;
        using pointer           = T*;  // or also value_type*
        using reference         = T&;  // or also value_type&

        ~iterator() = default;

        iterator(const  iterator& other) : v_(other.v_), idx_(other.idx_) {
        }

        iterator& operator=(const iterator& other) {
            v_ = other.v_;
            idx_ = other.idx_;
            return *this;
        }

        iterator& operator ++ () {
            ++idx_;
            return *this;
        }

        iterator& operator -- () {
            --idx_;
            return *this;
        }

        const iterator operator ++ (int) {
            size_t old = idx_++;
            return iterator(v_, old);
        }

        const iterator operator -- (int) {
            size_t old = idx_--;
            return iterator(v_, old);
        }

        iterator& operator += (size_t diff) {
            idx_ += diff;
            return *this;
        }

        iterator& operator -= (size_t diff) {
            idx_ -= diff;
            return *this;
        }

        difference_type operator - (const iterator& other) {
            return idx_ - other.idx_;
        }

        iterator operator + (size_t diff) {
            iterator res(v_, idx_);
            return res += diff;
        }

        iterator operator - (size_t diff) {
            iterator res(v_, idx_);
            return res += diff;
        }

        friend iterator operator + (size_t diff, const iterator& it) {
            return it + diff;
        }

        reference operator [] (size_t offset) {
            return v_[idx_ + offset];
        }

        reference operator * () {
            return v_[idx_];
        }

        pointer operator -> () {
            return &v_[idx_];
        }

        bool operator == (const iterator& other) {
            return other.idx_ == idx_;
        }

        bool operator != (const iterator& other) {
            return other.idx_ != idx_;
        }

        bool operator < (const iterator& other) {
            return other.idx_ < idx_;
        }

        bool operator > (const iterator& other) {
            return other.idx_ > idx_;
        }

        bool operator <= (const iterator& other) {
            return other.idx_ <= idx_;
        }

        bool operator >= (const iterator& other) {
            return other.idx_ >= idx_;
        }

    private:
        explicit iterator(Vector<T>& v, size_t idx) : v_(v), idx_(idx) {
        }



        Vector<T>& v_;
        size_t idx_;
    };

    iterator begin() {
        return iterator(*this, 0);
    }

    iterator end() {
        return iterator(*this, size_);
    }

    void push_back(const T& el) {
        if (size_ == capacity_) {
            Vector old = std::move(*this);
            for (auto& e : old) {
                push_back(e);
            }
        }
        data_[size_++] = el;
    }

    ~Vector() = default;

    T& operator [] (size_t offset) {
        return data_[offset];
    }

    Vector() = default;

    Vector(const Vector& other) {

    }

    Vector(Vector<T>&& other) : capacity_(other.capacity_), size_(other.size_), data_(std::move(other.data_)) {
        other.capacity_ = 2;
        other.size_ = 0;
        other.data_ = std::move(std::unique_ptr<T[]>(new T[2]));
    }
// Оператор присвоения копированием
    Vector& operator = (Vector& other) {
        if (&other != this) {
            //clear();
            for (auto& e : other) {
                push_back(e);
            }
        }
        return *this;
    }
// Оператор присвоения перемещением
    Vector& operator = (Vector&& other)  noexcept {
        if (&other != this) {
            capacity_ = other.capacity_;
            size_ = other.size_;
            data_ = std::move(other.data_);
            other.capacity_ = 2;
            other.size_ = 0;
            other.data_ = std::move(std::unique_ptr<T[]>(new T[2]));
        }
        return *this;
    }


private:
    size_t capacity_{4};
    size_t size_{2};
    std::unique_ptr<T[]> data_{new T[capacity_]};

};


class Car {
public:
    Car() =default;
    Car& operator = (const Car& other) = default;
    Car(const Car& other) = default;

    void ride() {
        std::cout << "kek";
    }
    int x{0};
};



int main() {
    Vector<Car> v;
    v.push_back(Car());
    v.push_back(Car());
    v.push_back(Car());
    v.push_back(Car());
    v.push_back(Car());

    auto it = v.begin();
    it++;
    ++++it;


    it->ride();

    Vector<int> v2;
    v2.push_back(3);
    v2.push_back(3454333);
    v2.push_back(33);
    v2.push_back(333545);
    v2.push_back(3343);
    v2.push_back(3);
    v2.push_back(33);
    v2.push_back(-3);

    std::sort(v2.begin(), v2.end());
    for (auto& e : v2) {
        std::cout << e << " ";
    }

    size_t cnt = std::count_if(v2.begin(), v2.end(), [&](int x) {
        return x == 33;
    });

    std::cout << cnt << std::endl;

    std::cout << std::accumulate(v2.begin(), v2.end(), 0ll, [](int old, int x) -> long long {
        return old + x;
    });

    Vector<int> moved = std::move(v2);
    std::cout << "test" << std::endl;
    for (auto& e : v2) {
        std::cout << e << " ";
    }
    std::cout << "test" << std::endl;
    for (auto& e : moved) {
        std::cout << e << " ";
    }

    return 0;
}
