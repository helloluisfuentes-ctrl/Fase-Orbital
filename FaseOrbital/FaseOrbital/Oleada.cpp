#include "Oleada.h"
#include "Generador.h"
#include "Globales.h"

/*
 * Funcion: GenerarOleada
 * Descripcion: Construye la secuencia completa de apariciones de enemigos para la oleada n en una ColaSpawn.
 *              Calcula la semilla de la oleada (semilla * 1000 + n), define los tipos disponibles segun el nivel,
 *              calcula los intervalos entre apariciones y consume los valores del LCG en orden riguroso.
 * Entradas: uint32_t semillaPartida, int numeroOleada, ColaSpawn& colaDestino
 * Salidas: ColaSpawn poblada con las estructuras Spawn listas para ser desencoladas durante la partida
 */
void GenerarOleada(uint32_t semillaPartida, int numeroOleada, ColaSpawn& colaDestino) {

    // Semilla obligatoria de la oleada n segun especificacion: semilla_partida * 1000 + n
    uint32_t semillaOleada = semillaPartida * 1000u + static_cast<uint32_t>(numeroOleada);
    Generador gen(semillaOleada);

    // Total de apariciones en oleada n: 6 + 3*n
    int totalApariciones = 6 + 3 * numeroOleada;

    // Arreglo estatico con los tipos de enemigos disponibles para esta oleada
    TipoEnemigo conjunto[7];
    int tamConjunto = 0;

    // Conjunto base inicial: [DRON, DRON]
    conjunto[tamConjunto++] = DRON;
    conjunto[tamConjunto++] = DRON;

    // A partir de la oleada 2 se van incorporando tipos mas avanzados
    if (numeroOleada >= 2) {
        conjunto[tamConjunto++] = GIRADOR;
    }
    if (numeroOleada >= 3) {
        conjunto[tamConjunto++] = DIVISOR;
    }
    if (numeroOleada >= 4) {
        conjunto[tamConjunto++] = TANQUE;
    }
    if (numeroOleada >= 5) {
        conjunto[tamConjunto++] = CAMALEON;
        conjunto[tamConjunto++] = GIRADOR;
    }

    int tickActual = 60; // La primera aparicion ocurre siempre en el tick 60

    // Generamos cada una de las apariciones programadas
    for (int i = 0; i < totalApariciones; i++) {
        // A partir de la segunda aparicion calculamos el intervalo de ticks
        if (i > 0) {
            uint32_t x_int = gen.Siguiente();
            int intervalo = 0;

            // Desde la oleada 7, si x mod 4 == 0 ocurre aparicion doble (en el mismo tick)
            if (numeroOleada >= 7 && (x_int % 4 == 0)) {
                intervalo = 0;
            } else {
                int calcBase = 35 - 2 * numeroOleada;
                int base = (calcBase > 9) ? calcBase : 9; // max(9, 35 - 2n)
                intervalo = static_cast<int>((base * (60 + (x_int % 81))) / 100);
            }
            tickActual += intervalo;
        }

        // Consumo estricto de numeros del LCG por cada enemigo (Seccion 3.6):
        // 1. Tipo = x mod tamano_del_conjunto
        uint32_t x_tipo = gen.Siguiente();
        TipoEnemigo tipo = conjunto[x_tipo % tamConjunto];

        // 2. Color = x mod 3 (0=Cian, 1=Magenta, 2=Ambar)
        uint32_t x_color = gen.Siguiente();
        Color color = static_cast<Color>(x_color % 3);

        // 3. Angulo = x mod 360
        uint32_t x_angulo = gen.Siguiente();
        int angulo = static_cast<int>(x_angulo % 360);

        // 4. Sentido (solo para Girador) = x mod 2 (0=horario, 1=antihorario)
        int sentido = 0;
        if (tipo == GIRADOR) {
            uint32_t x_sentido = gen.Siguiente();
            sentido = static_cast<int>(x_sentido % 2);
        }

        // Empaquetamos los datos en la estructura Spawn y encolamos
        Spawn s;
        s.tick = tickActual;
        s.tipo = tipo;
        s.color = color;
        s.angulo = angulo;
        s.sentido = sentido;

        colaDestino.Encolar(s);
    }
}
