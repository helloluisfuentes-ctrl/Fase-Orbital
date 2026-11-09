#pragma once
#include <string>
#include "HistorialPartidas.h"
#include "Ranking.h"
#include "Globales.h"

// Calculos de la seccion 6.3 y 6.4 a partir de historial.csv, devueltos en estructuras
// para que la interfaz los pueda dibujar (HistorialPartidas::GenerarReporteJugador solo imprime en consola).
// No depende de Allegro.

const int MAX_ALIAS_HISTORIAL = 50;

struct ReporteJugador {
    std::string alias;
    bool esBot = false;
    int partidas = 0;
    int victorias = 0;
    int mejor = 0;
    int peor = 0;
    double media = 0.0;
    double mediana = 0.0;       // merge sort sobre ListaPuntajes
    double desviacion = 0.0;    // poblacional
    double precision = 0.0;     // correctos / (correctos + erroneos), de 0 a 1
    int oleadaMaxima = 0;
    int bajasTipo[6] = { 0, 0, 0, 0, 0, 0 };
    int histograma[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 10 intervalos iguales entre peor y mejor
};

// llena destino con los alias distintos del historial, en orden de aparicion. Devuelve cuantos hay
int ObtenerAliasHistorial(const HistorialPartidas& historial, std::string destino[], int maximo);

// calcula el reporte de un jugador, devuelve false si no tiene partidas
bool CalcularReporteJugador(const HistorialPartidas& historial, const std::string& alias, ReporteJugador& r);

// llena el ranking con insercion ordenada usando todas las filas del historial
void ConstruirRanking(const HistorialPartidas& historial, Ranking& ranking);
