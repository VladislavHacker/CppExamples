#include <iostream>
#include <memory>
#include <iterator>
using std::cout;

template<class T>
class Vector {
public:

    class iterator : std::iterator<std::bidirectional_iterator_tag, T> {
        friend Vector;
    public:
        T& operator*() {
            return v[now];
        }

        iterator& operator ++() {
            ++now;
            return *this;
        }

        iterator& operator --() {
            --now;
            return *this;
        }

        bool operator == (const iterator& other) {
            return now == other.now;
        }

        bool operator != (const iterator& other) {
            return now != other.now;
        }

    private:
        iterator(Vector& v, size_t now) : v(v), now(now) {}
        Vector& v;
        size_t now;
    };

    iterator begin() {
        return iterator(*this, 0);
    }

    iterator end() {
        return iterator(*this, size_);
    }

    explicit Vector(): capacity_(10), size_(0), data_(new T[size_]) {}

    // copy constructor
    Vector(const Vector<T>& v): capacity_(v.capacity_), size_(v.size_), data_(new T[size_]) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = v[i];
        }
    }

    // copy operator
    Vector<T>& operator= (const Vector<T>& v) {
        if (&v == this) {
            return *this;
        }
        Vector<T>(v).swap(*this);
    }

    Vector swap(Vector<T> v) {
        std::swap(data_, v.data_);
        std::swap(size_, v.size_);
        std::swap(capacity_, v.capacity_);
    }

    const T& operator[](size_t idx) const {
        return data_[idx];
    }

    T& operator[](size_t idx) {
        return data_[idx];
    }

    size_t size() const {
        return size_;
    }

    void push_back(const T& t) {
        if (size_ == capacity_) {
            capacity_ *= 2;
            std::unique_ptr<T[]> new_data(new T[capacity_]);
            for (size_t i = 0; i < size_; ++i) {
                new_data[i] = data_[i];
            }
            data_ = std::move(new_data);
        }
        data_[size_++] = t;
    }

    // to do
    void pop_back() {
        --size_;
    }

    friend std::ostream& operator<< (std::ostream& out, const Vector<T>& v) {
        for (size_t i = 0; i < v.size_; ++i) {
            out << v.data_[i] << " ";
        }
        return out;
    }

    // operator > <  != ==   spacesheep operator

    ~Vector() = default;


private:
    size_t capacity_;
    size_t size_;
    std::unique_ptr<T[]> data_;
};



int main() {
    Vector<int> v;
    v.push_back(10);
    v.push_back(12);
    Vector<int> v2 = v;

    for (auto& el : v2) {
        std::cout << el << " ";
    }
    return 0;
}
