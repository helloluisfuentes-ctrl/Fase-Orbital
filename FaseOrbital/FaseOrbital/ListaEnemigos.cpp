#include "ListaEnemigos.h"



//Inicializa la lista
ListaEnemigos::ListaEnemigos() {
	cabeza = NULL;
	cola = NULL;
	tam = 0;
}



//Destruye toda la lista
ListaEnemigos::~ListaEnemigos() {
	NodoEnemigo* aux = cabeza;
	while (aux != NULL) {
		cabeza = cabeza->siguiente;
		delete aux;
		aux = cabeza;
	}
	cola = NULL;
	tam = 0;
}



//Inserta un nuevo nodo al final
void ListaEnemigos::InsertarFinal(NodoEnemigo* nuevo) {
	tam++;
	if (cabeza == NULL) {
		cabeza = nuevo;
		cola = nuevo;
	}
	else {
		nuevo->anterior = cola;
		cola->siguiente = nuevo;
		cola = nuevo;
	}
}

//Inserta un nodo despues del nodo dado
void ListaEnemigos::InsertarDespues(NodoEnemigo* nodoAnterior, NodoEnemigo* nuevo) {
	if (nodoAnterior == NULL || nuevo == NULL) return;
	nuevo->anterior = nodoAnterior;
	nuevo->siguiente = nodoAnterior->siguiente;
	if (cola == nodoAnterior) {
		cola = nuevo;
	}
	else {
		nodoAnterior->siguiente->anterior = nuevo;
	}
	nodoAnterior->siguiente = nuevo;
	tam++;
}

//Elimina un nodo de la lista
NodoEnemigo* ListaEnemigos::Eliminar(NodoEnemigo* nodoABorrar) {
	if (nodoABorrar == NULL) return NULL;
	NodoEnemigo* nodoSiguiente = NULL;
	if (cabeza == cola) {
		// Caso: Era el único nodo de la lista
		cabeza = NULL;
		cola = NULL;
		nodoSiguiente = NULL;
	}
	else if (cabeza == nodoABorrar) {
		// Caso: Era la cabeza
		cabeza = cabeza->siguiente;
		cabeza->anterior = NULL;
		nodoSiguiente = cabeza;
	}
	else if (cola == nodoABorrar) {
		// Caso: Era la cola
		cola = cola->anterior;
		cola->siguiente = NULL;
		nodoSiguiente = NULL;
	}
	else {
		// Caso: Nodo intermedio
		nodoABorrar->anterior->siguiente = nodoABorrar->siguiente;
		nodoABorrar->siguiente->anterior = nodoABorrar->anterior;
		nodoSiguiente = nodoABorrar->siguiente;
	}
	delete nodoABorrar;
	tam--;
	return nodoSiguiente;
}