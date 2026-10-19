#pragma once
#include <string>
#include "Globales.h"

using namespace std;

struct Jugador {
    string alias;
    bool esBot;
    int vidaNucleo = 100;
    int puntaje = 0;
    int combo = 0;
    int cargaPulso = 0;
    Color faseColor = CIAN;
    double anguloNave = 270.0; 
};

struct NodoJugador {
    Jugador* jugador;
    NodoJugador* anterior = NULL;
    NodoJugador* siguiente = NULL;
};

class RondaJugadores {
private:
    NodoJugador* actual;
    int tam;

public:
    RondaJugadores();
    ~RondaJugadores();

    //delete porque no es necesario copiar rondas
    RondaJugadores(const RondaJugadores&) = delete; //manejador de copias
    RondaJugadores& operator=(const RondaJugadores&) = delete; //operador de asignacion

    // Operaciones principales F
    void Agregar(Jugador* j);                  // Agrega antes de actual (al "final" de la ronda)
    void Avanzar(bool horario);               // avanza hacia true = siguiente, false = anterior
    Jugador* EliminarActual(bool horario);     // Elimina al jugador actual y mueve 'actual' al siguiente según sentido, 

    // Consultas
    Jugador* VerActual() const;
    int Tamano() const { return tam; }
    bool Vacia() const { return actual == NULL; }
};