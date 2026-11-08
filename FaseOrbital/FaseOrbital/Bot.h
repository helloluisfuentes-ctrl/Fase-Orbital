#pragma once
#include "Globales.h"
#include "ListaEnemigos.h"

// Estructura que almacena la orden o decision del jugador (o bot) para el tick actual
struct AccionTick {
    double anguloObjetivo; // Angulo en grados al que debe girar la nave
    Color colorObjetivo;   // Color al que se desea cambiar la fase
    bool activarPulso;     // true si se decide disparar la onda de pulso
};

// Modulo de inteligencia artificial que implementa la heuristica fija oficial (seccion 5.2)
class Bot {
public:
    /*
     * Funcion: DecidirAccion
     * Descripcion: Evalua los enemigos en la lista y determina la accion optima del tick:
     *              apunta al enemigo con menor r (distancia > 100) y activa pulso si hay peligro.
     * Entradas: double anguloActual, Color colorActual, int cargaPulso, const ListaEnemigos& listaEnemigos
     * Salidas: Retorna una estructura AccionTick con las decisiones tomadas
     */
    static AccionTick DecidirAccion(double anguloActual, Color colorActual, int cargaPulso, const ListaEnemigos& listaEnemigos);
};