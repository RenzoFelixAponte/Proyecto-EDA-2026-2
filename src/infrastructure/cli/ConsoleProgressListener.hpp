#pragma once

#include <iostream>

#include "application/ports/ProgressListener.hpp"

namespace eda::infrastructure {

// Adaptador: imprime el avance en stderr al terminar cada escenario.
class ConsoleProgressListener final : public application::ProgressListener {
public:
    void onScenarioDone(const std::string& description) override {
        std::cerr << "[ok] " << description << '\n';
    }
};

}  // namespace eda::infrastructure
