#include "ListaEnemigos.h"

/*
 * Funcion: Constructor ListaEnemigos
 * Descripcion: Inicializa la lista doblemente enlazada vacia, colocando cabeza y cola en NULL y tamano en 0.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
ListaEnemigos::ListaEnemigos() {
	cabeza = NULL;
	cola = NULL;
	tam = 0;
}

/*
 * Funcion: Destructor ~ListaEnemigos
 * Descripcion: Recorre la lista de inicio a fin liberando la memoria de cada nodo para evitar fugas.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
ListaEnemigos::~ListaEnemigos() {
	// Puntero auxiliar para avanzar antes de borrar el nodo actual
	NodoEnemigo* aux = cabeza;
	while (aux != NULL) {
		cabeza = cabeza->siguiente;
		delete aux;
		aux = cabeza;
	}
	cola = NULL;
	tam = 0;
}

/*
 * Funcion: InsertarFinal
 * Descripcion: Agrega un nodo al final de la lista en O(1) usando directamente el puntero cola.
 * Entradas: NodoEnemigo* nuevo (nodo a insertar)
 * Salidas: Lista con el nodo agregado al final
 */
void ListaEnemigos::InsertarFinal(NodoEnemigo* nuevo) {
	tam++;
	// Si la lista estaba vacia, el nuevo nodo es tanto la cabeza como la cola
	if (cabeza == NULL) {
		cabeza = nuevo;
		cola = nuevo;
	}
	else {
		// Enlazamos la cola actual con el nuevo nodo y movemos la cola
		nuevo->anterior = cola;
		cola->siguiente = nuevo;
		cola = nuevo;
	}
}

/*
 * Funcion: InsertarDespues
 * Descripcion: Inserta un nodo inmediatamente despues de otro nodo existente en O(1), acomodando los 4 punteros.
 *              Se utiliza principalmente cuando un enemigo Divisor muere y genera dos Minis.
 * Entradas: NodoEnemigo* nodoAnterior (nodo de referencia), NodoEnemigo* nuevo (nodo a insertar)
 * Salidas: Lista con el nodo insertado en la posicion intermedia o final correspondiente
 */
void ListaEnemigos::InsertarDespues(NodoEnemigo* nodoAnterior, NodoEnemigo* nuevo) {
	if (nodoAnterior == NULL || nuevo == NULL) return;
	
	nuevo->anterior = nodoAnterior;
	nuevo->siguiente = nodoAnterior->siguiente;
	
	// Si el nodo de referencia era la cola, ahora el nuevo pasa a ser la nueva cola
	if (cola == nodoAnterior) {
		cola = nuevo;
	}
	else {
		nodoAnterior->siguiente->anterior = nuevo;
	}
	nodoAnterior->siguiente = nuevo;
	tam++;
}

/*
 * Funcion: Eliminar
 * Descripcion: Desconecta un nodo de la lista doblemente enlazada y libera su memoria en O(1).
 *              Maneja casos borde: lista con un solo nodo, borrar cabeza, borrar cola o nodo intermedio.
 * Entradas: NodoEnemigo* nodoABorrar (puntero al nodo que se va a eliminar)
 * Salidas: Retorna el puntero al siguiente nodo valido para continuar recorridos sin perder la referencia
 */
NodoEnemigo* ListaEnemigos::Eliminar(NodoEnemigo* nodoABorrar) {
	if (nodoABorrar == NULL) return NULL;
	NodoEnemigo* nodoSiguiente = NULL;

	if (cabeza == cola) {
		// Caso 1: Era el unico nodo de la lista
		cabeza = NULL;
		cola = NULL;
		nodoSiguiente = NULL;
	}
	else if (cabeza == nodoABorrar) {
		// Caso 2: Era la cabeza de la lista
		cabeza = cabeza->siguiente;
		cabeza->anterior = NULL;
		nodoSiguiente = cabeza;
	}
	else if (cola == nodoABorrar) {
		// Caso 3: Era la cola de la lista
		cola = cola->anterior;
		cola->siguiente = NULL;
		nodoSiguiente = NULL;
	}
	else {
		// Caso 4: Nodo intermedio, unimos el anterior con el siguiente saltandonos este nodo
		nodoABorrar->anterior->siguiente = nodoABorrar->siguiente;
		nodoABorrar->siguiente->anterior = nodoABorrar->anterior;
		nodoSiguiente = nodoABorrar->siguiente;
	}

	delete nodoABorrar;
	tam--;
	return nodoSiguiente;
}

/*
 * Funcion: Vaciar
 * Descripcion: Libera todos los nodos de la lista dejandola en su estado inicial vacio.
 * Entradas: Ninguna
 * Salidas: Lista vacia con cabeza y cola en NULL
 */
void ListaEnemigos::Vaciar() {
	NodoEnemigo* aux = cabeza;
	while (aux != NULL) {
		cabeza = cabeza->siguiente;
		delete aux;
		aux = cabeza;
	}
	cola = NULL;
	tam = 0;
}