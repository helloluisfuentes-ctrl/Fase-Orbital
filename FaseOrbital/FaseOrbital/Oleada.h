#pragma once
#include <cstdint>
#include "ColaSpawn.h"

// Llena la ColaSpawn con todos los enemigos de la oleada n de forma determinista
void GenerarOleada(uint32_t semillaPartida, int numeroOleada, ColaSpawn& colaDestino);

 