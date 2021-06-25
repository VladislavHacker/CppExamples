#include <iostream>
#include <memory>
#include <algorithm>

template <typename T>
struct remove_array {
    typedef T type;
};

template <typename T>
struct remove_array<T[]> {
    typedef T type;
};

template <typename T>
struct is_array {
    static constexpr bool res = false;
};

template <typename T>
struct is_array<T[]> {
    static constexpr bool res = true;
};


template <typename T>
class UniquePtr {
public:

    using pointer_type = typename remove_array<T>::type*;
    using data_type = typename remove_array<T>::type;
    UniquePtr(pointer_type ptr) : ptr(ptr) {
    }

    UniquePtr(UniquePtr& other) = delete;
    UniquePtr& operator=(const UniquePtr& other) = delete;

    UniquePtr(UniquePtr&& other) : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) {
        ptr = other.ptr;
        other.ptr = nullptr;
    };

    ~UniquePtr() {
        if (ptr == nullptr) {
            return;
        }
        if constexpr(is_array<T>::res) {
            delete [] ptr;
        } else {
            delete ptr;
        }
    }

    pointer_type operator -> () {
        return ptr;
    }

    data_type& operator[] (size_t i) {
        return ptr[i];
    };

private:
    pointer_type ptr;
};

template <typename T>
class WeakPtr;

template <typename T>
class SharedPtr {
public:

    friend class WeakPtr<T>;

    using pointer_type = typename remove_array<T>::type*;
    using data_type = typename remove_array<T>::type;

    explicit SharedPtr(pointer_type ptr)
        : ptr(ptr), shared_counter(new size_t(1)), weak_counter(new size_t(0)) {
    }

    SharedPtr(const SharedPtr<T>& other)
        : ptr(other.ptr), shared_counter(other.shared_counter), weak_counter(other.weak_counter) {
        ++*shared_counter;
    }

    SharedPtr& operator=(const SharedPtr<T>& other) {
        if (&other != this) {
            ptr = other.ptr;
            shared_counter = other.shared_counter;
            ++*shared_counter;
        }
        return *this;
    }

    ~SharedPtr() {
        --*shared_counter;
        if (*shared_counter == 0) {
            delete shared_counter;
            if (*weak_counter == 0) {
                delete weak_counter;
            }
            if constexpr(is_array<T>::res) {
                delete [] ptr;
            } else {
                delete ptr;
            }
        }
    }

    pointer_type operator -> () {
        return ptr;
    }

    data_type& operator[] (size_t i) {
        return ptr[i];
    };

private:

    explicit SharedPtr(const WeakPtr<T>& other) {
        weak_counter = other.weak_counter;
        shared_counter = other.shared_counter;
        ptr = other.ptr;
        ++*shared_counter;
    }

    size_t* weak_counter;
    size_t* shared_counter;
    pointer_type ptr;

};


template <typename T>
class WeakPtr {
public:
    using pointer_type = typename remove_array<T>::type*;
    using data_type = typename remove_array<T>::type;

    WeakPtr(SharedPtr<T>& other)
        : ptr(other.ptr), shared_counter(other.shared_counter), weak_counter(other.weak_counter) {
        ++*weak_counter;
    }

    WeakPtr(const WeakPtr<T>& other)
        : ptr(other.ptr), shared_counter(other.shared_counter), weak_counter(other.weak_counter) {
        ++*weak_counter;
    }

    WeakPtr& operator=(const WeakPtr<T>& other) {
        if (&other != *this) {
            ptr = other.ptr;
            shared_counter = other.shared_counter;
            weak_counter = other.weak_counter;
            ++*weak_counter;
        }
        return *this;
    }

    bool expired() {
        return  *shared_counter == 0;
    }

    SharedPtr<T> lock() {
        return SharedPtr(*this);
    }

    ~WeakPtr() {
        --*weak_counter;
        if (*weak_counter == 0 && *shared_counter == 0) {
            delete shared_counter;
            delete weak_counter;
        }
    }

    pointer_type operator -> () {
        return ptr;
    }

    data_type& operator[] (size_t i) {
        return ptr[i];
    };

private:
    pointer_type ptr;
    size_t* shared_counter;
    size_t* weak_counter;
};

int main() {
   SharedPtr<int[]> p(new int[100]);
   p[10] = 100;
   auto ptr = p;

   UniquePtr<int> p2(new int(10));
   WeakPtr<int[]> p3(p);
   std::cout << p3[10] << " " << p[10];
   int x = 8;
   int z = (6, 7);
   (z < x) ? z : x = 8;

   return 0;
}
