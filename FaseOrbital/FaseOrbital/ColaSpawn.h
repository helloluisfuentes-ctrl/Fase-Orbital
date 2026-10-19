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
    void Encolar(const Spawn& s);
    bool Desencolar(Spawn& out);
    const Spawn* VerFrente() const;

    // Consultas
    bool Vacia() const { return cabeza == NULL; }
    int Tamano() const { return tam; }
};