#include "SesionJuego.h"
#include "HistorialPartidas.h"
#include <iostream>

//constructor
SesionJuego::SesionJuego() {
    for (int i = 0; i < MAX_JUGADORES; i++) {
        jugadores[i] = NULL;
        orden[i] = i;
    }
    totalJugadores = 0;
    partida = NULL;
    motor = NULL;
    bitacora = NULL;
    semilla = 42;
    limiteOleadas = 10;
    iniciada = false;
    turnoEnCurso = false;
    indiceEnTurno = -1;
}

//destructor
SesionJuego::~SesionJuego() {
    Reiniciar();
}

//libera toda la memoria de la sesion y la deja vacia
void SesionJuego::Reiniciar() {
    // primero la partida (su ronda solo guarda punteros a los jugadores, no los libera)
    delete partida;
    partida = NULL;
    delete motor;
    motor = NULL;
    delete bitacora;
    bitacora = NULL;

    for (int i = 0; i < MAX_JUGADORES; i++) {
        delete jugadores[i];
        jugadores[i] = NULL;
        metricas[i] = MetricasJugador();
        orden[i] = i;
    }
    totalJugadores = 0;
    iniciada = false;
    turnoEnCurso = false;
    indiceEnTurno = -1;
    ultimoResumen = ResumenOleada();
}

//true si el alias empieza con BOT (regla de la seccion 7.2)
bool SesionJuego::EsAliasDeBot(const std::string& alias) {
    return alias.size() >= 3 && alias[0] == 'B' && alias[1] == 'O' && alias[2] == 'T';
}

bool SesionJuego::ExisteAlias(const std::string& alias) const {
    for (int i = 0; i < totalJugadores; i++) {
        if (jugadores[i]->alias == alias) return true;
    }
    return false;
}

//registra un jugador nuevo, devuelve false si no se puede
bool SesionJuego::RegistrarJugador(const std::string& alias, bool esBot) {
    if (iniciada || totalJugadores >= MAX_JUGADORES) return false;
    if (alias.empty() || ExisteAlias(alias)) return false;
    if (alias.find(',') != std::string::npos) return false; // romperia el CSV

    Jugador* nuevo = new Jugador;
    nuevo->alias = alias;
    nuevo->esBot = esBot || EsAliasDeBot(alias);
    jugadores[totalJugadores] = nuevo;
    metricas[totalJugadores] = MetricasJugador();
    totalJugadores++;
    return true;
}

bool SesionJuego::QuitarUltimoJugador() {
    if (iniciada || totalJugadores == 0) return false;
    totalJugadores--;
    delete jugadores[totalJugadores];
    jugadores[totalJugadores] = NULL;
    return true;
}

//crea la partida, el motor y la bitacora, y mete a los jugadores en la ronda en orden de registro
void SesionJuego::Iniciar(uint32_t semillaPartida, int maxOleadas) {
    if (iniciada || totalJugadores == 0) return;

    semilla = semillaPartida;
    limiteOleadas = maxOleadas;

    partida = new Partida(semilla, limiteOleadas);
    motor = new Motor();
    bitacora = new BitacoraEventos();

    for (int i = 0; i < totalJugadores; i++) {
        partida->RegistrarJugador(jugadores[i]);
    }
    partida->IniciarPartida();

    iniciada = true;
    turnoEnCurso = false;
}

int SesionJuego::IndiceDe(const Jugador* j) const {
    for (int i = 0; i < totalJugadores; i++) {
        if (jugadores[i] == j) return i;
    }
    return -1;
}

bool SesionJuego::EstaTerminada() const {
    return iniciada && (partida == NULL || partida->EstaTerminada());
}

const Jugador* SesionJuego::GetJugadorEnTurno() const {
    if (!iniciada || partida == NULL) return NULL;
    return partida->ObtenerJugadorEnTurno();
}

int SesionJuego::GetOleadaActual() const {
    if (partida == NULL) return 0;
    return partida->ObtenerOleadaActual();
}

bool SesionJuego::EsSentidoHorario() const {
    if (partida == NULL) return true;
    return partida->EsSentidoHorario();
}

//prepara el turno del jugador actual con una copia profunda de la oleada
bool SesionJuego::ComenzarTurno() {
    if (!iniciada || EstaTerminada()) return false;

    Jugador* j = partida->ObtenerJugadorEnTurno();
    if (j == NULL) return false;

    indiceEnTurno = IndiceDe(j);
    int oleada = partida->ObtenerOleadaActual();

    partida->PrepararInicioTurno(j);
    ColaSpawn copia = partida->ObtenerCopiaOleadaActual();
    motor->IniciarTurno(j, oleada, copia, bitacora);

    metricas[indiceEnTurno].oleadaAlcanzada = oleada;
    turnoEnCurso = true;
    return true;
}

EstadoMotor SesionJuego::EjecutarTick(const AccionTick& accion) {
    if (!turnoEnCurso) return ESTADO_EN_JUEGO;
    return motor->ActualizarTick(accion);
}

//el bot decide con la heuristica fija de Bot
AccionTick SesionJuego::DecidirAccionBot() const {
    const Jugador* j = motor->GetJugador();
    return Bot::DecidirAccion(j->anguloNave, j->faseColor, j->cargaPulso, motor->GetListaEnemigos());
}

//junta las metricas del turno que termino, arma el resumen y avanza la ronda
void SesionJuego::CerrarTurno(EstadoMotor estadoFinal) {
    if (!turnoEnCurso || indiceEnTurno < 0) return;

    Jugador* j = jugadores[indiceEnTurno];
    MetricasJugador& m = metricas[indiceEnTurno];
    int oleada = motor->GetNumeroOleada();
    int ticksTurno = motor->GetTickActual() + 1; // el tick final tambien se jugo

    m.disparos += motor->GetDisparosTurno();
    m.correctos += motor->GetCorrectosTurno();
    m.erroneos += motor->GetErroneosTurno();
    if (motor->GetComboMaxTurno() > m.comboMax) m.comboMax = motor->GetComboMaxTurno();
    m.ticks += ticksTurno;
    for (int c = 0; c < 3; c++) {
        m.correctosColor[c] += motor->GetCorrectosColor(static_cast<Color>(c));
        m.impactosColor[c] += motor->GetImpactosColor(static_cast<Color>(c));
    }

    bool sobrevivio = (estadoFinal != ESTADO_JUGADOR_ELIMINADO);
    if (!sobrevivio) m.eliminado = true;

    // resumen de la oleada: recorrido de la bitacora filtrando por jugador y oleada
    ResumenOleada r;
    r.alias = j->alias;
    r.esBot = j->esBot;
    r.oleada = oleada;
    r.ticks = ticksTurno;
    r.vidaNucleo = j->vidaNucleo;
    r.puntajeTotal = j->puntaje;
    r.eliminado = !sobrevivio;

    NodoEvento* aux = bitacora->ObtenerCabeza();
    while (aux != NULL) {
        const Evento& e = aux->evento;
        if (e.jugador == j->alias && e.oleada == oleada) {
            if (e.tipoEvento == BAJA) {
                r.bajas++;
                r.puntosOleada += e.valor;
            }
            else if (e.tipoEvento == DANO_NUCLEO || e.tipoEvento == CHOQUE_NAVE) {
                r.danoRecibido += e.valor;
            }
            else if (e.tipoEvento == IMPACTO_ERRONEO) {
                r.erroneos++;
            }
        }
        aux = aux->siguiente;
    }
    ultimoResumen = r;

    partida->FinalizarTurnoActual(sobrevivio);
    turnoEnCurso = false;
}

//bajas por tipo y color, dano por tipo y pulsos salen de recorrer la bitacora completa
void SesionJuego::CalcularMetricasDesdeBitacora() {
    for (int i = 0; i < totalJugadores; i++) {
        MetricasJugador& m = metricas[i];
        m.pulsos = 0;
        m.danoTotal = 0;
        for (int t = 0; t < 6; t++) {
            m.bajasTipo[t] = 0;
            m.danoTipo[t] = 0;
        }
        for (int c = 0; c < 3; c++) m.bajasColor[c] = 0;
    }

    NodoEvento* aux = bitacora->ObtenerCabeza();
    while (aux != NULL) {
        const Evento& e = aux->evento;
        int i = -1;
        for (int k = 0; k < totalJugadores; k++) {
            if (jugadores[k]->alias == e.jugador) {
                i = k;
                break;
            }
        }

        if (i >= 0) {
            MetricasJugador& m = metricas[i];
            if (e.tipoEvento == BAJA) {
                m.bajasTipo[e.tipoEnemigo]++;
                m.bajasColor[e.color]++;
            }
            else if (e.tipoEvento == DANO_NUCLEO || e.tipoEvento == CHOQUE_NAVE) {
                m.danoTotal += e.valor;
                m.danoTipo[e.tipoEnemigo] += e.valor;
            }
            else if (e.tipoEvento == PULSO) {
                m.pulsos++;
            }
        }
        aux = aux->siguiente;
    }
}

double SesionJuego::Precision(int i) const {
    int total = metricas[i].correctos + metricas[i].erroneos;
    if (total == 0) return 0.0;
    return static_cast<double>(metricas[i].correctos) / total;
}

//posiciones: mayor puntaje, en empate mayor oleada alcanzada y luego mayor precision (seccion 5.1)
//son maximo 6 jugadores, se ordena con insercion sobre un arreglo de indices
void SesionJuego::CalcularPosiciones() {
    for (int i = 0; i < totalJugadores; i++) orden[i] = i;

    for (int i = 1; i < totalJugadores; i++) {
        int actual = orden[i];
        int k = i - 1;
        while (k >= 0) {
            int otro = orden[k];
            bool va_antes = false;
            if (jugadores[actual]->puntaje != jugadores[otro]->puntaje) {
                va_antes = jugadores[actual]->puntaje > jugadores[otro]->puntaje;
            }
            else if (metricas[actual].oleadaAlcanzada != metricas[otro].oleadaAlcanzada) {
                va_antes = metricas[actual].oleadaAlcanzada > metricas[otro].oleadaAlcanzada;
            }
            else {
                va_antes = Precision(actual) > Precision(otro);
            }
            if (!va_antes) break;
            orden[k + 1] = orden[k];
            k--;
        }
        orden[k + 1] = actual;
    }

    for (int p = 0; p < totalJugadores; p++) {
        metricas[orden[p]].posicion = p + 1;
    }
}

void SesionJuego::Finalizar() {
    if (!iniciada) return;
    CalcularMetricasDesdeBitacora();
    CalcularPosiciones();
}

int SesionJuego::GetIndiceEnPosicion(int p) const {
    if (p < 1 || p > totalJugadores) return -1;
    return orden[p - 1];
}

//agrega las filas de esta partida a historial.csv (carga lo que ya habia y vuelve a guardar)
bool SesionJuego::GuardarHistorial(const std::string& rutaCSV, std::string& mensaje) const {
    if (!iniciada) {
        mensaje = "No hay partida para guardar";
        return false;
    }

    HistorialPartidas historial;
    historial.CargarCSV(rutaCSV); // si no existe se crea uno nuevo

    int idNuevo = 1;
    NodoRegistro* aux = historial.ObtenerCabeza();
    while (aux != NULL) {
        if (aux->registro.idPartida >= idNuevo) idNuevo = aux->registro.idPartida + 1;
        aux = aux->siguiente;
    }

    for (int i = 0; i < totalJugadores; i++) {
        const Jugador* j = jugadores[i];
        const MetricasJugador& m = metricas[i];

        RegistroPartida reg;
        reg.idPartida = idNuevo;
        reg.semilla = semilla;
        reg.modo = "por_turnos";
        reg.jugador = j->alias;
        reg.esBot = j->esBot;
        reg.posicion = m.posicion;
        reg.puntaje = j->puntaje;
        reg.oleada = m.oleadaAlcanzada;
        reg.disparos = m.disparos;
        reg.correctos = m.correctos;
        reg.erroneos = m.erroneos;
        reg.comboMax = m.comboMax;
        reg.pulsos = m.pulsos;
        reg.danoTotal = m.danoTotal;
        reg.ticks = m.ticks;
        reg.bajasDron = m.bajasTipo[DRON];
        reg.bajasGirador = m.bajasTipo[GIRADOR];
        reg.bajasDivisor = m.bajasTipo[DIVISOR];
        reg.bajasMini = m.bajasTipo[MINI];
        reg.bajasTanque = m.bajasTipo[TANQUE];
        reg.bajasCamaleon = m.bajasTipo[CAMALEON];
        historial.AgregarFinal(reg);
    }

    if (!historial.GuardarCSV(rutaCSV)) {
        mensaje = "No se pudo escribir " + rutaCSV;
        return false;
    }
    mensaje = "Partida #" + std::to_string(idNuevo) + " guardada en " + rutaCSV;
    return true;
}

//linea de tiempo por oleada de cada jugador (seccion 6.2), se imprime en consola
void SesionJuego::ImprimirLineasTiempo() const {
    if (bitacora == NULL) return;
    for (int i = 0; i < totalJugadores; i++) {
        bitacora->ImprimirLineaTiempo(jugadores[i]->alias);
    }
}
