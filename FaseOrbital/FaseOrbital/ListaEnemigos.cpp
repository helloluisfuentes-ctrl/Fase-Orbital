#include "ListaEnemigos.h"



//Inicializa la lista
void InicializarListaEnemigos(ListaEnemigos& lista) {
	lista.cabeza = NULL;
	lista.cola = NULL;
	lista.tam = 0;
}



//Destruye toda la lista
void DestruirListaEnemigos(ListaEnemigos& lista) {

	PtrNodoEnemigo aux;
	aux = lista.cabeza;

	while (aux != NULL)
	{
		lista.cabeza = lista.cabeza->siguiente;
		delete(aux);
		aux = lista.cabeza;
	}

	lista.cola = NULL;
	lista.tam = 0;
}



//Inserta un nuevo nodo al final
void InsertarFinalListaEnemigos(ListaEnemigos& lista, PtrNodoEnemigo nuevo) {

	lista.tam++;

	if (lista.cabeza == NULL) {
		lista.cabeza = nuevo;
		lista.cola = nuevo;
	}
	else {
		nuevo->anterior = lista.cola;
		lista.cola->siguiente = nuevo;
		lista.cola = nuevo;
	}
}

//Inserta un nodo despues del nodo dado
void InsertarDespuesDe(ListaEnemigos& lista, PtrNodoEnemigo nodoAnterior, PtrNodoEnemigo nuevo) {

	nuevo->anterior = nodoAnterior;
	nuevo->siguiente = nodoAnterior->siguiente;

	if (lista.cola == nodoAnterior) {
		lista.cola = nuevo;
	}
	else {
		nodoAnterior->siguiente->anterior = nuevo;
	}

	nodoAnterior->siguiente = nuevo;

}

//Elimina un nodo de la lista
PtrNodoEnemigo EliminarNodoListaEnemigos(ListaEnemigos& lista, PtrNodoEnemigo nodoABorrar) {

	PtrNodoEnemigo nodoSiguiente;

	if (lista.cabeza == lista.cola) {
		lista.cabeza = NULL;
		lista.cola = NULL;
		nodoSiguiente = NULL;
	}
	else if (lista.cabeza == nodoABorrar){
		lista.cabeza = lista.cabeza->siguiente;
		lista.cabeza->anterior = NULL;
		nodoSiguiente = lista.cabeza;
	}
	else if (lista.cola == nodoABorrar){
		lista.cola = lista.cola->anterior;
		lista.cola->siguiente = NULL;
		nodoSiguiente = NULL;
	}
	else {
		nodoABorrar->anterior->siguiente = nodoABorrar->siguiente;
		nodoABorrar->siguiente->anterior = nodoABorrar->anterior;
		nodoSiguiente = nodoABorrar->siguiente;
	}

	delete(nodoABorrar);
	lista.tam--;
	return nodoSiguiente;
}