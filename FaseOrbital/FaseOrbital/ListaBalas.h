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

typedef struct ListaBalas {
	PtrNodoBala cabeza;
	int tam;
};


//Inicializa la lista
void InicializarListaBalas(ListaBalas &lista);



//Destruye toda la lista
void DestruirListaBalas(ListaBalas &lista);



//Agrega una bala al inicio de la lista
void AgregarInicioListaBalas(ListaBalas &lista, PtrNodoBala &nuevo);



//Elimina las balas de la lista que cumplan la condicion
void EliminarBalasSi(ListaBalas &lista, bool (*condicion)(Bala));

/*uso ejemplo:

	bool balaFueraDelMapa(Bala b) {
		return (b.cord_x > 320 || b.cord_y > 320);
	}

	EliminarBalasSi(cabeza_balas, balaFueraDelMapa);

*/