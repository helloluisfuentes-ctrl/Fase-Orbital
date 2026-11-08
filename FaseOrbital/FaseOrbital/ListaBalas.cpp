#include "ListaBalas.h"

/*
 * Funcion: Constructor ListaBalas
 * Descripcion: Inicializa la lista simplemente enlazada de balas vacia, dejando la cabeza en NULL y tam en 0.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
ListaBalas::ListaBalas() {
	cabeza = NULL; 
	tam = 0;
}

/*
 * Funcion: Destructor ~ListaBalas
 * Descripcion: Recorre la lista de balas desde la cabeza hasta el final y libera la memoria de cada nodo con delete.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
ListaBalas::~ListaBalas() {
	// Usamos un puntero auxiliar para no perder la referencia del siguiente nodo antes de hacer delete
	NodoBala* aux = cabeza;
	while (aux != NULL) {
		cabeza = cabeza->siguiente;
		delete aux;
		aux = cabeza;
	}
	tam = 0;
}

/*
 * Funcion: AgregarInicio
 * Descripcion: Inserta un nuevo nodo al inicio de la lista enlazada en tiempo constante O(1).
 * Entradas: NodoBala* nuevo (puntero al nuevo nodo de bala creado dinamicamente)
 * Salidas: Lista con la nueva bala al frente
 */
void ListaBalas::AgregarInicio(NodoBala* nuevo) {
	// Apuntamos el nuevo nodo a la cabeza actual y luego movemos la cabeza al nuevo nodo
	nuevo->siguiente = cabeza;
	cabeza = nuevo;
	tam++;
}

/*
 * Funcion: EliminarSi
 * Descripcion: Recorre toda la lista evaluando una funcion condicion sobre cada bala. Si se cumple,
 *              elimina el nodo usando un puntero doble para evitar un caso especial con la cabeza.
 * Entradas: Puntero a funcion condicion (recibe una estructura Bala y retorna bool)
 * Salidas: Lista filtrada con los nodos eliminados y liberados
 */
void ListaBalas::EliminarSi(bool (*condicion)(Bala)) {
	// Usamos un puntero a puntero para modificar directamente el enlace que apunta al nodo actual
	NodoBala** ptrActual = &cabeza;
	
	while (*ptrActual != NULL) {
		if (condicion((*ptrActual)->bala)) {
			// Guardamos el nodo a borrar, enlazamos el puntero al siguiente y liberamos la memoria
			NodoBala* ptrKiller = *ptrActual;
			*ptrActual = ptrKiller->siguiente;
			delete ptrKiller;
			tam--;
		}
		else {
			// Avanzamos tomando la direccion del puntero siguiente del nodo actual
			ptrActual = &((*ptrActual)->siguiente);
		}
	}
}

/*
 * Funcion: Vaciar
 * Descripcion: Elimina y libera todos los nodos de la lista de balas, dejandola como recien creada.
 * Entradas: Ninguna
 * Salidas: Lista completamente vacia con cabeza en NULL
 */
void ListaBalas::Vaciar() {
	NodoBala* aux = cabeza;
	while (aux != NULL) {
		cabeza = cabeza->siguiente;
		delete aux;
		aux = cabeza;
	}
	tam = 0;
}