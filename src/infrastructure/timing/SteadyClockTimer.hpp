#pragma once

#include <chrono>

#include "application/ports/Timer.hpp"

namespace eda::infrastructure {

// Adaptador: reloj basado en std::chrono::steady_clock.
class SteadyClockTimer final : public application::Timer {
public:
    std::int64_t nowNs() const override {
        using namespace std::chrono;
        return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
    }
};

}  // namespace eda::infrastructure
