#pragma once
#include <chrono>

class Timer
{
public:
    Timer() {
        Reset();
    }

    inline double GetTime() {
        return
            std::chrono::duration_cast<std::chrono::duration<double>>
            (std::chrono::high_resolution_clock::now() - basic_time).count();
    }

    inline void Reset() {
        basic_time = std::chrono::high_resolution_clock::now();
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> basic_time;
};

extern Timer gTime;