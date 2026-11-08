#pragma once
#include <string>
#include <cstdint>

// Estructura que almacena los 21 campos requeridos por el archivo historial.csv
struct RegistroPartida {
    int idPartida;
    uint32_t semilla;
    std::string modo;
    std::string jugador;
    bool esBot;
    int posicion;
    int puntaje;
    int oleada;
    int disparos;
    int correctos;
    int erroneos;
    int comboMax;
    int pulsos;
    int danoTotal;
    int ticks;
    int bajasDron;
    int bajasGirador;
    int bajasDivisor;
    int bajasMini;
    int bajasTanque;
    int bajasCamaleon;
};

// Nodo simple para la lista auxiliar de puntajes que ordenamos con Merge Sort
struct NodoPuntaje {
    int valor;
    NodoPuntaje* siguiente = NULL;
};

// Lista enlazada auxiliar utilizada para ordenar puntajes y calcular la mediana sin usar arreglos
class ListaPuntajes {
private:
    NodoPuntaje* cabeza;
    int tam;

    // Metodos auxiliares para la recursion del Merge Sort
    NodoPuntaje* Dividir(NodoPuntaje* fuente);
    NodoPuntaje* Fusionar(NodoPuntaje* a, NodoPuntaje* b);
    NodoPuntaje* MergeSortRec(NodoPuntaje* nodo);

public:
    ListaPuntajes();
    ~ListaPuntajes();

    // Metodos principales
    void InsertarFinal(int valor);
    void MergeSort();
    double ObtenerMediana() const;

    int Tamano() const { return tam; }
    NodoPuntaje* ObtenerCabeza() const { return cabeza; }
};

// ========================================================================================================

// Nodo simple para almacenar cada registro cargado o guardado del historial
struct NodoRegistro {
    RegistroPartida registro;
    NodoRegistro* siguiente = NULL;
};

// Lista simplemente enlazada para gestionar la persistencia y lectura de partidas en CSV
class HistorialPartidas {
private:
    NodoRegistro* cabeza;
    NodoRegistro* cola;
    int tam;

public:
    HistorialPartidas();
    ~HistorialPartidas();

    // Regla de Tres: eliminamos copia y asignacion para evitar duplicar la lista de registros
    HistorialPartidas(const HistorialPartidas&) = delete;
    HistorialPartidas& operator=(const HistorialPartidas&) = delete;

    void AgregarFinal(const RegistroPartida& reg);
    bool CargarCSV(const std::string& rutaArchivo);
    bool GuardarCSV(const std::string& rutaArchivo) const;

    void CopiarPuntajes(const std::string& aliasJugador, ListaPuntajes& destino) const;
    void GenerarReporteJugador(const std::string& aliasJugador) const;

    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == NULL; }
    NodoRegistro* ObtenerCabeza() const { return cabeza; }
};