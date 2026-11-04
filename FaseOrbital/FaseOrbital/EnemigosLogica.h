#pragma once
#include "Globales.h"
#include "ListaEnemigos.h"
#include "ColaSpawn.h"

// configura un enemigo con los atributos segun el tipo y la oleada al salir de la ColaSpawn
Enemigo CrearEnemigo(const Spawn& spawn, int NumeroOleada, int id);

// obtiene la velocidad radial de un enemigo
double ObtenerVelocidadRadial(TipoEnemigo tipo, int NumeroOleada);

// devuelve los puntos que da el enemigo al destruirse
int ObtenerPuntosEnemigo(TipoEnemigo enemigo);

// actualiza la posicion (r, theta), gira giradores y tambien cambia el color de los camaleones
void MoverEnemigo(Enemigo& e, int NumeroOleada);

// cuando muere un divisor genera los 2 minis y los agrega a ListaEnemigos
void GenerarMinis(NodoEnemigo* nodoDivisor, ListaEnemigos& listaEnemigos, int NumeroOleada, int& SiguienteID);

// obtiene 2 colores diferentes al ColorPadre para los minis
void ObtenerColoresDistintos(Color colorPadre, Color& color1, Color& color2);
