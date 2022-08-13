#pragma once

/*
 * Author: gongzhitaao
 * Code source: https://gist.github.com/gongzhitaao/7062087
 */

#include <iostream>
#include <chrono>

class Timer {
public:
    Timer() : beg_(clock_::now()) {}

    void reset() { beg_ = clock_::now(); }

    long long int elapsed() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>
                (clock_::now() - beg_).count();
    }

private:
    typedef std::chrono::high_resolution_clock clock_;
    std::chrono::time_point<clock_> beg_;
};
