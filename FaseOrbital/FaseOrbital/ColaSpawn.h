#pragma once
#include "ListaEnemigos.h" 

// Los datos de cuándo y cómo nacerá el enemigo
struct Spawn {
    int tick;
    TipoEnemigo tipo;
    Color color;
    int angulo;
    int sentido; // 0 = horario, 1 = antihorario (solo para Girador)
};

typedef struct NodoSpawn {
    Spawn dato;
    NodoSpawn* sig;
}*PtrNodoSpawn;


typedef struct ColaSpawn {
    PtrNodoSpawn frente;
    PtrNodoSpawn final;
    int tam;
};


void InicializarCola(ColaSpawn& cola);
void DestruirCola(ColaSpawn& cola);
void Encolar(ColaSpawn& cola, Spawn nuevoSpawn);
bool Desencolar(ColaSpawn& cola, Spawn& spawnSaliente);
bool ColaVacia(ColaSpawn& cola);
void CopiarColaProfunda(ColaSpawn& origen, ColaSpawn& destino);