#pragma once
#include "Globales.h"
#include "ListaEnemigos.h"

// Estructura que representa la decisión del bot en cada tick
struct AccionTick {
    double anguloObjetivo; // Ángulo al que desea apuntar
    Color colorObjetivo;   // Color al que desea cambiar
    bool activarPulso;     // true si decide activar el pulso
};

class Bot {
public:
    // Toma la decisión para el tick actual basándose en el estado de la nave y los enemigos
    static AccionTick DecidirAccion(double anguloActual, Color colorActual, int cargaPulso, const ListaEnemigos& listaEnemigos);
};