#include "ColaSpawn.h"

/*
 * Funcion: Constructor ColaSpawn
 * Descripcion: Inicializa la cola de apariciones vacia con cabeza y cola en NULL y contador en cero.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
ColaSpawn::ColaSpawn() {
	cabeza = NULL;
	cola = NULL;
	tam = 0;
}

/*
 * Funcion: Destructor ~ColaSpawn
 * Descripcion: Desencola y libera toda la memoria dinamica de los nodos hasta dejar la cola vacia.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
ColaSpawn::~ColaSpawn() {
	// Recorremos con un auxiliar liberando nodo a nodo
	NodoSpawn* aux = cabeza;
	while (aux != NULL) {
		cabeza = cabeza->siguiente;
		delete aux;
		aux = cabeza;
	}
	tam = 0;
}

/*
 * Funcion: Constructor de copia ColaSpawn
 * Descripcion: Realiza una copia profunda de todos los nodos de 'otra' cola para que cada jugador
 *              tenga su propia instancia sin compartir memoria.
 * Entradas: const ColaSpawn& otra (referencia a la cola que queremos clonar)
 * Salidas: Nueva cola identica pero con nodos independientes en memoria
 */
ColaSpawn::ColaSpawn(const ColaSpawn& otra) {
    cabeza = NULL;
    cola = NULL;
    tam = 0;

    // Recorremos la cola original y encolamos cada elemento creando nodos nuevos
    NodoSpawn* aux = otra.cabeza;
    while (aux != NULL) {
        Encolar(aux->dato);
        aux = aux->siguiente;
    }
}

/*
 * Funcion: Operador de asignacion operator=
 * Descripcion: Limpia la memoria actual y copia profundamente los nodos de la otra cola, evitando autoasignacion.
 * Entradas: const ColaSpawn& otra (cola a asignar)
 * Salidas: Referencia a nosotros mismos (*this)
 */
ColaSpawn& ColaSpawn::operator=(const ColaSpawn& otra) {
    // Si intentamos asignarnos a nosotros mismos, no hacemos nada para no destruir nuestros datos
    if (this == &otra) {
        return *this;
    }

    // Vaciamos los nodos que teniamos actualmente antes de copiar los nuevos
    NodoSpawn* actual = cabeza;
    while (actual != NULL) {
        cabeza = cabeza->siguiente;
        delete actual;
        actual = cabeza;
    }
    cola = NULL;
    tam = 0;

    // Copiamos los nodos de la otra cola uno por uno
    NodoSpawn* aux = otra.cabeza;
    while (aux != NULL) {
        Encolar(aux->dato);
        aux = aux->siguiente;
    }

    return *this;
}

/*
 * Funcion: Encolar
 * Descripcion: Inserta un nuevo elemento al final de la cola en O(1) usando el puntero cola.
 * Entradas: const Spawn& s (datos de aparicion del enemigo)
 * Salidas: Cola con el nuevo nodo al final
 */
void ColaSpawn::Encolar(const Spawn& s) {
    NodoSpawn* nuevo = new NodoSpawn();
    nuevo->dato = s;
    nuevo->siguiente = NULL;

    // Si estaba vacia, la cabeza y la cola son el mismo nodo nuevo
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

/*
 * Funcion: Desencolar
 * Descripcion: Extrae el elemento del frente de la cola en O(1), copia su valor en 'out' y libera el nodo.
 * Entradas: Spawn& out (referencia donde se guarda el dato extraido)
 * Salidas: Retorna true si se pudo desencolar, false si la cola estaba vacia
 */
bool ColaSpawn::Desencolar(Spawn& out) {
    if (Vacia()) {
        return false;
    }

    // Copiamos el dato del frente, avanzamos la cabeza y borramos el nodo extraido
    out = cabeza->dato;
    NodoSpawn* aux = cabeza;
    cabeza = cabeza->siguiente;
    delete aux;
    tam--;

    // Si la cola se quedo sin nodos, reiniciamos el puntero cola a NULL
    if (Vacia()) {
        cola = NULL;
    }

    return true;
}

/*
 * Funcion: VerFrente
 * Descripcion: Permite consultar el dato del primer nodo sin sacarlo de la cola.
 * Entradas: Ninguna
 * Salidas: Puntero constante al Spawn del frente o NULL si la cola esta vacia
 */
const Spawn* ColaSpawn::VerFrente() const {
    if (Vacia()) {
        return NULL;
    }

    return &cabeza->dato;
}
