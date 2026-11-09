#pragma once
#include "Globales.h"

// Estructura que guarda los atributos de un enemigo activo en el juego
struct Enemigo {
    int id;
    TipoEnemigo tipo;
    Color color;       // 1 = Cian, 2 = Magenta, 3 = Ámbar
    int vida;
    int radio;

    double r;        // Posición radial (distancia al núcleo)
    double theta;    // Ángulo en grados
    double omega;    // Velocidad de giro

    int ticksColor;  // Contador de ticks para el cambio de color del camaleón
};

// Nodo doble para la lista de enemigos (guarda puntero al siguiente y al anterior)
struct NodoEnemigo {
    Enemigo enemigo;
    NodoEnemigo* siguiente = NULL;
    NodoEnemigo* anterior = NULL;
};

// Lista doblemente enlazada con punteros a cabeza y cola para manipular enemigos en O(1)
class ListaEnemigos {
private:
    NodoEnemigo* cabeza;
    NodoEnemigo* cola;
    int tam;

public:
    // Constructor y Destructor
    ListaEnemigos();
    ~ListaEnemigos();

    // Regla de Tres: deshabilitamos copia y asignacion para evitar duplicar punteros a nodos
    ListaEnemigos(const ListaEnemigos&) = delete;
    ListaEnemigos& operator=(const ListaEnemigos&) = delete;

    // Metodos de modificacion
    void InsertarFinal(NodoEnemigo* nuevo);
    void InsertarDespues(NodoEnemigo* nodoAnterior, NodoEnemigo* nuevo);
    NodoEnemigo* Eliminar(NodoEnemigo* nodoABorrar);
    void Vaciar();

    // Metodos de consulta (getters)
    NodoEnemigo* Primero() const { return cabeza; }
    NodoEnemigo* Ultimo() const { return cola; }
    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == NULL; }
};



