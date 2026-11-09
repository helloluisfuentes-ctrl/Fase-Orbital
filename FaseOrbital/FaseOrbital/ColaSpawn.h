#pragma once
#include "Globales.h"      // Para Color y TipoEnemigo

// Estructura que define la informacion de aparicion de un enemigo en la oleada
struct Spawn {
    int tick;
    TipoEnemigo tipo;
    Color color;
    int angulo;
    int sentido; // 0 = horario, 1 = antihorario (solo para Girador)
};

// Nodo simple para la cola de apariciones
struct NodoSpawn {
    Spawn dato;
    NodoSpawn* siguiente = NULL;
};

// Cola dinamica FIFO para gestionar el orden de aparicion de enemigos en la oleada
class ColaSpawn {
private:
    NodoSpawn* cabeza;
    NodoSpawn* cola;
    int tam;

public:
    // Constructor y Destructor
    ColaSpawn();
    ~ColaSpawn();

    // Regla de Tres: implementamos copia profunda para que cada jugador reciba su propia oleada independiente
    ColaSpawn(const ColaSpawn& otra);                // Constructor de copia
    ColaSpawn& operator=(const ColaSpawn& otra);     // Operador de asignación

    // Operaciones basicas de una Cola FIFO
    void Encolar(const Spawn& s);
    bool Desencolar(Spawn& out);
    const Spawn* VerFrente() const;

    // Consultas
    bool Vacia() const { return cabeza == NULL; }
    int Tamano() const { return tam; }
};