// Composition root: el unico lugar que conoce los adaptadores concretos.
#include <exception>
#include <iostream>
#include <memory>

#include "application/services/BenchmarkService.hpp"
#include "infrastructure/cli/CommandLineParser.hpp"
#include "infrastructure/cli/ConsoleProgressListener.hpp"
#include "infrastructure/data/RandomDatasetGenerator.hpp"
#include "infrastructure/factories/AvlTreeFactory.hpp"
#include "infrastructure/factories/SkipListFactory.hpp"
#include "infrastructure/persistence/CsvResultRepository.hpp"
#include "infrastructure/timing/SteadyClockTimer.hpp"

using namespace eda;

int main(int argc, char** argv) {
    try {
        const auto options = infrastructure::CommandLineParser{}.parse(argc, argv);
        if (options.showHelp) {
            std::cout << infrastructure::CommandLineParser::usage();
            return 0;
        }

        application::BenchmarkService service(
            {std::make_shared<infrastructure::SkipListFactory>(options.p, options.maxLevel),
             std::make_shared<infrastructure::AvlTreeFactory>()},
            std::make_shared<infrastructure::RandomDatasetGenerator>(),
            std::make_shared<infrastructure::SteadyClockTimer>(),
            std::make_shared<infrastructure::CsvResultRepository>(options.outputDir),
            std::make_shared<infrastructure::ConsoleProgressListener>());

        service.run(options.config);
        std::cerr << "Resultados en " << options.outputDir << "/summary.csv\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n' << infrastructure::CommandLineParser::usage();
        return 1;
    }
}
