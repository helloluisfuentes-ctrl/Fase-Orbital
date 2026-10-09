#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "Globales.h"


struct Enemigo {
    int id;
    TipoEnemigo tipo;
    Color color;       // 1 = Cian, 2 = Magenta, 3 = Ámbar
    int vida;
    int radio;

    double r;        // Posición radial (distancia al núcleo)
    double theta;    // Ángulo en grados
    double omega;    // Velocidad de giro

    int ticksColor;  // para el camaleon
};

typedef struct NodoEnemigo {
    Enemigo enemigo;
    NodoEnemigo* siguiente = NULL;
    NodoEnemigo* anterior = NULL;
}*PtrNodoEnemigo;

typedef struct ListaEnemigos {
    NodoEnemigo* cabeza;
    NodoEnemigo* cola;
    int tam;
};



//Inicializa la lista
void InicializarListaEnemigos(ListaEnemigos &lista);



//Destruye toda la lista
void DestruirListaEnemigos(ListaEnemigos &lista);



//Inserta un nuevo nodo al final
void InsertarFinalListaEnemigos(ListaEnemigos &lista, PtrNodoEnemigo nuevo);



//Inserta un nodo despues del nodo dado
void InsertarDespuesDe(ListaEnemigos &lista, PtrNodoEnemigo NodoAnterior, PtrNodoEnemigo nuevo);



//Elimina un nodo de la lista
PtrNodoEnemigo EliminarNodoListaEnemigos(ListaEnemigos& lista, PtrNodoEnemigo nodoABorrar);