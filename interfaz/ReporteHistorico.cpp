#include "ReporteHistorico.h"
#include <cmath>

int ObtenerAliasHistorial(const HistorialPartidas& historial, std::string destino[], int maximo) {
    int total = 0;
    NodoRegistro* aux = historial.ObtenerCabeza();
    while (aux != NULL) {
        const std::string& alias = aux->registro.jugador;
        bool yaVisto = false;
        for (int i = 0; i < total; i++) {
            if (destino[i] == alias) {
                yaVisto = true;
                break;
            }
        }
        if (!yaVisto && total < maximo) {
            destino[total++] = alias;
        }
        aux = aux->siguiente;
    }
    return total;
}

bool CalcularReporteJugador(const HistorialPartidas& historial, const std::string& alias, ReporteJugador& r) {
    r = ReporteJugador();
    r.alias = alias;

    long long suma = 0;
    int correctos = 0;
    int erroneos = 0;

    // primera pasada: conteos, mejor, peor y suma
    NodoRegistro* aux = historial.ObtenerCabeza();
    while (aux != NULL) {
        const RegistroPartida& reg = aux->registro;
        if (reg.jugador == alias) {
            if (r.partidas == 0) {
                r.mejor = reg.puntaje;
                r.peor = reg.puntaje;
            }
            else {
                if (reg.puntaje > r.mejor) r.mejor = reg.puntaje;
                if (reg.puntaje < r.peor) r.peor = reg.puntaje;
            }
            r.partidas++;
            if (reg.posicion == 1) r.victorias++;
            if (reg.oleada > r.oleadaMaxima) r.oleadaMaxima = reg.oleada;
            if (reg.esBot) r.esBot = true;
            suma += reg.puntaje;
            correctos += reg.correctos;
            erroneos += reg.erroneos;
            r.bajasTipo[DRON] += reg.bajasDron;
            r.bajasTipo[GIRADOR] += reg.bajasGirador;
            r.bajasTipo[DIVISOR] += reg.bajasDivisor;
            r.bajasTipo[MINI] += reg.bajasMini;
            r.bajasTipo[TANQUE] += reg.bajasTanque;
            r.bajasTipo[CAMALEON] += reg.bajasCamaleon;
        }
        aux = aux->siguiente;
    }

    if (r.partidas == 0) return false;

    r.media = static_cast<double>(suma) / r.partidas;
    if (correctos + erroneos > 0) {
        r.precision = static_cast<double>(correctos) / (correctos + erroneos);
    }

    // mediana: copia a lista auxiliar + merge sort sobre la lista
    ListaPuntajes puntajes;
    historial.CopiarPuntajes(alias, puntajes);
    puntajes.MergeSort();
    r.mediana = puntajes.ObtenerMediana();

    // segunda pasada: desviacion estandar poblacional e histograma
    double sumaCuadrados = 0.0;
    int rango = r.mejor - r.peor;
    aux = historial.ObtenerCabeza();
    while (aux != NULL) {
        const RegistroPartida& reg = aux->registro;
        if (reg.jugador == alias) {
            double dif = reg.puntaje - r.media;
            sumaCuadrados += dif * dif;

            int intervalo = 0;
            if (rango > 0) {
                intervalo = static_cast<int>((static_cast<long long>(reg.puntaje - r.peor) * 10) / rango);
                if (intervalo > 9) intervalo = 9; // el maximo cae en el ultimo intervalo
            }
            r.histograma[intervalo]++;
        }
        aux = aux->siguiente;
    }
    r.desviacion = std::sqrt(sumaCuadrados / r.partidas);

    return true;
}

void ConstruirRanking(const HistorialPartidas& historial, Ranking& ranking) {
    NodoRegistro* aux = historial.ObtenerCabeza();
    while (aux != NULL) {
        const RegistroPartida& reg = aux->registro;

        EntradaRanking entrada;
        entrada.alias = reg.jugador;
        entrada.puntaje = reg.puntaje;
        entrada.precisionColor = 0.0;
        if (reg.correctos + reg.erroneos > 0) {
            entrada.precisionColor = static_cast<double>(reg.correctos) / (reg.correctos + reg.erroneos);
        }

        ranking.InsertarOrdenado(entrada);
        aux = aux->siguiente;
    }
}
