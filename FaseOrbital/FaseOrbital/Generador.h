#pragma once
#include <cstdint>

class Generador {
private:
    uint32_t x;

public:
    explicit Generador(uint32_t semilla) : x(semilla) {}

    // Genera el siguiente número pseudoaleatorio determinista
    uint32_t Siguiente() {
        x = (1103515245u * x + 12345u) & 0x7FFFFFFFu;
        return x;
    }

    uint32_t ObtenerEstadoActual() const { return x; }
};

