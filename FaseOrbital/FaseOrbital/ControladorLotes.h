#pragma once
#include <string>
#include <cstdint>

// Estructura que representa una línea de acción del script 
struct AccionScript {
    std::string jugador;
    int oleada;
    int tick;
    std::string comando;   // "COLOR", "GIRAR_A", "PULSO"
    std::string parametro; // "M", "135", etc. vacío para PULSO
};

// Nodo de la lista enlazada de acciones
struct NodoAccionScript {
    AccionScript dato;
    NodoAccionScript* siguiente = nullptr;
};

// Lista simplemente enlazada para almacenar las acciones del script 
class ListaAccionesScript {
private:
    NodoAccionScript* cabeza;
    NodoAccionScript* cola;
    int tam;

public:
    ListaAccionesScript();
    ~ListaAccionesScript();

    // Regla de Tres
    ListaAccionesScript(const ListaAccionesScript&) = delete;
    ListaAccionesScript& operator=(const ListaAccionesScript&) = delete;

    void AgregarFinal(const AccionScript& acc);

    // Consulta rápida durante el tick del juego si Hay alguna acción programada para este jugador, oleada y tick?
    bool BuscarAccion(const std::string& jugador, int oleada, int tick, AccionScript& salida) const;

    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == nullptr; }
};

// Parámetros de la línea de comandos
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

class ControladorLotes {
public:
    static ConfiguracionLotes ParsearArgumentos(int argc, char* argv[]);
    static void MostrarRankingConsola(const std::string& rutaCSV);
    static void MostrarEstadisticasConsola(const std::string& rutaCSV);

    // Lee el archivo de script línea por línea y llena la lista enlazada de acciones
    static bool CargarScript(const std::string& rutaArchivo, ListaAccionesScript& listaDestino);

    // Ejecuta la partida completa en modo por lotes según la especificación (Sección 7.1)
    static void EjecutarModoLotes(const ConfiguracionLotes& config);
};