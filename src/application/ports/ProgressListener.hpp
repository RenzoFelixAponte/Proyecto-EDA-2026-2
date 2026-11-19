#pragma once

#include <string>

namespace eda::application {

// Puerto de salida: notifica el avance (se usa fuera de las zonas medidas).
class ProgressListener {
public:
    virtual ~ProgressListener() = default;
    virtual void onScenarioDone(const std::string& description) = 0;
};

}  // namespace eda::application
