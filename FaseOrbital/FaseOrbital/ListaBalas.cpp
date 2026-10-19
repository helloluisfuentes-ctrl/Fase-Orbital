#include "ListaBalas.h"



//Inicializa la lista
ListaBalas::ListaBalas() {
	cabeza = NULL; 
	tam = 0;
}



//Destruye toda la lista
ListaBalas::~ListaBalas() {
	NodoBala* aux = cabeza;
	while (aux != NULL) {
		cabeza = cabeza->siguiente;
		delete aux;
		aux = cabeza;
	}
	tam = 0;
}



//Agrega una bala al inicio de la lista
void ListaBalas::AgregarInicio(NodoBala* nuevo) {
	nuevo->siguiente = cabeza;
	cabeza = nuevo;
	tam++;
}



//Elimina las balas de la lista que cumplan la condicion
void ListaBalas::EliminarSi(bool (*condicion)(Bala)) {

	NodoBala** ptrActual = &cabeza; //puntero a puntero
	
	while (*ptrActual != NULL) {
		if (condicion((*ptrActual)->bala)) {

			NodoBala* ptrKiller = *ptrActual;
			*ptrActual = ptrKiller->siguiente;
			delete ptrKiller;
			tam--;
		}
		else {
			ptrActual = &((*ptrActual)->siguiente);
		}
	}
}
/*uso ejemplo:
	
	bool balaFueraDelMapa(Bala b) {
	    return (b.cord_x > 320 || b.cord_y > 320);
	}

	EliminarSi(balaFueraDelMapa);

*/