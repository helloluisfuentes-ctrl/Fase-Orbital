#pragma once
#include <string>
#include "Globales.h"

using namespace std;

// Estructura que mantiene el estado de cada participante en la partida
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

// Nodo para la lista circular doble (enlaza al siguiente y al anterior)
struct NodoJugador {
    Jugador* jugador;
    NodoJugador* anterior = NULL;
    NodoJugador* siguiente = NULL;
};

// Lista circular doblemente enlazada para manejar los turnos serpenteantes de los jugadores
class RondaJugadores {
private:
    NodoJugador* actual;
    int tam;

public:
    RondaJugadores();
    ~RondaJugadores();

    // Regla de Tres: eliminamos copia y asignacion para evitar duplicar referencias de turnos
    RondaJugadores(const RondaJugadores&) = delete;
    RondaJugadores& operator=(const RondaJugadores&) = delete;

    // Operaciones principales
    void Agregar(Jugador* j);                  // Agrega antes de actual (al "final" de la ronda circular)
    void Avanzar(bool horario);               // Avanza segun el sentido: true = siguiente, false = anterior
    Jugador* EliminarActual(bool horario);     // Elimina al jugador actual y mueve 'actual' segun el sentido

    // Consultas
    Jugador* VerActual() const;
    int Tamano() const { return tam; }
    bool Vacia() const { return actual == NULL; }
};