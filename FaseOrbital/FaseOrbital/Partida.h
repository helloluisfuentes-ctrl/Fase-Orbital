#pragma once
#include <string>
#include <cstdint>
#include "RondaJugadores.h"
#include "ColaSpawn.h"
#include "Oleada.h"

// Clase controladora del flujo de rondas, turnos serpenteantes y estado general de la partida
class Partida {
private:
    uint32_t semilla;
    int oleadaActual;
    int limiteOleadas;
    int totalJugadoresIniciales;
    
    RondaJugadores ronda;
    ColaSpawn colaMaestraOleada;
    
    bool sentidoHorario;        // true = horario (siguiente), false = antihorario (anterior)
    int jugadoresTurnoRestantes; // Conteo de cuantos jugadores vivos faltan por jugar en la ronda actual
    bool terminada;

public:
    Partida(uint32_t semillaPartida, int maxOleadas = 10);
    ~Partida();

    // Registro de participantes
    void RegistrarJugador(Jugador* j);

    // Flujo general de la partida
    void IniciarPartida();
    void IniciarNuevaRonda();
    
    // Gestion del turno actual
    Jugador* ObtenerJugadorEnTurno() const;
    ColaSpawn ObtenerCopiaOleadaActual() const; // Utiliza constructor de copia profunda
    void PrepararInicioTurno(Jugador* j);
    void FinalizarTurnoActual(bool sobrevivio);

    // Metodos de consulta (getters)
    bool EstaTerminada() const { return terminada; }
    int ObtenerOleadaActual() const { return oleadaActual; }
    bool EsSentidoHorario() const { return sentidoHorario; }
    int JugadoresActivos() const { return ronda.Tamano(); }

    // Determina el ganador segun las reglas de desempate
    Jugador* DeterminarGanador(Jugador* todosLosRegistrados[], int totalRegistrados) const;
};