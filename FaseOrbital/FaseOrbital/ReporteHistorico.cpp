#include "ReporteHistorico.h"
#include <cmath>

/*
 * Funcion: ObtenerAliasHistorial
 * Descripcion: Recorre la lista de registros enlazados y llena el arreglo destino con los alias distintos
 *              de jugadores sin duplicados para alimentar el selector de la pantalla de estadisticas.
 * Entradas: const HistorialPartidas& historial, std::string destino[], int maximo
 * Salidas: Retorna el numero total de jugadores distintos encontrados
 */
int ObtenerAliasHistorial(const HistorialPartidas& historial, std::string destino[], int maximo) {
    int total = 0;
    NodoRegistro* aux = historial.ObtenerCabeza();

    while (aux != NULL) {
        const std::string& alias = aux->registro.jugador;
        bool yaVisto = false;

        // Comprobamos si ya agregamos este alias previamente
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

/*
 * Funcion: CalcularReporteJugador
 * Descripcion: Procesa los registros de un jugador en 2 pasadas para calcular:
 *              1. Mejor/peor puntaje, partidas, victorias, bajas por tipo y media.
 *              2. Mediana copiando puntajes a ListaPuntajes y ordenando con Merge Sort.
 *              3. Desviacion estandar poblacional e histograma de 10 barras uniformes.
 * Entradas: const HistorialPartidas& historial, const std::string& alias, ReporteJugador& r (referencia)
 * Salidas: Retorna true si el jugador tiene registros y se calculo el reporte; false si no hay datos
 */
bool CalcularReporteJugador(const HistorialPartidas& historial, const std::string& alias, ReporteJugador& r) {
    r = ReporteJugador();
    r.alias = alias;

    long long suma = 0;
    int correctos = 0;
    int erroneos = 0;

    // Primera pasada: acumulamos puntajes, conteos de partidas, victorias y extremos
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

    // Calculo de media y precision de color
    r.media = static_cast<double>(suma) / r.partidas;
    if (correctos + erroneos > 0) {
        r.precision = static_cast<double>(correctos) / (correctos + erroneos);
    }

    // Calculo de mediana mediante Merge Sort sobre la lista auxiliar
    ListaPuntajes puntajes;
    historial.CopiarPuntajes(alias, puntajes);
    puntajes.MergeSort();
    r.mediana = puntajes.ObtenerMediana();

    // Segunda pasada: calculo de la varianza/desviacion estandar y distribucion del histograma
    double sumaCuadrados = 0.0;
    int rango = r.mejor - r.peor;
    aux = historial.ObtenerCabeza();
    while (aux != NULL) {
        const RegistroPartida& reg = aux->registro;
        if (reg.jugador == alias) {
            double dif = reg.puntaje - r.media;
            sumaCuadrados += dif * dif;

            // Clasificamos el puntaje en uno de los 10 intervalos para el histograma grafico
            int intervalo = 0;
            if (rango > 0) {
                intervalo = static_cast<int>((static_cast<long long>(reg.puntaje - r.peor) * 10) / rango);
                if (intervalo > 9) intervalo = 9; // El mejor puntaje cae en el ultimo intervalo
            }
            r.histograma[intervalo]++;
        }
        aux = aux->siguiente;
    }
    r.desviacion = std::sqrt(sumaCuadrados / r.partidas);

    return true;
}

/*
 * Funcion: ConstruirRanking
 * Descripcion: Recorre los registros del historial CSV e inserta cada partida en el Ranking con insercion ordenada.
 * Entradas: const HistorialPartidas& historial, Ranking& ranking (referencia a la lista ordenada destino)
 * Salidas: Ranking poblado y ordenado segun criterios de desempate
 */
void ConstruirRanking(const HistorialPartidas& historial, Ranking& ranking) {
    NodoRegistro* aux = historial.ObtenerCabeza();
    while (aux != NULL) {
        const RegistroPartida& reg = aux->registro;
        EntradaRanking entrada;
        entrada.alias = reg.jugador;
        entrada.puntaje = reg.puntaje;

        // Calculamos la precision de color de la partida
        if (reg.correctos + reg.erroneos > 0) {
            entrada.precisionColor = static_cast<double>(reg.correctos) / (reg.correctos + reg.erroneos);
        } else {
            entrada.precisionColor = 0.0;
        }

        ranking.InsertarOrdenado(entrada);
        aux = aux->siguiente;
    }
}
