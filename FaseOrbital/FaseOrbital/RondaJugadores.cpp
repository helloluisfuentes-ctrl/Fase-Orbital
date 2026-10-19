#include "RondaJugadores.h"

//constructor
RondaJugadores::RondaJugadores() {
	actual = NULL;
	tam = 0;
}

//destructor
RondaJugadores::~RondaJugadores() {
	if (actual == NULL) {
		return;
	}
	actual->anterior->siguiente = NULL;
	NodoJugador* aux = actual;
	while (aux != NULL) {
		actual = actual->siguiente;
		delete aux;
		aux = actual;
	}
	tam = 0;
}

//Agrega un jugador antes de actual 
void RondaJugadores::Agregar(Jugador* j){
	NodoJugador* nuevoNodo = new NodoJugador;
	nuevoNodo->jugador = j;

	if (actual == NULL){
		actual = nuevoNodo;
		nuevoNodo -> siguiente = nuevoNodo;
		nuevoNodo -> anterior = nuevoNodo;
	} else {
		NodoJugador* ultimo = actual->anterior;
		nuevoNodo->siguiente = actual;
		nuevoNodo->anterior = ultimo;
		ultimo->siguiente = nuevoNodo;
		actual->anterior = nuevoNodo;
	}
	tam++;
}

//avanza actual al siguiente jugador segn horario dado true->siguiente false->anterior
void RondaJugadores::Avanzar(bool direccion) {
	if (actual == NULL) return;

	if (direccion) {
		actual = actual->siguiente;
	} else  {
		actual = actual->anterior;
	}
}

//Elimina el jugador actaul y mueve actual segun sentido
Jugador* RondaJugadores::EliminarActual(bool direccion) {
    if (actual == NULL) {
        return NULL;
    }

    Jugador* jugadorEliminado = actual->jugador;
    NodoJugador* nodoABorrar = actual;

    //si es el unico jugador
    if (actual->siguiente == actual) {
        delete nodoABorrar;
        actual = NULL;
        tam = 0;
        return jugadorEliminado;
    }

    //si hay 2 o mas
    if (direccion) {
        actual = actual->siguiente; 
    } else {
        actual = actual->anterior;  
    }

    // Reconectamos 
    nodoABorrar->anterior->siguiente = nodoABorrar->siguiente;
    nodoABorrar->siguiente->anterior = nodoABorrar->anterior;

    delete nodoABorrar;
    tam--;

    return jugadorEliminado;
}

//ver jugador actual
Jugador* RondaJugadores::VerActual() const {
    if (actual == NULL) {
        return NULL;
    }
    return actual->jugador;
}