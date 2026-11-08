#pragma once
#include <stdio.h>
#include <stdlib.h>

/*
 * Archivo: Globales.h
 * Descripcion: Constantes y enumeraciones globales que usamos en todo el juego.
 * Aqui definimos los tipos de enemigos, los colores del sistema de afinidad
 * y los tipos de eventos que se registran en la bitacora de cada partida.
 */

// Tipos de enemigos que pueden aparecer durante las oleadas
enum TipoEnemigo { DRON, GIRADOR, DIVISOR, MINI, TANQUE, CAMALEON };

// Colores disponibles tanto para la nave y sus disparos como para los enemigos
enum Color { CIAN, MAGENTA, AMBAR };

// Tipos de eventos que registramos en la bitacora (pila LIFO) para estadisticas
enum TipoEvento {
    BAJA,
    IMPACTO_ERRONEO,
    DANO_NUCLEO,
    CHOQUE_NAVE,
    PULSO,
    FIN_OLEADA,
    ELIMINACION
};
