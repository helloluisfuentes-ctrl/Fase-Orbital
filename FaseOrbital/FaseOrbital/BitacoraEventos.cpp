#include "BitacoraEventos.h"
#include <iostream>

/*
 * Funcion: Constructor BitacoraEventos
 * Descripcion: Inicializa la bitacora vacia, dejando la cabeza y cola en NULL y tamano en cero.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
BitacoraEventos::BitacoraEventos() {
    cabeza = NULL;
    cola = NULL;
    tam = 0;
}

/*
 * Funcion: Destructor ~BitacoraEventos
 * Descripcion: Recorre la lista de eventos desde la cabeza liberando la memoria de cada nodo con delete.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
BitacoraEventos::~BitacoraEventos() {
    // Usamos aux para guardar el puntero antes de avanzar la cabeza y borrar
    NodoEvento* aux = cabeza;
    while (aux != NULL) {
        cabeza = cabeza->siguiente;
        delete aux;
        aux = cabeza;
    }
    cola = NULL;
    tam = 0;
}

/*
 * Funcion: AgregarFinal
 * Descripcion: Registra un nuevo evento al final de la lista cronologica en tiempo O(1) usando el puntero cola.
 * Entradas: const Evento& e (estructura con los datos del tick, oleada, jugador y tipo de accion)
 * Salidas: Bitacora actualizada con el nuevo evento enlazado
 */
void BitacoraEventos::AgregarFinal(const Evento& e) {
    NodoEvento* nuevoNodo = new NodoEvento;
    nuevoNodo->evento = e;
    nuevoNodo->siguiente = NULL;

    // Si la lista esta vacia, el nuevo nodo es cabeza y cola a la vez
    if (cabeza == NULL) {
        cabeza = nuevoNodo;
        cola = nuevoNodo;
    } else {
        cola->siguiente = nuevoNodo;
        cola = nuevoNodo;
    }
    tam++;
}

/*
 * Funcion: ImprimirLineaTiempo
 * Descripcion: Recorre la bitacora filtrando por el alias del jugador e imprime un resumen consolidado
 *              de bajas, dano recibido y puntaje acumulado por cada oleada jugada.
 * Entradas: const std::string& aliasJugador (nombre del jugador a consultar)
 * Salidas: Imprime la linea de tiempo en la consola estandar
 */
void BitacoraEventos::ImprimirLineaTiempo(const std::string& aliasJugador) const {
    std::cout << "\n=== LINEA DE TIEMPO: " << aliasJugador << " ===\n";

    NodoEvento* aux = cabeza;
    int oleadaActual = -1;
    int bajas = 0;
    int danoRecibido = 0;
    int puntajeAcumulado = 0;
    bool tieneEventos = false;

    // Recorremos secuencialmente todos los eventos registrados
    while (aux != NULL) {
        if (aux->evento.jugador == aliasJugador) {
            tieneEventos = true;

            // Si detectamos cambio de oleada, imprimimos el resumen acumulado de la oleada anterior
            if (oleadaActual != -1 && aux->evento.oleada != oleadaActual) {
                std::cout << "Oleada " << oleadaActual 
                          << " -> Bajas: " << bajas 
                          << " | Daño recibido: " << danoRecibido 
                          << " | Puntaje acumulado: " << puntajeAcumulado << "\n";
                
                // Reiniciamos los acumuladores para la nueva oleada
                bajas = 0;
                danoRecibido = 0;
            }

            oleadaActual = aux->evento.oleada;

            // Clasificamos el evento para sumar a sus respectivos acumuladores
            if (aux->evento.tipoEvento == BAJA) {
                bajas++;
                puntajeAcumulado += aux->evento.valor;
            } 
            else if (aux->evento.tipoEvento == DANO_NUCLEO || aux->evento.tipoEvento == CHOQUE_NAVE) {
                danoRecibido += aux->evento.valor;
            }
        }
        aux = aux->siguiente;
    }

    // Imprimimos la ultima oleada que quedo pendiente despues de salir del while
    if (oleadaActual != -1) {
        std::cout << "Oleada " << oleadaActual 
                  << " -> Bajas: " << bajas 
                  << " | Daño recibido: " << danoRecibido 
                  << " | Puntaje acumulado: " << puntajeAcumulado << "\n";
    }

    if (!tieneEventos) {
        std::cout << "No hay eventos registrados para este jugador.\n";
    }
}
