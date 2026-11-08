#include "ControladorLotes.h"
#include "HistorialPartidas.h"
#include "Ranking.h"
#include "SesionJuego.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

// ==========================================
// METODOS DE ListaAccionesScript
// ==========================================

/*
 * Funcion: Constructor ListaAccionesScript
 * Descripcion: Inicializa la lista simplemente enlazada de acciones vacia, con cabeza y cola en NULL y tamano en cero.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
ListaAccionesScript::ListaAccionesScript() {
    cabeza = NULL;
    cola = NULL;
    tam = 0;
}

/*
 * Funcion: Destructor ~ListaAccionesScript
 * Descripcion: Recorre y libera la memoria de todos los nodos de comandos cargados desde el archivo de script.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
ListaAccionesScript::~ListaAccionesScript() {
    NodoAccionScript* aux = cabeza;
    while (aux != NULL) {
        cabeza = cabeza->siguiente;
        delete aux;
        aux = cabeza;
    }
    cola = NULL;
    tam = 0;
}

/*
 * Funcion: AgregarFinal
 * Descripcion: Inserta un nuevo comando al final de la lista enlazada en O(1) usando el puntero cola.
 * Entradas: const AccionScript& acc (comando leido del archivo)
 * Salidas: Lista con la nueva accion agregada
 */
void ListaAccionesScript::AgregarFinal(const AccionScript& acc) {
    NodoAccionScript* nuevo = new NodoAccionScript;
    nuevo->dato = acc;
    nuevo->siguiente = NULL;

    if (cabeza == NULL) {
        cabeza = nuevo;
        cola = nuevo;
    }
    else {
        cola->siguiente = nuevo;
        cola = nuevo;
    }
    tam++;
}

/*
 * Funcion: BuscarAccion
 * Descripcion: Recorre la lista de acciones buscando si existe una orden programada para el jugador,
 *              oleada y tick indicados. Si la encuentra, la devuelve a traves de 'salida'.
 * Entradas: const std::string& jugador, int oleada, int tick, AccionScript& salida
 * Salidas: Retorna true si habia accion programada para ese tick, false en caso contrario
 */
bool ListaAccionesScript::BuscarAccion(const std::string& jugador, int oleada, int tick, AccionScript& salida) const {
    NodoAccionScript* aux = cabeza;
    while (aux != NULL) {
        if (aux->dato.jugador == jugador && aux->dato.oleada == oleada && aux->dato.tick == tick) {
            salida = aux->dato;
            return true;
        }
        aux = aux->siguiente;
    }
    return false;
}

// ==========================================
// PARSER Y CONTROLADOR POR LOTES
// ==========================================

/*
 * Funcion: ParsearArgumentos
 * Descripcion: Lee el arreglo argv de la linea de comandos identificando banderas (--lotes, --ranking,
 *              --estadisticas, --semilla, --oleadas, --script, --jugadores) y rellena ConfiguracionLotes.
 * Entradas: int argc, char* argv[]
 * Salidas: Estructura ConfiguracionLotes poblada con los parametros recibidos
 */
ConfiguracionLotes ControladorLotes::ParsearArgumentos(int argc, char* argv[]) {
    ConfiguracionLotes config;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--ranking" && i + 1 < argc) {
            config.verRanking = true;
            config.rutaCSV = argv[++i];
        }
        else if (arg == "--estadisticas" && i + 1 < argc) {
            config.verEstadisticas = true;
            config.rutaCSV = argv[++i];
        }
        else if (arg == "--lotes") {
            config.modoLotes = true;
        }
        else if (arg == "--semilla" && i + 1 < argc) {
            config.semilla = static_cast<uint32_t>(std::stoul(argv[++i]));
        }
        else if (arg == "--oleadas" && i + 1 < argc) {
            config.oleadas = std::stoi(argv[++i]);
        }
        else if (arg == "--script" && i + 1 < argc) {
            config.rutaScript = argv[++i];
        }
        else if (arg == "--jugadores" && i + 1 < argc) {
            std::string lista = argv[++i];
            std::stringstream ss(lista);
            std::string nombre;
            // Parseamos los nombres separados por comas (hasta un maximo de 6 participantes)
            while (std::getline(ss, nombre, ',') && config.totalJugadores < 6) {
                if (!nombre.empty()) {
                    config.jugadores[config.totalJugadores++] = nombre;
                }
            }
        }
    }

    return config;
}

/*
 * Funcion: MostrarRankingConsola
 * Descripcion: Carga el archivo CSV en un HistorialPartidas, construye la lista enlazada ordenada Ranking
 *              aplicando insercion ordenada y criterios de desempate, e imprime el Top 10 en la terminal.
 * Entradas: const std::string& rutaCSV (ruta del archivo de persistencia)
 * Salidas: Imprime la tabla del Top 10 en consola
 */
void ControladorLotes::MostrarRankingConsola(const std::string& rutaCSV) {
    HistorialPartidas historial;
    if (!historial.CargarCSV(rutaCSV)) {
        std::cout << "Error: No se pudo abrir o cargar el archivo: " << rutaCSV << "\n";
        return;
    }

    Ranking ranking;
    NodoRegistro* aux = historial.ObtenerCabeza();
    while (aux != NULL) {
        const RegistroPartida& r = aux->registro;

        double precision = 0.0;
        int totalImpactos = r.correctos + r.erroneos;
        if (totalImpactos > 0) {
            precision = static_cast<double>(r.correctos) / totalImpactos;
        }

        EntradaRanking entrada;
        entrada.alias = r.jugador;
        entrada.puntaje = r.puntaje;
        entrada.precisionColor = precision;

        ranking.InsertarOrdenado(entrada);
        aux = aux->siguiente;
    }

    ranking.Top(10);
}

/*
 * Funcion: MostrarEstadisticasConsola
 * Descripcion: Carga el historial CSV, obtiene todos los participantes unicos y genera el reporte
 *              estadistico detallado (con Merge Sort) para cada uno de ellos.
 * Entradas: const std::string& rutaCSV
 * Salidas: Imprime los reportes individuales en consola
 */
void ControladorLotes::MostrarEstadisticasConsola(const std::string& rutaCSV) {
    HistorialPartidas historial;
    if (!historial.CargarCSV(rutaCSV)) {
        std::cout << "Error: No se pudo abrir o cargar el archivo: " << rutaCSV << "\n";
        return;
    }

    std::string jugadoresVistos[50];
    int totalVistos = 0;

    NodoRegistro* aux = historial.ObtenerCabeza();
    while (aux != NULL) {
        std::string alias = aux->registro.jugador;
        bool yaVisto = false;
        for (int i = 0; i < totalVistos; i++) {
            if (jugadoresVistos[i] == alias) {
                yaVisto = true;
                break;
            }
        }
        if (!yaVisto && totalVistos < 50) {
            jugadoresVistos[totalVistos++] = alias;
        }
        aux = aux->siguiente;
    }

    for (int i = 0; i < totalVistos; i++) {
        historial.GenerarReporteJugador(jugadoresVistos[i]);
    }
}

/*
 * Funcion: CargarScript
 * Descripcion: Lee un archivo de texto plano linea por linea ignorando comentarios con # y lineas vacias,
 *              extrayendo los campos (jugador, oleada, tick, comando y parametro opcional).
 * Entradas: const std::string& rutaArchivo, ListaAccionesScript& listaDestino
 * Salidas: Retorna true si pudo abrir y parsear el archivo, false si no se pudo abrir
 */
bool ControladorLotes::CargarScript(const std::string& rutaArchivo, ListaAccionesScript& listaDestino) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo de script: " << rutaArchivo << "\n";
        return false;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        // Ignoramos lineas vacias
        if (linea.empty()) continue;

        // Omitimos espacios en blanco al inicio para validar si es comentario
        size_t inicio = linea.find_first_not_of(" \t\r\n");
        if (inicio == std::string::npos) continue;

        // Las lineas que empiezan con # son comentarios segun la seccion 7.3
        if (linea[inicio] == '#') continue;

        std::stringstream ss(linea);
        AccionScript acc;
        ss >> acc.jugador >> acc.oleada >> acc.tick >> acc.comando;

        // El parametro (como angulo o color) es opcional segun el comando (ej: PULSO no lleva)
        if (!(ss >> acc.parametro)) {
            acc.parametro = "";
        }

        listaDestino.AgregarFinal(acc);
    }

    return true;
}

/*
 * Funcion: EjecutarModoLotes
 * Descripcion: Ejecuta una simulacion completa de la partida en consola (headless) sin abrir ventana
 *              de Allegro. Procesa los ticks a velocidad de CPU, alimenta acciones desde bots o script,
 *              muestra las lineas de tiempo requeridas (seccion 6.2) y persiste el resultado en el CSV.
 * Entradas: const ConfiguracionLotes& config
 * Salidas: Salida formateada en consola y actualizacion de historial.csv
 */
void ControladorLotes::EjecutarModoLotes(const ConfiguracionLotes& config) {
    SesionJuego sesion;

    // Si no se pasaron jugadores por parametro, usamos dos bots por defecto para pruebas
    if (config.totalJugadores == 0) {
        sesion.RegistrarJugador("BOT1", true);
        sesion.RegistrarJugador("BOT2", true);
    } else {
        for (int i = 0; i < config.totalJugadores; i++) {
            bool esBot = SesionJuego::EsAliasDeBot(config.jugadores[i]);
            sesion.RegistrarJugador(config.jugadores[i], esBot);
        }
    }

    // Cargamos el script si se proporciono la bandera --script
    ListaAccionesScript script;
    bool tieneScript = false;
    if (!config.rutaScript.empty()) {
        tieneScript = ControladorLotes::CargarScript(config.rutaScript, script);
    }

    // Inicializamos la sesion con la semilla y limite de oleadas
    sesion.Iniciar(config.semilla, config.oleadas);

    std::cout << "\n============================================\n";
    std::cout << " INICIANDO PARTIDA EN MODO POR LOTES\n";
    std::cout << " Semilla: " << config.semilla << " | Oleadas: " << config.oleadas << "\n";
    std::cout << " Jugadores (" << sesion.GetTotalJugadores() << "): ";
    for (int i = 0; i < sesion.GetTotalJugadores(); i++) {
        std::cout << sesion.GetJugador(i)->alias << (sesion.GetJugador(i)->esBot ? " (bot)" : "") << " ";
    }
    std::cout << "\n============================================\n";

    // Variables de estado persistente de direccion y fase para jugadores humanos controlados por script
    double anguloHumano[6] = { 270.0, 270.0, 270.0, 270.0, 270.0, 270.0 };
    Color colorHumano[6] = { CIAN, CIAN, CIAN, CIAN, CIAN, CIAN };

    int totalTicksSimulados = 0;
    const int MAX_TICKS_SEGURIDAD = 120000; // Limite de seguridad para evitar ciclos infinitos

    // Bucle principal de la simulacion headless
    while (!sesion.EstaTerminada() && totalTicksSimulados < MAX_TICKS_SEGURIDAD) {
        if (sesion.ComenzarTurno()) {
            int idxJugador = sesion.GetIndiceEnTurno();
            const Jugador* jActual = sesion.GetJugadorEnTurno();
            int oleadaActual = sesion.GetOleadaActual();

            // Simulacion tick a tick del turno actual
            while (sesion.TurnoEnCurso() && totalTicksSimulados < MAX_TICKS_SEGURIDAD) {
                int tickActual = sesion.GetMotor()->GetTickActual();
                AccionTick act;

                // Si el jugador es Bot, calculamos su decision con la heuristica
                if (jActual->esBot) {
                    act = sesion.DecidirAccionBot();
                } else {
                    // Si es humano, mantenemos el estado anterior salvo que el script mande otra cosa
                    act.anguloObjetivo = anguloHumano[idxJugador];
                    act.colorObjetivo = colorHumano[idxJugador];
                    act.activarPulso = false;

                    if (tieneScript) {
                        for (NodoAccionScript* nodoAcc = script.ObtenerCabeza(); nodoAcc != NULL; nodoAcc = nodoAcc->siguiente) {
                            if (nodoAcc->dato.jugador == jActual->alias && nodoAcc->dato.oleada == oleadaActual && nodoAcc->dato.tick == tickActual) {
                                const AccionScript& accScript = nodoAcc->dato;
                                if (accScript.comando == "COLOR") {
                                    if (accScript.parametro == "M") colorHumano[idxJugador] = MAGENTA;
                                    else if (accScript.parametro == "A") colorHumano[idxJugador] = AMBAR;
                                    else colorHumano[idxJugador] = CIAN;
                                    act.colorObjetivo = colorHumano[idxJugador];
                                } else if (accScript.comando == "GIRAR_A") {
                                    try {
                                        anguloHumano[idxJugador] = std::stod(accScript.parametro);
                                        act.anguloObjetivo = anguloHumano[idxJugador];
                                    } catch (...) {}
                                } else if (accScript.comando == "PULSO") {
                                    act.activarPulso = true;
                                }
                            }
                        }
                    }
                }

                // Ejecutamos el tick en el Motor
                EstadoMotor estado = sesion.EjecutarTick(act);
                totalTicksSimulados++;

                if (estado != ESTADO_EN_JUEGO) {
                    sesion.CerrarTurno(estado);
                    break;
                }
            }
        } else {
            break;
        }
    }

    sesion.Finalizar();

    // Lineas de tiempo por oleada requeridas por la seccion 6.2
    sesion.ImprimirLineasTiempo();

    // Tabla de posiciones final en consola
    std::cout << "\n============================================\n";
    std::cout << "         TABLA FINAL DE POSICIONES\n";
    std::cout << "============================================\n";
    for (int p = 1; p <= sesion.GetTotalJugadores(); p++) {
        int idx = sesion.GetIndiceEnPosicion(p);
        if (idx >= 0) {
            const Jugador* j = sesion.GetJugador(idx);
            const MetricasJugador& m = sesion.GetMetricas(idx);
            std::cout << p << ". " << j->alias << (j->esBot ? " (bot)" : "")
                      << " | Puntos: " << j->puntaje
                      << " | Oleada: " << m.oleadaAlcanzada
                      << " | Precision: " << std::fixed << std::setprecision(1) << (sesion.GetPrecision(idx) * 100.0) << "%\n";
        }
    }
    std::cout << "============================================\n";

    // Persistencia automatica de los resultados de cada jugador en el CSV
    std::string rutaGuardar = config.rutaCSV.empty() ? "historial.csv" : config.rutaCSV;
    std::string mensaje;
    if (sesion.GuardarHistorial(rutaGuardar, mensaje)) {
        std::cout << "[PERSISTENCIA] " << mensaje << "\n\n";
    }
}
