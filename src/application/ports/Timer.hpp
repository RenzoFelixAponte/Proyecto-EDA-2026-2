#pragma once

#include <cstdint>

namespace eda::application {

// Puerto de salida: reloj monotono en nanosegundos.
class Timer {
public:
    virtual ~Timer() = default;
    virtual std::int64_t nowNs() const = 0;
};

}  // namespace eda::application
