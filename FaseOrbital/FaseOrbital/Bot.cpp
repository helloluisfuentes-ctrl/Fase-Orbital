#include "Bot.h"

/*
 * Funcion: DecidirAccion
 * Descripcion: Aplica la heuristica fija requerida por la catedra en la seccion 5.2:
 *              1. Selecciona como objetivo el enemigo mas cercano a la nave (menor r) que cumpla r > 100.
 *              2. Gira la nave hacia el angulo theta de ese enemigo y cambia al mismo color.
 *              3. Si la carga es 100 y hay 5 o mas enemigos a una distancia r < 160, activa la onda de pulso.
 * Entradas: double anguloActual, Color colorActual, int cargaPulso, const ListaEnemigos& listaEnemigos
 * Salidas: Estructura AccionTick con anguloObjetivo, colorObjetivo y activarPulso
 */
AccionTick Bot::DecidirAccion(double anguloActual, Color colorActual, int cargaPulso, const ListaEnemigos& listaEnemigos) {
    AccionTick accion;
    
    // Por defecto conservamos el angulo y color que la nave ya tiene
    accion.anguloObjetivo = anguloActual;
    accion.colorObjetivo = colorActual;
    accion.activarPulso = false;

    const Enemigo* mejorObjetivo = NULL;
    double menorR = 999999.0; // Valor inicial alto para la busqueda del minimo
    int enemigosCercanosPulso = 0; // Contador de enemigos en zona critica (r < 160)

    // Recorremos la lista de enemigos activos buscando el mas amenazante
    NodoEnemigo* aux = listaEnemigos.Primero();

    while (aux != NULL) {
        double r = aux->enemigo.r;

        // Verificamos si esta dentro del radio de peligro para activar pulso
        if (r < 160.0) {
            enemigosCercanosPulso++;
        }

        // Criterio de seleccion de blanco: menor distancia r que todavia no haya cruzado la orbita (r > 100)
        if (r > 100.0) {
            if (r < menorR) {
                menorR = r;
                mejorObjetivo = &(aux->enemigo);
            }
        }

        aux = aux->siguiente;
    }

    // Si encontramos un objetivo valido, alineamos la mira y emparejamos el color
    if (mejorObjetivo != NULL) {
        accion.anguloObjetivo = mejorObjetivo->theta;
        accion.colorObjetivo = mejorObjetivo->color;
    }

    // Regla de activacion del pulso orbital:
    // Carga completa en 100 y al menos 5 enemigos acumulados a menos de 160 unidades del centro
    if (cargaPulso >= 100 && enemigosCercanosPulso >= 5) {
        accion.activarPulso = true;
    }

    return accion;
}
