#pragma once
#include <cstdint>

// Generador congruencial lineal (LCG) determinista obligatorio por especificacion (seccion 3.6)
// Formula: x = (1103515245 * x + 12345) & 0x7FFFFFFF
class Generador {
private:
    uint32_t x; // Estado interno / semilla

public:
    /*
     * Funcion: Constructor Generador
     * Descripcion: Inicializa el generador con una semilla base entera sin signo de 32 bits.
     * Entradas: uint32_t semilla
     * Salidas: Instancia creada
     */
    explicit Generador(uint32_t semilla) : x(semilla) {}

    /*
     * Funcion: Siguiente
     * Descripcion: Aplica la formula del LCG para calcular el siguiente numero pseudoaleatorio.
     * Entradas: Ninguna
     * Salidas: Retorna el nuevo valor de x
     */
    uint32_t Siguiente() {
        x = (1103515245u * x + 12345u) & 0x7FFFFFFFu;
        return x;
    }

    /*
     * Funcion: ObtenerEstadoActual
     * Descripcion: Consulta el valor actual del estado interno sin avanzar el generador.
     * Entradas: Ninguna
     * Salidas: uint32_t estado actual
     */
    uint32_t ObtenerEstadoActual() const { return x; }
};

