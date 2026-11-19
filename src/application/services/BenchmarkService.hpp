#pragma once

#include <memory>
#include <vector>

#include "application/model/BenchmarkConfig.hpp"
#include "application/model/Measurement.hpp"
#include "application/ports/DatasetGenerator.hpp"
#include "application/ports/OrderedSetFactory.hpp"
#include "application/ports/ProgressListener.hpp"
#include "application/ports/ResultRepository.hpp"
#include "application/ports/Timer.hpp"
#include "application/services/StatisticsCalculator.hpp"

namespace eda::application {

// Caso de uso principal: ejecuta el experimento de la Figura 4 para cada
// estructura, tamano y orden, y entrega los resultados al repositorio.
class BenchmarkService {
public:
    BenchmarkService(std::vector<std::shared_ptr<OrderedSetFactory>> factories,
                     std::shared_ptr<DatasetGenerator> generator,
                     std::shared_ptr<Timer> timer,
                     std::shared_ptr<ResultRepository> repository,
                     std::shared_ptr<ProgressListener> progress);

    void run(const BenchmarkConfig& config);

private:
    // Inserta, busca y elimina; agrega 3 mediciones. Si record es false
    // (calentamiento) no guarda nada.
    void runOnce(const OrderedSetFactory& factory, const Dataset& data, std::size_t n,
                 DataOrder order, int repetition, std::uint64_t seed, bool record);
    double measureNsPerOp(std::int64_t start, std::size_t ops) const;

    std::vector<std::shared_ptr<OrderedSetFactory>> factories_;
    std::shared_ptr<DatasetGenerator> generator_;
    std::shared_ptr<Timer> timer_;
    std::shared_ptr<ResultRepository> repository_;
    std::shared_ptr<ProgressListener> progress_;
    StatisticsCalculator statistics_;
    std::vector<Measurement> measurements_;
};

}  // namespace eda::application
