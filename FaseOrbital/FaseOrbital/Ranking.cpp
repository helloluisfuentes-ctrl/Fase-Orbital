#include "Ranking.h"
#include <iostream>

/*
 * Funcion: Constructor Ranking
 * Descripcion: Inicializa la lista ordenada del ranking vacia, con cabeza en NULL y tamano en cero.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
Ranking::Ranking() {
    cabeza = NULL;
    tam = 0;
}

/*
 * Funcion: Destructor ~Ranking
 * Descripcion: Recorre la lista de clasificacion liberando cada nodo con delete para no dejar fugas.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
Ranking::~Ranking() {
    NodoRanking* aux = cabeza;
    while (aux != NULL) {
        cabeza = cabeza->siguiente;
        delete aux;
        aux = cabeza;
    }
    tam = 0;
}

/*
 * Funcion: EsMejor
 * Descripcion: Aplica los 3 criterios de desempate oficiales de la seccion 6.4:
 *              1. Mayor puntaje
 *              2. En empate, mayor precision de color
 *              3. En empate, orden alfabetico por alias
 * Entradas: const EntradaRanking& a, const EntradaRanking& b (las dos entradas a comparar)
 * Salidas: Retorna true si 'a' debe colocarse antes que 'b' en el ranking
 */
bool Ranking::EsMejor(const EntradaRanking& a, const EntradaRanking& b) const {
    // 1. Criterio principal: mayor puntaje
    if (a.puntaje != b.puntaje) {
        return a.puntaje > b.puntaje;
    }
    // 2. Primer desempate: mayor precision de color
    if (a.precisionColor != b.precisionColor) {
        return a.precisionColor > b.precisionColor;
    }
    // 3. Segundo desempate: orden alfabetico ascendente por alias
    return a.alias < b.alias;
}

/*
 * Funcion: InsertarOrdenado
 * Descripcion: Inserta un nuevo participante en la posicion correcta de la lista ordenada segun EsMejor.
 *              Maneja los 3 casos: lista vacia, insercion en la cabeza o insercion intermedia/final.
 * Entradas: const EntradaRanking& entrada (datos del jugador a clasificar)
 * Salidas: Lista ordenada con el nuevo nodo insertado
 */
void Ranking::InsertarOrdenado(const EntradaRanking& entrada) {
    NodoRanking* nuevoNodo = new NodoRanking;
    nuevoNodo->dato = entrada;
    nuevoNodo->siguiente = NULL;

    bool insertado = false;
    NodoRanking* aux = cabeza;

    // Caso 1: la lista estaba vacia
    if (cabeza == NULL) {
        cabeza = nuevoNodo;
        insertado = true;
    } 
    // Caso 2: supera al que estaba de primer lugar (nueva cabeza)
    else if (EsMejor(nuevoNodo->dato, cabeza->dato)) {
        nuevoNodo->siguiente = cabeza;
        cabeza = nuevoNodo;
        insertado = true;
    } 
    // Caso 3: buscamos la posicion intermedia adecuada
    else {
        while (aux->siguiente != NULL) {
            if (EsMejor(nuevoNodo->dato, aux->siguiente->dato)) {
                nuevoNodo->siguiente = aux->siguiente;
                aux->siguiente = nuevoNodo;
                insertado = true;
                break;
            }
            aux = aux->siguiente;
        }
    }

    // Si no fue mejor que ninguno de los anteriores, se inserta al puro final
    if (!insertado) {
        aux->siguiente = nuevoNodo;
    }

    tam++;
}

/*
 * Funcion: Top
 * Descripcion: Recorre la lista e imprime en consola hasta los primeros k jugadores del ranking.
 * Entradas: int k (cantidad maxima de posiciones a mostrar, por defecto 10)
 * Salidas: Imprime la tabla en la consola estandar
 */
void Ranking::Top(int k) const {
    NodoRanking* aux = cabeza;
    int counter = 1;

    std::cout << "\n=== TOP " << k << " ===\n\n";

    if (aux == NULL) {
        std::cout << "No hay registros por el momento :/ \n";
        return;
    }

    // Recorremos hasta k nodos o hasta que se acabe la lista
    while (aux != NULL && counter <= k) {
        std::cout << counter << ". " << aux->dato.alias << " " << aux->dato.puntaje << " pts\n\n";
        aux = aux->siguiente;
        counter++;
    }
}
