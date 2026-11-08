#pragma once
#include <cstdint>
#include "ColaSpawn.h"

/*
 * Funcion: GenerarOleada
 * Descripcion: Genera todos los enemigos correspondientes a la oleada dada siguiendo el orden estricto
 *              de consumo de numeros pseudoaleatorios del LCG especificado en la seccion 3.6 del PDF.
 * Entradas: uint32_t semillaPartida, int numeroOleada, ColaSpawn& colaDestino
 * Salidas: Llena la ColaSpawn con los nodos de aparicion ordenados cronologicamente por tick
 */
void GenerarOleada(uint32_t semillaPartida, int numeroOleada, ColaSpawn& colaDestino);