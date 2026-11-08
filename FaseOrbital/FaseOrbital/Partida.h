#pragma once
#include <string>
#include <cstdint>
#include "RondaJugadores.h"
#include "ColaSpawn.h"
#include "Oleada.h"

class Partida {
private:
    uint32_t semilla;
    int oleadaActual;
    int limiteOleadas;
    int totalJugadoresIniciales;
    
    RondaJugadores ronda;
    ColaSpawn colaMaestraOleada;
    
    bool sentidoHorario;      // true = horario , false = antihorario 
    int jugadoresTurnoRestantes; // Cuántos jugadores faltan por jugar en la ronda actual
    bool terminada;

public:
    Partida(uint32_t semillaPartida, int maxOleadas = 10);
    ~Partida();

    // Registro de participantes 
    void RegistrarJugador(Jugador* j);

    // Flujo de la partida
    void IniciarPartida();
    void IniciarNuevaRonda();
    
    // Gestión del turno actual
    Jugador* ObtenerJugadorEnTurno() const;
    ColaSpawn ObtenerCopiaOleadaActual() const; // Usa constructor de copia profunda
    void PrepararInicioTurno(Jugador* j);
    void FinalizarTurnoActual(bool sobrevivio);

    // getters
    bool EstaTerminada() const { return terminada; }
    int ObtenerOleadaActual() const { return oleadaActual; }
    bool EsSentidoHorario() const { return sentidoHorario; }
    int JugadoresActivos() const { return ronda.Tamano(); }

    // Determina el ganador según las reglas oficiales de desempate
    Jugador* DeterminarGanador(Jugador* todosLosRegistrados[], int totalRegistrados) const;
};