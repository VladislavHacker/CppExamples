#include <iostream>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>

class BasicStream {
public:
    explicit BasicStream(int fd) : fd_(fd) {
    }

    void close () {
        un_fail_ = false;
        ::close(fd_);
    };

    bool fail() {
        return !un_fail_;
    }

    explicit operator bool() {
        return !fail();
    }

protected:
    int fd_;
    bool un_fail_{true};
};

class PipeInputStream : public BasicStream {
public:
    explicit PipeInputStream(int fd) : BasicStream(fd) {
    }

    template <typename T>
    friend PipeInputStream& operator >> (PipeInputStream& in, T& x) {
        auto* buffer = new int8_t[sizeof(T)];
        int res = read(in.fd_, buffer, sizeof(T));
        if (res <= 0) {
            in.un_fail_ = false;
            delete []buffer;
            return in;
        }
        std::memcpy(reinterpret_cast<void*>(std::addressof(x)), buffer, sizeof(T));
        delete []buffer;
        return in;
    }


};

class PipeOutputStream : public BasicStream {
public:
    explicit PipeOutputStream(int fd) : BasicStream(fd) {
    }

    template <typename T>
    friend PipeOutputStream& operator << (PipeOutputStream& out, const T& x) {
        if (write(out.fd_, reinterpret_cast<const void*>(std::addressof(x)), sizeof(T)) <= 0) {
            out.un_fail_ = false;
        }
        return out;
    }

};

class PipeStream {
public:
    PipeStream() : pipe_fd_{0,0} {
        pipe(pipe_fd_);
    }

    PipeInputStream getPipeInputStream() {
        if (gotIn_ || gotOut_) {
            throw std::runtime_error("Error, some of streams is already got");
        }
        close(pipe_fd_[1]);
        gotIn_ = true;
        return PipeInputStream(pipe_fd_[0]);
    }

    PipeOutputStream getPipeOutputStream() {
        if (gotIn_ || gotOut_) {
            throw std::runtime_error("Error, some of streams is already got");
        }
        close(pipe_fd_[0]);
        gotOut_ = true;
        return PipeOutputStream(pipe_fd_[1]);
    }

private:
    int pipe_fd_[2];
    bool gotIn_{false};
    bool gotOut_{false};
};


class Example {
public:
    int x = 0;
    int y = 100;
    char z = 'z';
};

class BidirectionalStream {
public:
    BidirectionalStream() = default;

    int ForkStream() {
        pid_ = fork();
        return pid_;
    }

    PipeInputStream GetInputStream() {
        if (pid_ == 0) {
            return first.getPipeInputStream();
        }
        return second.getPipeInputStream();
    }

    PipeOutputStream GetOutputStream() {
        if (pid_ == 0) {
            return second.getPipeOutputStream();
        }
        return first.getPipeOutputStream();
    }

    template<typename T>
    friend BidirectionalStream& operator << (BidirectionalStream& out, const T& data) {
        auto realout = out.GetOutputStream();
        realout << data;
        return out;
    }

    template<typename T>
    friend BidirectionalStream& operator >> (BidirectionalStream& in, T& data) {
        auto realin = in.GetInputStream();
        realin >> data;
        return in;
    }
private:
    int pid_{0};
    PipeStream first;
    PipeStream second;
};

int main() {
   /* PipeStream inter;
    write(0, "ihh", 3);

    char buff[100];
    read(1, buff, 10);
    int pid = fork();
    if (pid == 0) {

        auto in = inter.getPipeInputStream();

        Example t;
        in >> t;
        std::cout << t.y << t.z << std::endl;
        int x = 5;
        while(in >> x) {
            std::cout << x << std::endl;
        }

        in.close();

    } else {

        auto out = inter.getPipeOutputStream();

        Example x;
        x.y = 1024;
        x.z = 'i';
        out << x;

        out << 10 << 100 << 1000;

        out.close();
        wait(nullptr);

    } */
    BidirectionalStream bidir;
    if (bidir.ForkStream() == 0) {
        int x = 77777;
        bidir << x;
        bidir >> x;
        std::cout << x;
    } else {
        int z;
        bidir >> z;
        bidir << z+1;
        std::cout << z << std::endl;
        wait(nullptr);
    }
    return 0;
}
