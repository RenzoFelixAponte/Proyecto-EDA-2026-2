#pragma once

#include <string>

#include "infrastructure/cli/CliOptions.hpp"

namespace eda::infrastructure {

// Adaptador de entrada: traduce argv a CliOptions.
class CommandLineParser {
public:
    CliOptions parse(int argc, char** argv) const;
    static std::string usage();
};

}  // namespace eda::infrastructure
