#include "infrastructure/cli/CommandLineParser.hpp"

#include <sstream>
#include <stdexcept>
#include <vector>

namespace eda::infrastructure {

namespace {
std::vector<std::string> splitCsv(const std::string& text) {
    std::vector<std::string> parts;
    std::stringstream ss(text);
    for (std::string item; std::getline(ss, item, ',');) {
        if (!item.empty()) parts.push_back(item);
    }
    return parts;
}
}  // namespace

std::string CommandLineParser::usage() {
    return "Uso: benchmark [opciones]\n"
           "  --sizes 1000,10000,...   tamanos n (por defecto 1e3..1e6)\n"
           "  --orders aleatorio,ordenado,inverso\n"
           "  --reps 10                repeticiones por escenario\n"
           "  --warmup 1               corridas de calentamiento descartadas\n"
           "  --seed 2026              semilla base\n"
           "  --p 0.5                  probabilidad de la Skip List\n"
           "  --max-level 20           MaxLevel de la Skip List\n"
           "  --out results            directorio de salida (raw.csv, summary.csv)\n";
}

CliOptions CommandLineParser::parse(int argc, char** argv) const {
    CliOptions options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            options.showHelp = true;
            continue;
        }
        if (i + 1 >= argc) throw std::invalid_argument("Falta valor para " + arg);
        const std::string value = argv[++i];

        if (arg == "--sizes") {
            options.config.sizes.clear();
            for (const auto& s : splitCsv(value)) options.config.sizes.push_back(std::stoull(s));
        } else if (arg == "--orders") {
            options.config.orders.clear();
            for (const auto& s : splitCsv(value))
                options.config.orders.push_back(application::dataOrderFromString(s));
        } else if (arg == "--reps") {
            options.config.repetitions = std::stoi(value);
        } else if (arg == "--warmup") {
            options.config.warmupRuns = std::stoi(value);
        } else if (arg == "--seed") {
            options.config.baseSeed = std::stoull(value);
        } else if (arg == "--p") {
            options.p = std::stod(value);
        } else if (arg == "--max-level") {
            options.maxLevel = std::stoi(value);
        } else if (arg == "--out") {
            options.outputDir = value;
        } else {
            throw std::invalid_argument("Opcion desconocida: " + arg);
        }
    }
    return options;
}

}  // namespace eda::infrastructure
