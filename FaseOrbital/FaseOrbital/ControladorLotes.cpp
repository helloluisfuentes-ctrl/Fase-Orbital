#include "ControladorLotes.h"
#include "HistorialPartidas.h"
#include "Ranking.h"
#include "SesionJuego.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>


// MÉTODOS DE ListaAccionesScript
ListaAccionesScript::ListaAccionesScript() {
    cabeza = NULL;
    cola = NULL;
    tam = 0;
}

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


// PARSER DE LÍNEA DE COMANDOS


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
            while (std::getline(ss, nombre, ',') && config.totalJugadores < 6) {
                if (!nombre.empty()) {
                    config.jugadores[config.totalJugadores++] = nombre;
                }
            }
        }
    }

    return config;
}

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

// PARSER DEL SCRIPT DE ACCIONES 

bool ControladorLotes::CargarScript(const std::string& rutaArchivo, ListaAccionesScript& listaDestino) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo de script: " << rutaArchivo << "\n";
        return false;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        // Ignorar líneas vacías
        if (linea.empty()) continue;

        // Quitar espacios iniciales para revisar comentarios
        size_t inicio = linea.find_first_not_of(" \t\r\n");
        if (inicio == std::string::npos) continue;

        // Las líneas que empiezan con # son comentarios 
        if (linea[inicio] == '#') continue;

        std::stringstream ss(linea);
        AccionScript acc;
        ss >> acc.jugador >> acc.oleada >> acc.tick >> acc.comando;

        // El parámetro es opcional 
        if (!(ss >> acc.parametro)) {
            acc.parametro = "";
        }

        listaDestino.AgregarFinal(acc);
    }

    return true;
}

void ControladorLotes::EjecutarModoLotes(const ConfiguracionLotes& config) {
    SesionJuego sesion;

    // Registrar jugadores
    if (config.totalJugadores == 0) {
        sesion.RegistrarJugador("BOT1", true);
        sesion.RegistrarJugador("BOT2", true);
    } else {
        for (int i = 0; i < config.totalJugadores; i++) {
            bool esBot = SesionJuego::EsAliasDeBot(config.jugadores[i]);
            sesion.RegistrarJugador(config.jugadores[i], esBot);
        }
    }

    // Cargar script si existe
    ListaAccionesScript script;
    bool tieneScript = false;
    if (!config.rutaScript.empty()) {
        tieneScript = ControladorLotes::CargarScript(config.rutaScript, script);
    }

    sesion.Iniciar(config.semilla, config.oleadas);

    std::cout << "\n============================================\n";
    std::cout << " INICIANDO PARTIDA EN MODO POR LOTES\n";
    std::cout << " Semilla: " << config.semilla << " | Oleadas: " << config.oleadas << "\n";
    std::cout << " Jugadores (" << sesion.GetTotalJugadores() << "): ";
    for (int i = 0; i < sesion.GetTotalJugadores(); i++) {
        std::cout << sesion.GetJugador(i)->alias << (sesion.GetJugador(i)->esBot ? " (bot)" : "") << " ";
    }
    std::cout << "\n============================================\n";

    // Estado inicial de dirección y color para humanos
    double anguloHumano[6] = { 270.0, 270.0, 270.0, 270.0, 270.0, 270.0 };
    Color colorHumano[6] = { CIAN, CIAN, CIAN, CIAN, CIAN, CIAN };

    int totalTicksSimulados = 0;
    const int MAX_TICKS_SEGURIDAD = 120000;

    while (!sesion.EstaTerminada() && totalTicksSimulados < MAX_TICKS_SEGURIDAD) {
        if (sesion.ComenzarTurno()) {
            int idxJugador = sesion.GetIndiceEnTurno();
            const Jugador* jActual = sesion.GetJugadorEnTurno();
            int oleadaActual = sesion.GetOleadaActual();

            while (sesion.TurnoEnCurso() && totalTicksSimulados < MAX_TICKS_SEGURIDAD) {
                int tickActual = sesion.GetMotor()->GetTickActual();
                AccionTick act;

                if (jActual->esBot) {
                    act = sesion.DecidirAccionBot();
                } else {
                    act.anguloObjetivo = anguloHumano[idxJugador];
                    act.colorObjetivo = colorHumano[idxJugador];
                    act.activarPulso = false;

                    if (tieneScript) {
                        AccionScript accScript;
                        if (script.BuscarAccion(jActual->alias, oleadaActual, tickActual, accScript)) {
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

    // Líneas de tiempo requeridas por la sección 6.2
    sesion.ImprimirLineasTiempo();

    // Tabla de resultados finales
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

    // Guardado en CSV
    std::string rutaGuardar = config.rutaCSV.empty() ? "historial.csv" : config.rutaCSV;
    std::string mensaje;
    if (sesion.GuardarHistorial(rutaGuardar, mensaje)) {
        std::cout << "[PERSISTENCIA] " << mensaje << "\n\n";
    }
}