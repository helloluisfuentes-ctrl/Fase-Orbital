#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Globales.h"


// --- Lista de balas ---

typedef struct Bala {
	double cord_x;
	double cord_y;
	double speed_x;
	double speed_y;
	Color color;	// 1 = Cian, 2 = Magenta, 3 = Ámbar
	int duenio;
};

typedef struct NodoBala {
	Bala bala;
	NodoBala* siguiente = NULL;
}*PtrNodoBala;

class ListaBalas {

private:
    NodoBala* cabeza;
    int tam;

public:
    //Constructor
    ListaBalas();

    //Destructor
    ~ListaBalas();

    //Regla de Tres, se usa delete porque aqui no en necesario complicarse con una copia profunda
    //constructor de copias
    ListaBalas(const ListaBalas&) = delete;
    //operador de asignacion
    ListaBalas& operator=(const ListaBalas&) = delete;
   
    //metodos (
    void agregarInicio(NodoBala* nuevo);
    void eliminarSi(bool (*condicion)(Bala));
    
    //getters
    int tamano() const { return tam; }
    bool vacia() const { return cabeza == NULL; }
    NodoBala* obtenerCabeza() const { return cabeza; }
};



/*uso ejemplo:

	bool balaFueraDelMapa(Bala b) {
		return (b.cord_x > 320 || b.cord_y > 320);
	}

	EliminarBalasSi(cabeza_balas, balaFueraDelMapa);

*/