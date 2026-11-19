#include "application/services/StatisticsCalculator.hpp"

#include <cmath>
#include <map>
#include <tuple>

namespace eda::application {

std::vector<Summary> StatisticsCalculator::summarize(
    const std::vector<Measurement>& measurements) const {
    using GroupKey = std::tuple<std::string, std::string, std::size_t, int>;
    std::map<GroupKey, std::vector<const Measurement*>> groups;
    for (const auto& m : measurements) {
        groups[{m.structure, m.operation, m.n, static_cast<int>(m.order)}].push_back(&m);
    }

    std::vector<Summary> summaries;
    summaries.reserve(groups.size());
    for (const auto& [key, group] : groups) {
        Summary s;
        s.structure = std::get<0>(key);
        s.operation = std::get<1>(key);
        s.n = std::get<2>(key);
        s.order = static_cast<DataOrder>(std::get<3>(key));
        s.repetitions = static_cast<int>(group.size());

        double sum = 0.0, heightSum = 0.0;
        for (const auto* m : group) {
            sum += m->nsPerOp;
            heightSum += static_cast<double>(m->height);
        }
        s.meanNs = sum / s.repetitions;
        s.meanHeight = heightSum / s.repetitions;

        double squares = 0.0;
        for (const auto* m : group) squares += (m->nsPerOp - s.meanNs) * (m->nsPerOp - s.meanNs);
        s.stdDevNs = s.repetitions > 1 ? std::sqrt(squares / (s.repetitions - 1)) : 0.0;
        summaries.push_back(s);
    }
    return summaries;
}

}  // namespace eda::application
