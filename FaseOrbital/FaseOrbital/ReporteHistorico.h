#pragma once
#include <string>
#include "HistorialPartidas.h"
#include "Ranking.h"
#include "Globales.h"

// Modulo para procesar y estructurar las estadisticas de la seccion 6.3 y 6.4 para la interfaz grafica

const int MAX_ALIAS_HISTORIAL = 50;

// Estructura que empaqueta las metricas calculadas de un jugador para mostrarlas en pantalla
struct ReporteJugador {
    std::string alias;
    bool esBot = false;
    int partidas = 0;
    int victorias = 0;
    int mejor = 0;
    int peor = 0;
    double media = 0.0;
    double mediana = 0.0;       // Calculada con Merge Sort sobre ListaPuntajes
    double desviacion = 0.0;    // Desviacion estandar poblacional
    double precision = 0.0;     // correctos / (correctos + erroneos)
    int oleadaMaxima = 0;
    int bajasTipo[6] = { 0, 0, 0, 0, 0, 0 };
    int histograma[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 10 intervalos entre peor y mejor puntaje
};

// Extrae los nombres unicos de jugadores presentes en el historial CSV
int ObtenerAliasHistorial(const HistorialPartidas& historial, std::string destino[], int maximo);

// Calcula la media, mediana (con Merge Sort), desviacion e histograma de un participante
bool CalcularReporteJugador(const HistorialPartidas& historial, const std::string& alias, ReporteJugador& r);

// Inserta ordenadamente todos los registros del historial en una instancia de Ranking
void ConstruirRanking(const HistorialPartidas& historial, Ranking& ranking);
