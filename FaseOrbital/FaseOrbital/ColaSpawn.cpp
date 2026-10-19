#include "ColaSpawn.h"

//constructor
ColaSpawn::ColaSpawn(){
	cabeza = NULL;
	cola = NULL;
	tam = 0;
}



//destructor
ColaSpawn::~ColaSpawn() {
	NodoSpawn* aux = cabeza;
	while (aux != NULL) {
		cabeza = cabeza->siguiente;
		delete aux;
		aux = cabeza;
	}
	tam = 0;
}



//Constructor de copia
ColaSpawn::ColaSpawn(const ColaSpawn& otra) {
    //inicializar cola
    cabeza = NULL;
    cola = cabeza = NULL;
    tam = 0;

    NodoSpawn* aux = otra.cabeza;

    //Copiar la cola nodo por nodo
    while (aux != NULL) {
        Encolar(aux->dato);
        aux = aux->siguiente;
    }
}

//operador de asignacion
ColaSpawn& ColaSpawn::operator=(const ColaSpawn& otra) {
    //Evitar auto-asignación 
    if (this == &otra) {
        return *this;
    }

    //Limpiar la memoria que ya teníamos 
    NodoSpawn* actual = cabeza;
    while (actual != NULL) {
        cabeza = cabeza->siguiente;
        delete (actual);
        actual = cabeza;
    }
    cola = NULL;
    tam = 0;

    //Copiar profundamente los nodos de 'otra'
    NodoSpawn* aux = otra.cabeza;
    while (aux != NULL) {
        Encolar(aux->dato);
        aux = aux->siguiente;
    }

    //Retornar la referencia a nosotros mismos
    return *this;
}


// añade al final
void ColaSpawn::Encolar(const Spawn& s) {
    NodoSpawn* nuevo = new NodoSpawn();
    nuevo->dato = s;
    nuevo->siguiente = nullptr;

    if (Vacia()) {
        cabeza = nuevo;
        cola = nuevo;
    }
    else {
        cola->siguiente = nuevo;
        cola = nuevo;
    }
    tam++;
}


// devuelve el primer nodo en "Out"
bool ColaSpawn::Desencolar(Spawn& out) {

    if (Vacia()) {
        return false;
    }

    out = cabeza->dato;
    NodoSpawn* aux = cabeza;
    cabeza = cabeza->siguiente;
    delete(aux);
    tam--;

    if (Vacia()) {
        cola = NULL;
    }

    return true;
}

//manda un solo leer del primer nodo
const Spawn* ColaSpawn::VerFrente() const {
    if (Vacia()) {
        return NULL;
    }

    Spawn* aux;
    aux = &cabeza->dato;
    return aux;
}