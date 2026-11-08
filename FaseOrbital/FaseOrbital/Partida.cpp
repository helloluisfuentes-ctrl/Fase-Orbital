#include "Partida.h"
#include <iostream>

/*
 * Funcion: Constructor Partida
 * Descripcion: Configura la partida con la semilla del generador, el limite de oleadas (10 por defecto),
 *              inicializa la oleada en 1, sentido horario y marca la partida como activa.
 * Entradas: uint32_t semillaPartida, int maxOleadas
 * Salidas: Instancia creada
 */
Partida::Partida(uint32_t semillaPartida, int maxOleadas) {
    semilla = semillaPartida;
    oleadaActual = 1;
    limiteOleadas = maxOleadas;
    totalJugadoresIniciales = 0;
    sentidoHorario = true; // Rondas impares arrancan siempre en sentido horario
    jugadoresTurnoRestantes = 0;
    terminada = false;
}

/*
 * Funcion: Destructor ~Partida
 * Descripcion: La estructura ronda libera sus propios nodos al destruirse de forma automatica.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
Partida::~Partida() {
}

/*
 * Funcion: RegistrarJugador
 * Descripcion: Agrega un participante a la ronda circular e incrementa el contador inicial de jugadores.
 * Entradas: Jugador* j (puntero al jugador a registrar)
 * Salidas: Participante insertado en la lista circular
 */
void Partida::RegistrarJugador(Jugador* j) {
    ronda.Agregar(j);
    totalJugadoresIniciales++;
}

/*
 * Funcion: IniciarPartida
 * Descripcion: Verifica que existan jugadores registrados y arranca la primera ronda de la oleada 1.
 * Entradas: Ninguna
 * Salidas: Partida iniciada o marcada como terminada si no habian participantes
 */
void Partida::IniciarPartida() {
    if (ronda.Vacia()) {
        terminada = true;
        return;
    }
    IniciarNuevaRonda();
}

/*
 * Funcion: IniciarNuevaRonda
 * Descripcion: Prepara una nueva ronda: calcula el sentido serpenteante (impar horario, par antihorario),
 *              vacia la cola maestra anterior, genera los enemigos con la semilla y resetea el contador de turnos.
 * Entradas: Ninguna
 * Salidas: ColaSpawn maestra poblada y turnos listos para la nueva oleada
 */
void Partida::IniciarNuevaRonda() {
    if (oleadaActual > limiteOleadas || ronda.Vacia()) {
        terminada = true;
        return;
    }

    // Regla de orden serpenteante: rondas impares horario (true), rondas pares antihorario (false)
    sentidoHorario = (oleadaActual % 2 != 0);

    // Vaciamos cualquier residuo que haya quedado en la cola maestra de la oleada previa
    while (!colaMaestraOleada.Vacia()) {
        Spawn basura;
        colaMaestraOleada.Desencolar(basura);
    }
    GenerarOleada(semilla, oleadaActual, colaMaestraOleada);

    // En esta ronda deben jugar todos los jugadores que permanezcan con vida en la lista circular
    jugadoresTurnoRestantes = ronda.Tamano();
}

/*
 * Funcion: ObtenerJugadorEnTurno
 * Descripcion: Retorna el puntero al jugador que le corresponde jugar en el tick actual de la ronda.
 * Entradas: Ninguna
 * Salidas: Puntero al Jugador en turno
 */
Jugador* Partida::ObtenerJugadorEnTurno() const {
    return ronda.VerActual();
}

/*
 * Funcion: ObtenerCopiaOleadaActual
 * Descripcion: Genera una copia profunda independiente de la cola maestra para que el jugador
 *              consuma sus enemigos sin alterar la cola original de los demas companeros.
 * Entradas: Ninguna
 * Salidas: Instancia de ColaSpawn con sus propios nodos clonados
 */
ColaSpawn Partida::ObtenerCopiaOleadaActual() const {
    ColaSpawn copia(colaMaestraOleada); // Invocamos el constructor de copia profunda
    return copia;
}

/*
 * Funcion: PrepararInicioTurno
 * Descripcion: Reinicia los valores temporales del jugador para el nuevo turno: el combo vuelve a 0
 *              y la nave se posiciona en el angulo inicial de 270 grados (apuntando hacia abajo).
 * Entradas: Jugador* j (jugador que inicia su turno)
 * Salidas: Estructura del jugador reiniciada para jugar la oleada
 */
void Partida::PrepararInicioTurno(Jugador* j) {
    if (j == NULL) return;

    // El combo se reinicia al inicio de cada turno; la vida del nucleo y la carga de pulso se conservan
    j->combo = 0;
    j->anguloNave = 270.0; 
}

/*
 * Funcion: FinalizarTurnoActual
 * Descripcion: Procesa el final de la oleada de un jugador: si murio se remueve de la ronda circular;
 *              si sobrevivio, avanza el turno al siguiente respetando el orden serpenteante.
 *              Al completar la ronda, verifica si la partida debe terminar o avanzar a la siguiente oleada.
 * Entradas: bool sobrevivio (true si logro superar la oleada, false si su nucleo llego a cero)
 * Salidas: Estado de la partida y de la lista circular actualizado
 */
void Partida::FinalizarTurnoActual(bool sobrevivio) {
    if (ronda.Vacia()) {
        terminada = true;
        return;
    }

    Jugador* jActual = ronda.VerActual();

    // Si el nucleo llego a 0 o no sobrevivio, lo eliminamos de la lista circular
    if (!sobrevivio || (jActual != NULL && jActual->vidaNucleo <= 0)) {
        ronda.EliminarActual(sentidoHorario);
    } 
    else {
        // Si no es el ultimo turno de la ronda, avanzamos al siguiente jugador en la lista circular
        if (jugadoresTurnoRestantes > 1) {
            ronda.Avanzar(sentidoHorario);
        }
        // Cuando jugadoresTurnoRestantes == 1 (el ultimo de la ronda),
        // NO avanzamos: asi la siguiente ronda arranca exactamente con el ultimo jugador (orden serpenteante)
    }

    jugadoresTurnoRestantes--;

    // Verificamos si todos los participantes activos ya completaron su turno en esta oleada
    if (jugadoresTurnoRestantes <= 0) {
        // En multijugador (2 o mas jugadores), la partida termina si solo queda un jugador vivo tras cerrar la ronda
        if (totalJugadoresIniciales > 1 && ronda.Tamano() <= 1) {
            terminada = true;
            return;
        }

        oleadaActual++;
        // Verificamos si superamos el limite de oleadas configurado o si no quedan jugadores
        if (oleadaActual > limiteOleadas || ronda.Vacia()) {
            terminada = true;
        } else {
            IniciarNuevaRonda();
        }
    }
}

/*
 * Funcion: DeterminarGanador
 * Descripcion: Determina cual participante gano la partida comparando los puntajes registrados.
 * Entradas: Jugador* todosLosRegistrados[], int totalRegistrados
 * Salidas: Retorna el puntero al Jugador ganador
 */
Jugador* Partida::DeterminarGanador(Jugador* todosLosRegistrados[], int totalRegistrados) const {
    if (totalRegistrados <= 0) return NULL;

    Jugador* ganador = todosLosRegistrados[0];

    // Recorremos el arreglo de participantes registrados comparando resultados
    for (int i = 1; i < totalRegistrados; i++) {
        Jugador* cand = todosLosRegistrados[i];

        if (cand->puntaje > ganador->puntaje) {
            ganador = cand;
        }
        // En caso de empate en puntaje, desempatamos por vida restante de nucleo
        else if (cand->puntaje == ganador->puntaje) {
            if (cand->vidaNucleo > ganador->vidaNucleo) {
                ganador = cand;
            }
        }
    }

    return ganador;
}
