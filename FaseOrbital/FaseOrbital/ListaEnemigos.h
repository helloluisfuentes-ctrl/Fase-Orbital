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

struct NodoEnemigo {
    Enemigo enemigo;
    NodoEnemigo* siguiente = NULL;
    NodoEnemigo* anterior = NULL;
}*PtrNodoEnemigo;

class ListaEnemigos {
private:
    NodoEnemigo* cabeza;
    NodoEnemigo* cola;
    int tam;

public:
    //Constructor y Destructor
    ListaEnemigos();
    ~ListaEnemigos();

    //Regla de Tres
    ListaEnemigos(const ListaEnemigos&) = delete;
    ListaEnemigos& operator=(const ListaEnemigos&) = delete;

    //metodos
    void insertarFinal(NodoEnemigo* nuevo);
    void insertarDespues(NodoEnemigo* nodoAnterior, NodoEnemigo* nuevo);
    NodoEnemigo* eliminar(NodoEnemigo* nodoABorrar);

    
    NodoEnemigo* primero() const { return cabeza; }
    NodoEnemigo* ultimo() const { return cola; }
    int tamano() const { return tam; }
    bool vacia() const { return cabeza == NULL; }
};



