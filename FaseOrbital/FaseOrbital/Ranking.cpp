#include "Ranking.h"
#include <iostream>

// Constructor
Ranking::Ranking() {
    cabeza = NULL;
    tam = 0;
}

// Destructor
Ranking::~Ranking() {
    NodoRanking* aux = cabeza;
    while (aux != NULL) {
        cabeza = cabeza->siguiente;
        delete aux;
        aux = cabeza;
    }
    tam = 0;
}

//Devuelve true si a esta mas alto en el top que b
bool Ranking::EsMejor(const EntradaRanking& a, const EntradaRanking& b) const {
    //mayor puntaje
    if (a.puntaje != b.puntaje) {
        return a.puntaje > b.puntaje;
    }
    // mayor prescion de color
    if (a.precisionColor != b.precisionColor) {
        return a.precisionColor > b.precisionColor;
    }
    // orden alfabetico
    return a.alias < b.alias;
}

//Inserta el nodo ordenado, la cabeza es el top1 y asi va bajando
void Ranking::InsertarOrdenado(const EntradaRanking& entrada){
    NodoRanking* nuevoNodo = new NodoRanking;
    nuevoNodo->dato = entrada;
    nuevoNodo->siguiente = NULL;

    bool insertado = false;
    NodoRanking* aux = cabeza;

    if (cabeza == NULL){        //si esta vacia
        cabeza = nuevoNodo;
        insertado = true;

    } else if (EsMejor(nuevoNodo->dato,cabeza->dato)) { //se inserta al inicio
        nuevoNodo->siguiente = cabeza;
        cabeza = nuevoNodo;
        insertado = true;

    } else {
        while(aux->siguiente != NULL){
            if (EsMejor(nuevoNodo->dato,aux->siguiente->dato)){ //se inserta entre aux y aux->siguiente
                nuevoNodo->siguiente = aux->siguiente;
                aux->siguiente = nuevoNodo;
                insertado = true;
                break;
            }
            aux = aux->siguiente;
        }
    }
    if (!insertado){
        aux->siguiente = nuevoNodo; //se incerta a lo ultimo
    }

    tam++;
}

// Imprime top 10, o lo que diga k
void Ranking::Top(int k) const {
    NodoRanking* aux = cabeza;
    int counter = 1;

    std::cout << "\n=== TOP " << k << " ===\n\n";

    if (aux == NULL){
        std::cout << "No hay registros por el momento :/ \n";
        return;
    }

    while (aux != NULL && counter <= k){
        std::cout << counter << ". " << aux->dato.alias << " " << aux->dato.puntaje << " pts\n\n";
        aux = aux->siguiente;
        counter++;
    }
}