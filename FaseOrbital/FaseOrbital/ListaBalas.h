#pragma once
#include "Globales.h"

// Estructura que almacena los datos de una bala disparada por la nave
struct Bala {
	double cord_x;
	double cord_y;
	double speed_x;
	double speed_y;
	Color color;	// 1 = Cian, 2 = Magenta, 3 = Ámbar
	int duenio;
};

// Nodo simple para la lista de balas activas en pantalla
struct NodoBala {
	Bala bala;
	NodoBala* siguiente = NULL;
};

// Lista simplemente enlazada para manejar las balas disparadas
class ListaBalas {

private:
    NodoBala* cabeza;
    int tam;

public:
    // Constructor: inicializa la lista vacia
    ListaBalas();

    // Destructor: recorre y libera toda la memoria de los nodos
    ~ListaBalas();

    // Regla de Tres: borramos el constructor de copia y el operador de asignacion
    // para evitar que se dupliquen punteros y tengamos problemas de doble liberacion
    ListaBalas(const ListaBalas&) = delete;
    ListaBalas& operator=(const ListaBalas&) = delete;
   
    // Metodos principales
    void AgregarInicio(NodoBala* nuevo);
    void EliminarSi(bool (*condicion)(Bala));
    void Vaciar();
    
    // Metodos de consulta (getters)
    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == NULL; }
    NodoBala* ObtenerCabeza() const { return cabeza; }
    NodoBala** ObtenerPtrCabeza() { return &cabeza; }
};