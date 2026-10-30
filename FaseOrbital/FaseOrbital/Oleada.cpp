#include "Oleada.h"
#include "Generador.h"
#include "Globales.h"

void GenerarOleada(uint32_t semillaPartida, int numeroOleada, ColaSpawn& colaDestino) {

    // Genera Semilla de la oleada n: semilla_partida * 1000 + n 
    uint32_t semillaOleada = semillaPartida * 1000u + static_cast<uint32_t>(numeroOleada);
    Generador gen(semillaOleada);

    //Total de apariciones en oleada n: 6 + 3*n
    int totalApariciones = 6 + 3 * numeroOleada;

    // Guardamos los tipos en un arreglo:
    // Inicial: [DRON, DRON]
    // Si n >= 2: agrega GIRADOR
    // Si n >= 3: agrega DIVISOR
    // Si n >= 4: agrega TANQUE
    // Si n >= 5: agrega CAMALEON y GIRADOR
    TipoEnemigo conjunto[7];
    int tamConjunto = 0;

    conjunto[tamConjunto++] = DRON;
    conjunto[tamConjunto++] = DRON;

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

    int tickActual = 60; // La primera aparición ocurre en el tick 60

    for (int i = 0; i < totalApariciones; i++) {
        // Para apariciones posteriores a la primera, calculamos el intervalo
        if (i > 0) {
            uint32_t x_int = gen.Siguiente();
            int intervalo = 0;

            // Desde la oleada 7, si x mod 4 == 0 ocurre aparición doble (mismo tick)
            if (numeroOleada >= 7 && (x_int % 4 == 0)) {
                intervalo = 0;
            } else {
                int calcBase = 35 - 2 * numeroOleada;
                int base = (calcBase > 9) ? calcBase : 9; // máx(9, 35 - 2n)
                intervalo = static_cast<int>((base * (60 + (x_int % 81))) / 100);
            }
            tickActual += intervalo;
        }

        // Consumo estricto de números por aparición:
        // Tipo = x mod tamaño_del_conjunto
        uint32_t x_tipo = gen.Siguiente();
        TipoEnemigo tipo = conjunto[x_tipo % tamConjunto];

        // Color = x mod 3
        uint32_t x_color = gen.Siguiente();
        Color color = static_cast<Color>(x_color % 3);

        // Ángulo = x mod 360
        uint32_t x_angulo = gen.Siguiente();
        int angulo = static_cast<int>(x_angulo % 360);

        // Sentido (solo Girador) = x mod 2 (0 = horario, 1 = antihorario)
        int sentido = 0;
        if (tipo == GIRADOR) {
            uint32_t x_sentido = gen.Siguiente();
            sentido = static_cast<int>(x_sentido % 2);
        }

        // Crear nodo y encolar en ColaSpawn
        Spawn s;
        s.tick = tickActual;
        s.tipo = tipo;
        s.color = color;
        s.angulo = angulo;
        s.sentido = sentido;

        colaDestino.Encolar(s);
    }
}

