#pragma once

#include <string>
#include "Globales.h"

// Estructura que almacena los detalles de una accion o suceso ocurrido en el juego
struct Evento {
    int tick;
    int oleada;
    std::string jugador;
    TipoEvento tipoEvento;
    TipoEnemigo tipoEnemigo;
    Color color;
    int valor;
};

// Nodo simple para encadenar cronologicamente los eventos
struct NodoEvento {
    Evento evento;
    NodoEvento* siguiente = NULL;
};

// Lista simplemente enlazada con punteros a cabeza y cola para registrar la cronologia de la partida
class BitacoraEventos {
private:
    NodoEvento* cabeza;
    NodoEvento* cola;
    int tam;

public:
    BitacoraEventos();
    ~BitacoraEventos();

    // Regla de Tres: eliminamos copia y asignacion para evitar duplicar nodos de eventos
    BitacoraEventos(const BitacoraEventos&) = delete;
    BitacoraEventos& operator=(const BitacoraEventos&) = delete;

    void AgregarFinal(const Evento& e);
    void ImprimirLineaTiempo(const std::string& aliasJugador) const;

    NodoEvento* ObtenerCabeza() const { return cabeza; }
    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == NULL; }
};