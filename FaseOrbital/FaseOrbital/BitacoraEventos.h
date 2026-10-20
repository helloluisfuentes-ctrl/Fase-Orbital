#pragma once

#include <string>
#include "Globales.h"

struct Evento {
    int tick;
    int oleada;
    std::string jugador;
    TipoEvento tipoEvento;
    TipoEnemigo tipoEnemigo;
    Color color;
    int valor;
};

struct NodoEvento {
    Evento evento;
    NodoEvento* siguiente = NULL;
};

class BitacoraEventos {
private:
    NodoEvento* cabeza;
    NodoEvento* cola;
    int tam;

public:
    BitacoraEventos();
    ~BitacoraEventos();

    // Regla de Tres, bloqueamos copia y asignacion con = delete , no la ocupamos
    BitacoraEventos(const BitacoraEventos&) = delete;
    BitacoraEventos& operator=(const BitacoraEventos&) = delete;

    void AgregarFinal(const Evento& e);
    void ImprimirLineaTiempo(const std::string& aliasJugador) const;

    NodoEvento* ObtenerCabeza() const { return cabeza; }
    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == NULL; }
};