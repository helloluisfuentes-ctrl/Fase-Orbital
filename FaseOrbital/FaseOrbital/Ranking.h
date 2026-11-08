#pragma once
#include <string>

// Registro para almacenar las estadisticas de un jugador en la tabla de clasificacion
struct EntradaRanking {
    std::string alias;
    int puntaje;
    double precisionColor; 
};

// Nodo simple para la lista enlazada ordenada del ranking
struct NodoRanking {
    EntradaRanking dato;
    NodoRanking* siguiente = NULL;
};

// Lista simplemente enlazada ordenada con insercion ordenada por criterios de desempate
class Ranking {
private:
    NodoRanking* cabeza;
    int tam;

    // Metodo privado que compara dos entradas segun las reglas de desempate del curso
    bool EsMejor(const EntradaRanking& a, const EntradaRanking& b) const;

public:
    Ranking();
    ~Ranking();

    // Regla de Tres: eliminamos copia y asignacion para evitar duplicados en memoria
    Ranking(const Ranking&) = delete;
    Ranking& operator=(const Ranking&) = delete;

    // Metodos principales
    void InsertarOrdenado(const EntradaRanking& entrada);
    void Top(int k = 10) const; // Imprime los primeros k registros (por defecto el Top 10)

    // Consultas
    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == NULL; }
    NodoRanking* ObtenerCabeza() const { return cabeza; }
};