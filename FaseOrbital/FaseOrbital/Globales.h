#pragma once
#include <stdio.h>
#include <stdlib.h>

enum TipoEnemigo { DRON, GIRADOR, DIVISOR, MINI, TANQUE, CAMALEON };

enum Color { CIAN, MAGENTA, AMBAR };

enum TipoEvento {
    BAJA,
    IMPACTO_ERRONEO,
    DANO_NUCLEO,
    CHOQUE_NAVE,
    PULSO,
    FIN_OLEADA,
    ELIMINACION
};