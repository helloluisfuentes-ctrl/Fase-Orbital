#pragma once
#include "Globales.h"


// --- Lista de balas ---

struct Bala {
	double cord_x;
	double cord_y;
	double speed_x;
	double speed_y;
	Color color;	// 1 = Cian, 2 = Magenta, 3 = Ámbar
	int duenio;
};

struct NodoBala {
	Bala bala;
	NodoBala* siguiente = NULL;
};

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
   
    //metodos
    void AgregarInicio(NodoBala* nuevo);
    void EliminarSi(bool (*condicion)(Bala));
    
    //getters
    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == NULL; }
    NodoBala* ObtenerCabeza() const { return cabeza; }
};



/*uso ejemplo:

	bool balaFueraDelMapa(Bala b) {
		return (b.cord_x > 320 || b.cord_y > 320);
	}

	EliminarBalasSi(cabeza_balas, balaFueraDelMapa);

*/