#pragma once
#include "Globales.h"      // Para Color y TipoEnemigo


struct Spawn {
    int tick;
    TipoEnemigo tipo;
    Color color;
    int angulo;
    int sentido; // 0 = horario, 1 = antihorario (solo para Girador)
};

struct NodoSpawn {
    Spawn dato;
    NodoSpawn* siguiente = NULL;
};

class ColaSpawn {
private:
    NodoSpawn* cabeza;
    NodoSpawn* cola;
    int tam;

public:
    // Constructor y Destructor
    ColaSpawn();
    ~ColaSpawn();

    
    ColaSpawn(const ColaSpawn& otra);                // Constructor de copia
    ColaSpawn& operator=(const ColaSpawn& otra);     // Operador de asignación

    // Operaciones básicas de una Cola 
    void encolar(const Spawn& s);
    bool desencolar(Spawn& out);
    const Spawn* verFrente() const;

    // Consultas
    bool vacia() const { return cabeza == NULL; }
    int tamano() const { return tam; }
};