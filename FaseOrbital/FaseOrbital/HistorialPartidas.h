#pragma once
#include <string>
#include <cstdint>

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

//lista auxiliar para el merge sort
struct NodoPuntaje {
    int valor;
    NodoPuntaje* siguiente = NULL;
};

class ListaPuntajes {
private:
    NodoPuntaje* cabeza;
    int tam;

    //metodos auxiliares para el merge sort
    NodoPuntaje* Dividir(NodoPuntaje* fuente);
    NodoPuntaje* Fusionar(NodoPuntaje* a, NodoPuntaje* b);
    NodoPuntaje* MergeSortRec(NodoPuntaje* nodo);

public:
    ListaPuntajes();
    ~ListaPuntajes();

    //metodos
    void InsertarFinal(int valor);
    void MergeSort();
    double ObtenerMediana() const;

    int Tamano() const { return tam; }
    NodoPuntaje* ObtenerCabeza() const { return cabeza; }
};

//========================================================================================================

struct NodoRegistro {
    RegistroPartida registro;
    NodoRegistro* siguiente = NULL;
};

class HistorialPartidas {
private:
    NodoRegistro* cabeza;
    NodoRegistro* cola;
    int tam;

public:
    HistorialPartidas();
    ~HistorialPartidas();

    HistorialPartidas(const HistorialPartidas&) = delete;
    HistorialPartidas& operator=(const HistorialPartidas&) = delete;

    void AgregarFinal(const RegistroPartida& reg);
    bool CargarCSV(const std::string& rutaArchivo);
    bool GuardarCSV(const std::string& rutaArchivo) const;

    void CopiarPuntajes(const std::string& aliasJugador, ListaPuntajes& destino) const;

    void GenerarReporteJugador(const std::string& aliasJugador) const;

    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == NULL; }
};