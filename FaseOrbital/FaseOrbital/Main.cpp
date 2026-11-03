
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include "ListaBalas.h"
#include "Globales.h"
#include "Pruebas.h"
#include "ControladorLotes.h"
#include <iostream>



int main(int argc, char* argv[]) {
    // Si se pasaron banderas por consola
    if (argc > 1) {
        ConfiguracionLotes config = ControladorLotes::ParsearArgumentos(argc, argv);

        if (config.verRanking) {
            ControladorLotes::MostrarRankingConsola(config.rutaCSV);
            return 0;
        }
        if (config.verEstadisticas) {
            ControladorLotes::MostrarEstadisticasConsola(config.rutaCSV);
            return 0;
        }
        if (config.modoLotes) {
            std::cout << "Modo por lotes detectado. Jugadores: " << config.totalJugadores
                << ", Semilla: " << config.semilla << "\n";
            // Cuando Marco termine el Motor, lo llamamos aquí
            return 0;
        }
    }

    // Si no hay argumentos, corre las pruebas o abre Allegro
    // EjecutarTodasLasPruebas();
    return 0;
}