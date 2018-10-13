#ifndef FPSCAL_H
#define FPSCAL_H

#include <chrono>


class FpsCal {
public:
    FpsCal() {
        last_ = std::chrono::high_resolution_clock::now();
    }
    int fps(int batch_size) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> delay = now - last_;
        last_ = now;
        return 1000 / delay.count() * batch_size;
    }
private:
    std::chrono::time_point<std::chrono::system_clock> last_;
};  // class FpsCal

#endif // FPSCAL_H
