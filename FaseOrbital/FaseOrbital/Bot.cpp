#include "Bot.h"

AccionTick Bot::DecidirAccion(double anguloActual,Color colorActual,int cargaPulso,const ListaEnemigos& listaEnemigos) 
{
    AccionTick accion;
    
    // Por defecto se mantiene el ángulo y color actuales
    accion.anguloObjetivo = anguloActual;
    accion.colorObjetivo = colorActual;
    accion.activarPulso = false;

    const Enemigo* mejorObjetivo = NULL;
    double menorR = 999999.0; // Distancia inicial alta para buscar el mínimo
    int enemigosCercanosPulso = 0; // Contador de enemigos con r < 160

    // Recorrido de la ListaEnemigos
    NodoEnemigo* aux = listaEnemigos.Primero();

    while (aux != NULL) {
        double r = aux->enemigo.r;

        // Condición para el pulso: enemigos con r < 160
        if (r < 160.0) {
            enemigosCercanosPulso++;
        }

        // Búsqueda del objetivo: menor r entre los que cumplen r > 100.
        if (r > 100.0) {
            if (r < menorR) {
                menorR = r;
                mejorObjetivo = &(aux->enemigo);
            }
        }

        aux = aux->siguiente;
    }

    // Asignar acción de objetivo (si se encontró uno)
    if (mejorObjetivo != NULL) {
        accion.anguloObjetivo = mejorObjetivo->theta;
        accion.colorObjetivo = mejorObjetivo->color;
    }

    // Condición del pulso:
    // Carga de pulso en 100 y al menos 5 enemigos con r < 160
    if (cargaPulso >= 100 && enemigosCercanosPulso >= 5) {
        accion.activarPulso = true;
    }

    return accion;
}