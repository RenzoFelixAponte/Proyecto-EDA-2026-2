#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace eda::domain {

using Key = std::int64_t;

// Puerto del dominio: contrato comun para cualquier conjunto ordenado dinamico.
// La Skip List y el AVL Tree lo implementan; el resto del sistema solo depende
// de esta interfaz, nunca de una estructura concreta.
class OrderedSet {
public:
    virtual ~OrderedSet() = default;

    // Inserta la clave. Devuelve false si ya existia.
    virtual bool insert(Key key) = 0;
    // Devuelve true si la clave esta en el conjunto.
    virtual bool search(Key key) const = 0;
    // Elimina la clave. Devuelve false si no existia.
    virtual bool remove(Key key) = 0;

    virtual std::size_t size() const = 0;
    // Altura del AVL o nivel maximo alcanzado en la Skip List.
    virtual std::size_t height() const = 0;
    // Verifica los invariantes internos (orden, balance, enlaces).
    virtual bool checkInvariants() const = 0;
    virtual std::string name() const = 0;
};

}  // namespace eda::domain
