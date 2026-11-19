#include "infrastructure/persistence/CsvResultRepository.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <utility>

namespace eda::infrastructure {

namespace {
std::ofstream openCsv(const std::string& dir, const std::string& file) {
    std::filesystem::create_directories(dir);
    std::ofstream out(std::filesystem::path(dir) / file);
    if (!out) throw std::runtime_error("No se pudo abrir " + file);
    out.setf(std::ios::fixed);
    out.precision(3);
    return out;
}
}  // namespace

CsvResultRepository::CsvResultRepository(std::string outputDir) : outputDir_(std::move(outputDir)) {}

void CsvResultRepository::saveMeasurements(const std::vector<application::Measurement>& measurements) {
    auto out = openCsv(outputDir_, "raw.csv");
    out << "structure,operation,n,order,repetition,ns_per_op,height\n";
    for (const auto& m : measurements) {
        out << m.structure << ',' << m.operation << ',' << m.n << ',' << toString(m.order) << ','
            << m.repetition << ',' << m.nsPerOp << ',' << m.height << '\n';
    }
}

void CsvResultRepository::saveSummaries(const std::vector<application::Summary>& summaries) {
    auto out = openCsv(outputDir_, "summary.csv");
    out << "structure,operation,n,order,repetitions,mean_ns,std_ns,mean_height\n";
    for (const auto& s : summaries) {
        out << s.structure << ',' << s.operation << ',' << s.n << ',' << toString(s.order) << ','
            << s.repetitions << ',' << s.meanNs << ',' << s.stdDevNs << ',' << s.meanHeight << '\n';
    }
}

}  // namespace eda::infrastructure
