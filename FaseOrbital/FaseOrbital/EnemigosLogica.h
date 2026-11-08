#pragma once
#include "Globales.h"
#include "ListaEnemigos.h"
#include "ColaSpawn.h"

// Modulo de cinematica, creacion y comportamiento particular de los 6 tipos de enemigos

// Instancia un nuevo enemigo con sus propiedades fisicas al salir de la ColaSpawn
Enemigo CrearEnemigo(const Spawn& spawn, int NumeroOleada, int id);

// Calcula la velocidad radial v segun la oleada y la escala por el tipo de enemigo
double ObtenerVelocidadRadial(TipoEnemigo tipo, int NumeroOleada);

// Retorna la puntuacion base otorgada al destruir un enemigo de determinado tipo
int ObtenerPuntosEnemigo(TipoEnemigo enemigo);

// Actualiza las coordenadas polares (r, theta), el giro de espiral y el color del camaleon
void MoverEnemigo(Enemigo& e, int NumeroOleada);

// Genera los 2 enemigos Mini e inserta inmediatamente despues del nodo Divisor antes de borrarlo
void GenerarMinis(NodoEnemigo* nodoDivisor, ListaEnemigos& listaEnemigos, int NumeroOleada, int& SiguienteID);

// Determina los 2 colores distintos al color del padre para asignar a los Minis
void ObtenerColoresDistintos(Color colorPadre, Color& color1, Color& color2);
