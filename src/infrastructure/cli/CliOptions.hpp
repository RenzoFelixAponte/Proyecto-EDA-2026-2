#pragma once

#include <string>

#include "application/model/BenchmarkConfig.hpp"

namespace eda::infrastructure {

// Opciones de la linea de comandos ya interpretadas.
class CliOptions {
public:
    application::BenchmarkConfig config;
    std::string outputDir = "results";
    double p = 0.5;
    int maxLevel = 20;
    bool showHelp = false;
};

}  // namespace eda::infrastructure
