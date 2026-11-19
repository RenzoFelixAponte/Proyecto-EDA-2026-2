#include "application/services/BenchmarkService.hpp"

#include <stdexcept>
#include <utility>

namespace eda::application {

namespace {
// Evita que el compilador elimine las busquedas cuyo resultado no se usa.
volatile std::size_t gSink = 0;
}  // namespace

BenchmarkService::BenchmarkService(std::vector<std::shared_ptr<OrderedSetFactory>> factories,
                                   std::shared_ptr<DatasetGenerator> generator,
                                   std::shared_ptr<Timer> timer,
                                   std::shared_ptr<ResultRepository> repository,
                                   std::shared_ptr<ProgressListener> progress)
    : factories_(std::move(factories)),
      generator_(std::move(generator)),
      timer_(std::move(timer)),
      repository_(std::move(repository)),
      progress_(std::move(progress)) {}

double BenchmarkService::measureNsPerOp(std::int64_t start, std::size_t ops) const {
    return static_cast<double>(timer_->nowNs() - start) / static_cast<double>(ops);
}

void BenchmarkService::runOnce(const OrderedSetFactory& factory, const Dataset& data,
                               std::size_t n, DataOrder order, int repetition,
                               std::uint64_t seed, bool record) {
    auto set = factory.create(seed);

    std::int64_t start = timer_->nowNs();
    for (auto key : data.insertKeys) set->insert(key);
    const double insertNs = measureNsPerOp(start, data.insertKeys.size());

    if (set->size() != n) throw std::logic_error(factory.name() + ": tamano incorrecto");
    const std::size_t height = set->height();

    std::size_t found = 0;
    start = timer_->nowNs();
    for (auto key : data.queries) found += set->search(key) ? 1 : 0;
    const double searchNs = measureNsPerOp(start, data.queries.size());
    gSink = gSink + found;

    start = timer_->nowNs();
    for (auto key : data.insertKeys) set->remove(key);
    const double removeNs = measureNsPerOp(start, data.insertKeys.size());

    if (set->size() != 0) throw std::logic_error(factory.name() + ": quedaron claves");
    if (!record) return;

    const std::string name = factory.name();
    measurements_.push_back({name, "insert", n, order, repetition, insertNs, height});
    measurements_.push_back({name, "search", n, order, repetition, searchNs, height});
    measurements_.push_back({name, "remove", n, order, repetition, removeNs, height});
}

void BenchmarkService::run(const BenchmarkConfig& config) {
    measurements_.clear();
    for (std::size_t n : config.sizes) {
        for (DataOrder order : config.orders) {
            for (int rep = -config.warmupRuns; rep < config.repetitions; ++rep) {
                const std::uint64_t seed = config.baseSeed + 7919ULL * (rep + config.warmupRuns)
                                           + 104729ULL * n + static_cast<std::uint64_t>(order);
                // Ambas estructuras reciben exactamente el mismo dataset.
                const Dataset data = generator_->generate(n, order, seed);
                for (const auto& factory : factories_) {
                    runOnce(*factory, data, n, order, rep, seed, rep >= 0);
                }
            }
            if (progress_) {
                progress_->onScenarioDone("n=" + std::to_string(n) + " orden=" + toString(order));
            }
        }
    }
    repository_->saveMeasurements(measurements_);
    repository_->saveSummaries(statistics_.summarize(measurements_));
}

}  // namespace eda::application
