#include "Partida.h"
#include <iostream>

Partida::Partida(uint32_t semillaPartida, int maxOleadas) {
    semilla = semillaPartida;
    oleadaActual = 1;
    limiteOleadas = maxOleadas;
    sentidoHorario = true; // Rondas impares arrancan en sentido horario
    jugadoresTurnoRestantes = 0;
    terminada = false;
}

Partida::~Partida() {
    // La ronda libera sus propios nodos al destruirse
}

void Partida::RegistrarJugador(Jugador* j) {
    ronda.Agregar(j);
}

void Partida::IniciarPartida() {
    if (ronda.Vacia()) {
        terminada = true;
        return;
    }
    IniciarNuevaRonda();
}

void Partida::IniciarNuevaRonda() {
    if (oleadaActual > limiteOleadas || ronda.Vacia()) {
        terminada = true;
        return;
    }

    //En rondas impares horario (true), en rondas pares antihorario (false)
    sentidoHorario = (oleadaActual % 2 != 0);

    // Se genera la ColaSpawn de la oleada una sola vez
    // Vaciamos cualquier residuo anterior y generamos la nueva
    while (!colaMaestraOleada.Vacia()) {
        Spawn basura;
        colaMaestraOleada.Desencolar(basura);
    }
    GenerarOleada(semilla, oleadaActual, colaMaestraOleada);

    // En esta ronda deben jugar todos los jugadores que sigan vivos
    jugadoresTurnoRestantes = ronda.Tamano();
}

Jugador* Partida::ObtenerJugadorEnTurno() const {
    return ronda.VerActual();
}

// Cada jugador recibe una copia profunda independiente
ColaSpawn Partida::ObtenerCopiaOleadaActual() const {
    ColaSpawn copia(colaMaestraOleada); // Constructor de copia profunda
    return copia;
}

void Partida::PrepararInicioTurno(Jugador* j) {
    if (j == NULL) return;

    // El combo se reinicia al inicio de cada turno; 
    // la vida del núcleo y la carga de pulso se conservan.
    j->combo = 0;
    j->anguloNave = 270.0; // Ángulo inicial 
}

void Partida::FinalizarTurnoActual(bool sobrevivio) {
    if (ronda.Vacia()) {
        terminada = true;
        return;
    }

    Jugador* jActual = ronda.VerActual();

    // Si el núcleo llega a 0, se elimina de la ronda
    if (!sobrevivio || (jActual != NULL && jActual->vidaNucleo <= 0)) {
        ronda.EliminarActual(sentidoHorario);
    } 
    else {
        // Si no es el último turno de la ronda, avanzamos al siguiente jugador
        if (jugadoresTurnoRestantes > 1) {
            ronda.Avanzar(sentidoHorario);
        }
        // Cuando jugadoresTurnoRestantes == 1 (el último de la ronda),
        // NO avanzamos, así la siguiente ronda arranca exactamente con el último jugador.
    }

    jugadoresTurnoRestantes--;

    // Si todos los jugadores activos ya jugaron esta oleada:
    if (jugadoresTurnoRestantes <= 0) {
        // La partida termina si queda <= 1 jugador tras completar una ronda
        if (ronda.Tamano() <= 1 && oleadaActual > 1) {
            terminada = true;
            return;
        }

        oleadaActual++;
        if (oleadaActual > limiteOleadas || ronda.Vacia()) {
            terminada = true;
        } else {
            IniciarNuevaRonda();
        }
    }
}

// Criterios de desempate:
// 1. Mayor puntaje
// 2. Mayor oleada alcanzada
// 3. Mayor precisión
Jugador* Partida::DeterminarGanador(Jugador* todosLosRegistrados[], int totalRegistrados) const {
    if (totalRegistrados <= 0) return NULL;

    Jugador* ganador = todosLosRegistrados[0];

    for (int i = 1; i < totalRegistrados; i++) {
        Jugador* cand = todosLosRegistrados[i];

        if (cand->puntaje > ganador->puntaje) {
            ganador = cand;
        }
        // En caso de empate en puntaje, desempatar por vida restante de núcleo
        else if (cand->puntaje == ganador->puntaje) {
            if (cand->vidaNucleo > ganador->vidaNucleo) {
                ganador = cand;
            }
        }
    }

    return ganador;
}