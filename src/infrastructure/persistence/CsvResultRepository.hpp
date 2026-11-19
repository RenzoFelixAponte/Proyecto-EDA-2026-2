#pragma once

#include <string>

#include "application/ports/ResultRepository.hpp"

namespace eda::infrastructure {

// Adaptador: escribe raw.csv y summary.csv en un directorio.
class CsvResultRepository final : public application::ResultRepository {
public:
    explicit CsvResultRepository(std::string outputDir);

    void saveMeasurements(const std::vector<application::Measurement>& measurements) override;
    void saveSummaries(const std::vector<application::Summary>& summaries) override;

private:
    std::string outputDir_;
};

}  // namespace eda::infrastructure
