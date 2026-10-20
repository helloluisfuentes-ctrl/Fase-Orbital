#include "BitacoraEventos.h"
#include <iostream>

//constructor
BitacoraEventos::BitacoraEventos(){
    cabeza = NULL;
    cola = NULL;
    tam = 0;
}

//Destructor
BitacoraEventos::~BitacoraEventos(){
    NodoEvento* aux = cabeza;
    while (aux != NULL){
        cabeza = cabeza->siguiente;
        delete(aux);
        aux = cabeza;
    }
    cola = NULL;
    tam = 0;
}

//Agrega un nodo al final, como encolar
void BitacoraEventos::AgregarFinal(const Evento& e){
    NodoEvento* nuevoNodo = new NodoEvento;
    nuevoNodo->evento = e;
    nuevoNodo->siguiente = NULL;

    if (cabeza == NULL){
        cabeza = nuevoNodo;
        cola = nuevoNodo;
    } else {
        cola->siguiente = nuevoNodo;
        cola = nuevoNodo;
    }
    tam++;
}

//imprime en pantalla los eventos de un jugador determinado
void BitacoraEventos::ImprimirLineaTiempo(const std::string& aliasJugador) const {
    std::cout << "\n=== LINEA DE TIEMPO: " << aliasJugador << " ===\n";

    NodoEvento* aux = cabeza;
    int oleadaActual = -1;
    int bajas = 0;
    int danoRecibido = 0;
    int puntajeAcumulado = 0;
    bool tieneEventos = false;

    while (aux != NULL) {
        if (aux->evento.jugador == aliasJugador) {
            tieneEventos = true;

            // Si se cambia de oleada, imprimimos el resumen de la oleada anterior
            if (oleadaActual != -1 && aux->evento.oleada != oleadaActual) {
                std::cout << "Oleada " << oleadaActual 
                          << " -> Bajas: " << bajas 
                          << " | Daño recibido: " << danoRecibido 
                          << " | Puntaje acumulado: " << puntajeAcumulado << "\n";
                
                // Reiniciamos contadores para la nueva oleada
                bajas = 0;
                danoRecibido = 0;
            }

            oleadaActual = aux->evento.oleada;
            //avanzamos la oleada

            // Procesar el evento actual
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

    // Imprimir la última oleada que quedó pendiente
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