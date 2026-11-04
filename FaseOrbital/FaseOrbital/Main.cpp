#include <iostream>
#include <string>
#include "Pruebas.h"
#include "ControladorLotes.h"
#include "VistaAllegro.h"

int main(int argc, char* argv[]) {
    // 1. Modo por consola (argumentos CLI: pruebas, ranking, estadisticas, lotes o script)
    if (argc > 1) {
        std::string primerArg = argv[1];
        if (primerArg == "--pruebas") {
            EjecutarTodasLasPruebas();
            return 0;
        }

        ConfiguracionLotes config = ControladorLotes::ParsearArgumentos(argc, argv);

        if (config.verRanking) {
            ControladorLotes::MostrarRankingConsola(config.rutaCSV);
            return 0;
        }
        if (config.verEstadisticas) {
            ControladorLotes::MostrarEstadisticasConsola(config.rutaCSV);
            return 0;
        }
        if (!config.rutaScript.empty()) {
            ListaAccionesScript listaAcciones;
            if (ControladorLotes::CargarScript(config.rutaScript, listaAcciones)) {
                std::cout << "\n=== SCRIPT CARGADO CON EXITO ===\n";
                std::cout << "Total de acciones: " << listaAcciones.Tamano() << "\n";
            }
            return 0;
        }
        if (config.modoLotes) {
            std::cout << "Modo por lotes detectado. Jugadores: " << config.totalJugadores
                      << ", Semilla: " << config.semilla << ", Oleadas: " << config.oleadas << "\n";
            return 0;
        }
    }

    // 2. Modo por defecto: Ejecutar la interfaz grafica completa con Allegro 5 (VistaAllegro)
    VistaAllegro vista;
    if (!vista.Inicializar()) {
        std::cerr << "Error: No se pudo inicializar la interfaz gráfica de Allegro 5.\n";
        return -1;
    }

    vista.Ejecutar();
    return 0;
}
