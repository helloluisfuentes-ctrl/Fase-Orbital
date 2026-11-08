#include "RondaJugadores.h"

/*
 * Funcion: Constructor RondaJugadores
 * Descripcion: Inicializa la lista circular vacia con el puntero actual en NULL y tamano en cero.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
RondaJugadores::RondaJugadores() {
	actual = NULL;
	tam = 0;
}

/*
 * Funcion: Destructor ~RondaJugadores
 * Descripcion: Rompe la circularidad de la lista poniendo el enlace final en NULL y luego libera
 *              todos los nodos secuencialmente para no dejar fugas de memoria.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
RondaJugadores::~RondaJugadores() {
	if (actual == NULL) {
		return;
	}

	// Rompemos el ciclo circular para poder recorrer como lista lineal hasta NULL
	actual->anterior->siguiente = NULL;
	NodoJugador* aux = actual;
	while (aux != NULL) {
		actual = actual->siguiente;
		delete aux;
		aux = actual;
	}
	tam = 0;
}

/*
 * Funcion: Agregar
 * Descripcion: Inserta un nuevo jugador antes del nodo 'actual' (equivalente a insertarlo al final de la ronda).
 *              Mantiene la propiedad circular enlazando el nuevo nodo con el ultimo y con el actual.
 * Entradas: Jugador* j (puntero a la estructura del jugador)
 * Salidas: Lista circular con el nuevo participante agregado
 */
void RondaJugadores::Agregar(Jugador* j) {
	NodoJugador* nuevoNodo = new NodoJugador;
	nuevoNodo->jugador = j;

	// Si la lista estaba vacia, el nodo se apunta a si mismo en ambos sentidos
	if (actual == NULL) {
		actual = nuevoNodo;
		nuevoNodo->siguiente = nuevoNodo;
		nuevoNodo->anterior = nuevoNodo;
	} else {
		// Insertamos entre el ultimo (actual->anterior) y el actual
		NodoJugador* ultimo = actual->anterior;
		nuevoNodo->siguiente = actual;
		nuevoNodo->anterior = ultimo;
		ultimo->siguiente = nuevoNodo;
		actual->anterior = nuevoNodo;
	}
	tam++;
}

/*
 * Funcion: Avanzar
 * Descripcion: Mueve el puntero 'actual' al siguiente o al anterior jugador en O(1) segun la direccion.
 *              Permite alternar entre sentido horario (siguiente) y antihorario (anterior) para el orden serpenteante.
 * Entradas: bool direccion (true = horario/siguiente, false = antihorario/anterior)
 * Salidas: Puntero actual actualizado
 */
void RondaJugadores::Avanzar(bool direccion) {
	if (actual == NULL) return;

	if (direccion) {
		actual = actual->siguiente;
	} else {
		actual = actual->anterior;
	}
}

/*
 * Funcion: EliminarActual
 * Descripcion: Remueve al jugador en turno cuando su nucleo es destruido, reacomoda los enlaces circulares
 *              de sus vecinos y avanza el turno al siguiente jugador activo segun la direccion.
 * Entradas: bool direccion (sentido en el que se movera el turno tras la eliminacion)
 * Salidas: Puntero al Jugador eliminado (para registrar metricas)
 */
Jugador* RondaJugadores::EliminarActual(bool direccion) {
    if (actual == NULL) {
        return NULL;
    }

    Jugador* jugadorEliminado = actual->jugador;
    NodoJugador* nodoABorrar = actual;

    // Caso borde: era el unico jugador restante en la ronda
    if (actual->siguiente == actual) {
        delete nodoABorrar;
        actual = NULL;
        tam = 0;
        return jugadorEliminado;
    }

    // Si hay mas jugadores, movemos actual al siguiente segun el sentido antes de borrar el nodo
    if (direccion) {
        actual = actual->siguiente; 
    } else {
        actual = actual->anterior;  
    }

    // Reconectamos los nodos adyacentes saltandonos el nodo a borrar
    nodoABorrar->anterior->siguiente = nodoABorrar->siguiente;
    nodoABorrar->siguiente->anterior = nodoABorrar->anterior;

    delete nodoABorrar;
    tam--;

    return jugadorEliminado;
}

/*
 * Funcion: VerActual
 * Descripcion: Devuelve el puntero al jugador que tiene el turno activo en este momento.
 * Entradas: Ninguna
 * Salidas: Jugador* del turno actual o NULL si la ronda esta vacia
 */
Jugador* RondaJugadores::VerActual() const {
    if (actual == NULL) {
        return NULL;
    }
    return actual->jugador;
}
