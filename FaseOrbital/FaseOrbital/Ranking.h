#pragma once
#include <string>

struct EntradaRanking {
    std::string alias;
    int puntaje;
    double precisionColor; 
};

struct NodoRanking {
    EntradaRanking dato;
    NodoRanking* siguiente = NULL;
};

class Ranking {
private:
    NodoRanking* cabeza;
    int tam;

    bool EsMejor(const EntradaRanking& a, const EntradaRanking& b) const;

public:
    Ranking();
    ~Ranking();

    // regla de Tres
    Ranking(const Ranking&) = delete;
    Ranking& operator=(const Ranking&) = delete;

    //Metodos
    void InsertarOrdenado(const EntradaRanking& entrada);
    void Top(int k = 10) const; // Imprime top 10, o lo que diga k

    int Tamano() const { return tam; }
    bool Vacia() const { return cabeza == NULL; }
};