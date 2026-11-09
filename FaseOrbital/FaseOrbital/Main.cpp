#include <iostream>
#include <string>
#include "Pruebas.h"
#include "ControladorLotes.h"
#include "VistaAllegro.h"

/*
 * Archivo: Main.cpp
 * Descripcion: Punto de entrada principal del proyecto Fase Orbital.
 * Revisa si el usuario paso parametros por linea de comandos para ejecutar
 * las pruebas unitarias de las estructuras de datos, simulaciones por lotes,
 * o consultas de ranking y estadisticas. Si no se pasan parametros,
 * inicializa y levanta la interfaz visual del juego con Allegro 5.
 */

/*
 * Funcion: main
 * Descripcion: Evalua los argumentos recibidos por consola y redirige el flujo
 * hacia el modo correspondiente (pruebas, lotes, ranking, estadisticas o juego grafico).
 * Entradas: argc (numero de argumentos), argv (arreglo de cadenas con los argumentos)
 * Salidas: 0 si la ejecucion concluye con exito, o -1 si ocurre un error al iniciar los graficos
 */
int main(int argc, char* argv[]) {
    // 1. Modo por consola: evaluamos banderas para pruebas, lotes, ranking o estadisticas
    if (argc > 1) {
        std::string primerArg = argv[1];
        if (primerArg == "--pruebas") {
            // Ejecuta la bateria completa de pruebas unitarias sobre nuestras estructuras
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
        if (config.modoLotes) {
            ControladorLotes::EjecutarModoLotes(config);
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
    }

    // 2. Modo grafico por defecto: iniciamos la ventana y recursos con Allegro 5
    VistaAllegro vista;
    if (!vista.Inicializar()) {
        std::cerr << "Error: No se pudo inicializar la interfaz gráfica de Allegro 5.\n";
        return -1;
    }

    // Arrancamos el bucle principal de la interfaz grafica
    vista.Ejecutar();
    return 0;
}
