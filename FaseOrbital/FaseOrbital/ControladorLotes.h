#pragma once
#include <string>
#include <cstdint>

// Estructura que representa un comando leido del archivo de script para un tick determinado
struct AccionScript {
    std::string jugador;
    int oleada;
    int tick;
    std::string comando;   // "COLOR", "GIRAR_A", "PULSO"
    std::string parametro; // "M", "135", etc. Vacio para PULSO
};

// Nodo simple para almacenar la lista enlazada de acciones del script
struct NodoAccionScript {
    AccionScript dato;
    NodoAccionScript* siguiente = NULL;
};

// Lista simplemente enlazada para almacenar en memoria las ordenes programadas del script
class ListaAccionesScript {
private:
    NodoAccionScript* cabeza;
    NodoAccionScript* cola;
    int tam;

public:
    ListaAccionesScript();
    ~ListaAccionesScript();

    // Regla de Tres: eliminamos copia y asignacion para evitar duplicar la lista de acciones
    ListaAccionesScript(const ListaAccionesScript&) = delete;
    ListaAccionesScript& operator=(const ListaAccionesScript&) = delete;

    void AgregarFinal(const AccionScript& acc);

    // Busca si existe una accion programada que coincida con el jugador, oleada y tick actual
    bool BuscarAccion(const std::string& jugador, int oleada, int tick, AccionScript& salida) const;

    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == NULL; }
};

// Estructura que consolida los parametros pasados por la linea de comandos (CLI)
struct ConfiguracionLotes {
    bool modoLotes = false;
    bool verRanking = false;
    bool verEstadisticas = false;

    std::string rutaCSV = "";
    std::string rutaScript = "";
    uint32_t semilla = 42;
    int oleadas = 10;

    std::string jugadores[6];
    int totalJugadores = 0;
    bool esValido = true;
};

// Modulo para parsear argumentos de consola y ejecutar la simulacion headless por lotes
class ControladorLotes {
public:
    static ConfiguracionLotes ParsearArgumentos(int argc, char* argv[]);
    static void MostrarRankingConsola(const std::string& rutaCSV);
    static void MostrarEstadisticasConsola(const std::string& rutaCSV);

    // Lee el archivo de script linea por linea y llena la lista enlazada de acciones
    static bool CargarScript(const std::string& rutaArchivo, ListaAccionesScript& listaDestino);

    // Ejecuta la partida completa en consola sin abrir ventana de Allegro (Seccion 7.1)
    static void EjecutarModoLotes(const ConfiguracionLotes& config);
};