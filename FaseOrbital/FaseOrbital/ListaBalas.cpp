#include "ListaBalas.h"



//Inicializa la lista
void InicializarListaBalas(ListaBalas& lista) {
	lista.cabeza = NULL;
	lista.tam = 0;
}



//Destruye toda la lista
void DestruirListaBalas(ListaBalas& lista) {

	PtrNodoBala aux;
	aux = lista.cabeza;

	while (aux != NULL)
	{
		lista.cabeza = lista.cabeza->siguiente;
		delete(aux);
		aux = lista.cabeza;
	}

	lista.tam = 0;
}



//Agrega una bala al inicio de la lista
void AgregarInicioListaBalas(ListaBalas& lista, PtrNodoBala& nuevo)
{
	nuevo->siguiente = lista.cabeza;
	lista.cabeza = nuevo;
	lista.tam++;
}



//Elimina las balas de la lista que cumplan la condicion
void EliminarBalasSi(ListaBalas& lista, bool (*condicion)(Bala)) {

	//puntero a puntero
	PtrNodoBala* ptrActual = &lista.cabeza;

	while (ptrActual != NULL) {

		if (condicion((*ptrActual)->bala)) {

			//Eliminamos el nodo bala y continuamos
			PtrNodoBala ptrKiller = *ptrActual;	//killer no es igual ptrActual, es igua a lo que ptrActual apunta
			*ptrActual = ptrKiller->siguiente;
			delete ptrKiller;
			lista.tam--;
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

	EliminarBalasSi(cabeza_balas, balaFueraDelMapa);

*/