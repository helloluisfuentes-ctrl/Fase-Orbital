#include "SesionJuego.h"
#include "HistorialPartidas.h"
#include <iostream>

/*
 * Funcion: Constructor SesionJuego
 * Descripcion: Inicializa la sesion con valores por defecto: punteros en NULL, semilla en 42 y arreglos vacios.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
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

/*
 * Funcion: Destructor ~SesionJuego
 * Descripcion: Libera toda la memoria dinamica de la sesion (jugadores, partida, motor y bitacora).
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
SesionJuego::~SesionJuego() {
    Reiniciar();
}

/*
 * Funcion: Reiniciar
 * Descripcion: Libera todas las instancias dinamicas activas y resetea las variables a su estado inicial.
 * Entradas: Ninguna
 * Salidas: Sesion vacia lista para nuevos registros
 */
void SesionJuego::Reiniciar() {
    delete partida;
    partida = NULL;
    delete motor;
    motor = NULL;
    delete bitacora;
    bitacora = NULL;

    // Liberamos cada jugador registrado
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

/*
 * Funcion: EsAliasDeBot
 * Descripcion: Comprueba si el alias comienza con el prefijo "BOT" segun la regla de la seccion 7.2.
 * Entradas: const std::string& alias
 * Salidas: Retorna true si empieza con "BOT", false en caso contrario
 */
bool SesionJuego::EsAliasDeBot(const std::string& alias) {
    return alias.size() >= 3 && alias[0] == 'B' && alias[1] == 'O' && alias[2] == 'T';
}

/*
 * Funcion: ExisteAlias
 * Descripcion: Verifica si ya existe otro jugador registrado con el mismo alias para evitar duplicados.
 * Entradas: const std::string& alias
 * Salidas: true si el alias ya esta en uso, false si esta disponible
 */
bool SesionJuego::ExisteAlias(const std::string& alias) const {
    for (int i = 0; i < totalJugadores; i++) {
        if (jugadores[i]->alias == alias) return true;
    }
    return false;
}

/*
 * Funcion: RegistrarJugador
 * Descripcion: Instancia un nuevo Jugador y lo agrega al arreglo si el alias es valido, no esta repetido,
 *              no contiene comas (para no danar el CSV) y no se ha alcanzado el tope de 6 participantes.
 * Entradas: const std::string& alias, bool esBot
 * Salidas: Retorna true si se registro con exito, false si fue rechazado
 */
bool SesionJuego::RegistrarJugador(const std::string& alias, bool esBot) {
    if (iniciada || totalJugadores >= MAX_JUGADORES) return false;
    if (alias.empty() || ExisteAlias(alias)) return false;
    if (alias.find(',') != std::string::npos) return false; // Evitamos romper el formato CSV

    Jugador* nuevo = new Jugador;
    nuevo->alias = alias;
    nuevo->esBot = esBot || EsAliasDeBot(alias);
    jugadores[totalJugadores] = nuevo;
    metricas[totalJugadores] = MetricasJugador();
    totalJugadores++;
    return true;
}

/*
 * Funcion: QuitarUltimoJugador
 * Descripcion: Remueve y libera el ultimo jugador agregado durante la pantalla de registro.
 * Entradas: Ninguna
 * Salidas: Retorna true si pudo quitar un jugador, false si no habia ninguno
 */
bool SesionJuego::QuitarUltimoJugador() {
    if (iniciada || totalJugadores == 0) return false;
    totalJugadores--;
    delete jugadores[totalJugadores];
    jugadores[totalJugadores] = NULL;
    return true;
}

/*
 * Funcion: Iniciar
 * Descripcion: Crea las instancias de Partida, Motor y BitacoraEventos, registra los participantes en
 *              la ronda circular y arranca la simulacion con la semilla dada.
 * Entradas: uint32_t semillaPartida, int maxOleadas
 * Salidas: Sesion marcada como iniciada
 */
void SesionJuego::Iniciar(uint32_t semillaPartida, int maxOleadas) {
    if (iniciada || totalJugadores == 0) return;

    semilla = semillaPartida;
    limiteOleadas = maxOleadas;

    partida = new Partida(semilla, limiteOleadas);
    motor = new Motor();
    bitacora = new BitacoraEventos();

    // Registramos a los jugadores en la ronda circular en el orden en que se anotaron
    for (int i = 0; i < totalJugadores; i++) {
        partida->RegistrarJugador(jugadores[i]);
    }
    partida->IniciarPartida();

    iniciada = true;
    turnoEnCurso = false;
}

/*
 * Funcion: IndiceDe
 * Descripcion: Busca la posicion de un puntero a jugador en el arreglo local jugadores[].
 * Entradas: const Jugador* j
 * Salidas: Indice entero en el rango [0, totalJugadores-1] o -1 si no se encuentra
 */
int SesionJuego::IndiceDe(const Jugador* j) const {
    for (int i = 0; i < totalJugadores; i++) {
        if (jugadores[i] == j) return i;
    }
    return -1;
}

/*
 * Funcion: EstaTerminada
 * Descripcion: Consulta si la partida ha finalizado por eliminacion de participantes o fin de oleadas.
 * Entradas: Ninguna
 * Salidas: Retorna true si la partida concluyo, false en caso contrario
 */
bool SesionJuego::EstaTerminada() const {
    return iniciada && (partida == NULL || partida->EstaTerminada());
}

/*
 * Funcion: GetJugadorEnTurno
 * Descripcion: Retorna el puntero al jugador que esta activo en este momento.
 * Entradas: Ninguna
 * Salidas: const Jugador* o NULL
 */
const Jugador* SesionJuego::GetJugadorEnTurno() const {
    if (!iniciada || partida == NULL) return NULL;
    return partida->ObtenerJugadorEnTurno();
}

/*
 * Funcion: GetOleadaActual
 * Descripcion: Devuelve el numero de la oleada que se esta disputando.
 * Entradas: Ninguna
 * Salidas: int numero de oleada
 */
int SesionJuego::GetOleadaActual() const {
    if (partida == NULL) return 0;
    return partida->ObtenerOleadaActual();
}

/*
 * Funcion: EsSentidoHorario
 * Descripcion: Consulta el sentido de rotacion de la ronda actual segun el orden serpenteante.
 * Entradas: Ninguna
 * Salidas: true = horario, false = antihorario
 */
bool SesionJuego::EsSentidoHorario() const {
    if (partida == NULL) return true;
    return partida->EsSentidoHorario();
}

/*
 * Funcion: ComenzarTurno
 * Descripcion: Prepara el turno del participante: clona profundamente la oleada desde la cola maestra
 *              y la entrega al Motor junto con la bitacora de eventos.
 * Entradas: Ninguna
 * Salidas: Retorna true si se inicio el turno, false si la partida ya termino
 */
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

/*
 * Funcion: EjecutarTick
 * Descripcion: Envia la accion al Motor para procesar un ciclo completo de tick.
 * Entradas: const AccionTick& accion
 * Salidas: EstadoMotor resultante del tick
 */
EstadoMotor SesionJuego::EjecutarTick(const AccionTick& accion) {
    if (!turnoEnCurso) return ESTADO_EN_JUEGO;
    return motor->ActualizarTick(accion);
}

/*
 * Funcion: DecidirAccionBot
 * Descripcion: Obtiene la decision de un bot mediante la heuristica oficial implementada en la clase Bot.
 * Entradas: Ninguna
 * Salidas: Estructura AccionTick con angulo, color y pulso
 */
AccionTick SesionJuego::DecidirAccionBot() const {
    const Jugador* j = motor->GetJugador();
    return Bot::DecidirAccion(j->anguloNave, j->faseColor, j->cargaPulso, motor->GetListaEnemigos());
}

/*
 * Funcion: CerrarTurno
 * Descripcion: Acumula las metricas del turno concluido, construye la estructura ResumenOleada recorriendo
 *              la bitacora y avanza el turno en la Partida segun sobrevivencia y orden serpenteante.
 * Entradas: EstadoMotor estadoFinal
 * Salidas: Metricas acumuladas y turno marcado como finalizado
 */
void SesionJuego::CerrarTurno(EstadoMotor estadoFinal) {
    if (!turnoEnCurso || indiceEnTurno < 0) return;

    Jugador* j = jugadores[indiceEnTurno];
    MetricasJugador& m = metricas[indiceEnTurno];
    int oleada = motor->GetNumeroOleada();
    int ticksTurno = motor->GetTickActual() + 1;

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

    // Resumen de la oleada: recorremos la bitacora filtrando por jugador y oleada actual
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

/*
 * Funcion: CalcularMetricasDesdeBitacora
 * Descripcion: Recorre la bitacora completa de la partida para consolidar el conteo de bajas por tipo/color,
 *              dano recibido y activaciones de pulso para cada jugador.
 * Entradas: Ninguna
 * Salidas: Arreglos de metricas actualizados
 */
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

/*
 * Funcion: Precision
 * Descripcion: Calcula el porcentaje de efectividad de disparo: correctos / (correctos + erroneos).
 * Entradas: int i (indice del participante)
 * Salidas: Valor decimal entre 0.0 y 1.0
 */
double SesionJuego::Precision(int i) const {
    int total = metricas[i].correctos + metricas[i].erroneos;
    if (total == 0) return 0.0;
    return static_cast<double>(metricas[i].correctos) / total;
}

/*
 * Funcion: CalcularPosiciones
 * Descripcion: Aplica el algoritmo de ordenamiento por insercion sobre el arreglo de indices para
 *              clasificar a los participantes segun los 3 criterios oficiales (seccion 5.1):
 *              1. Mayor puntaje, 2. Mayor oleada alcanzada, 3. Mayor precision de color.
 * Entradas: Ninguna
 * Salidas: Arreglo orden[] y metricas[].posicion asignados
 */
void SesionJuego::CalcularPosiciones() {
    for (int i = 0; i < totalJugadores; i++) orden[i] = i;

    // Ordenamiento por insercion sobre el arreglo de indices
    for (int i = 1; i < totalJugadores; i++) {
        int actual = orden[i];
        int k = i - 1;
        while (k >= 0) {
            int otro = orden[k];
            bool va_antes = false;
            // 1. Criterio de mayor puntaje
            if (jugadores[actual]->puntaje != jugadores[otro]->puntaje) {
                va_antes = jugadores[actual]->puntaje > jugadores[otro]->puntaje;
            }
            // 2. Desempate por mayor oleada alcanzada
            else if (metricas[actual].oleadaAlcanzada != metricas[otro].oleadaAlcanzada) {
                va_antes = metricas[actual].oleadaAlcanzada > metricas[otro].oleadaAlcanzada;
            }
            // 3. Desempate por mayor precision de color
            else {
                va_antes = Precision(actual) > Precision(otro);
            }
            if (!va_antes) break;
            orden[k + 1] = orden[k];
            k--;
        }
        orden[k + 1] = actual;
    }

    // Asignamos la posicion final a cada participante (1 = primer lugar)
    for (int p = 0; p < totalJugadores; p++) {
        metricas[orden[p]].posicion = p + 1;
    }
}

/*
 * Funcion: Finalizar
 * Descripcion: Consolida las metricas finales y calcula las posiciones del podio.
 * Entradas: Ninguna
 * Salidas: Estadisticas listas para persistir o graficar
 */
void SesionJuego::Finalizar() {
    if (!iniciada) return;
    CalcularMetricasDesdeBitacora();
    CalcularPosiciones();
}

/*
 * Funcion: GetIndiceEnPosicion
 * Descripcion: Consulta el indice del jugador que termino en la posicion p del podio.
 * Entradas: int p (posicion 1 a totalJugadores)
 * Salidas: Indice en jugadores[] o -1 si p es invalido
 */
int SesionJuego::GetIndiceEnPosicion(int p) const {
    if (p < 1 || p > totalJugadores) return -1;
    return orden[p - 1];
}

/*
 * Funcion: GuardarHistorial
 * Descripcion: Carga los registros previos de historial.csv, calcula el siguiente ID de partida,
 *              agrega una nueva fila por cada participante con sus 21 campos y guarda el archivo.
 * Entradas: const std::string& rutaCSV, std::string& mensaje (mensaje de salida)
 * Salidas: Retorna true si guardo exitosamente, false en caso de error
 */
bool SesionJuego::GuardarHistorial(const std::string& rutaCSV, std::string& mensaje) const {
    if (!iniciada) {
        mensaje = "No hay partida para guardar";
        return false;
    }

    HistorialPartidas historial;
    historial.CargarCSV(rutaCSV);

    // Calculamos el ID incremental para la nueva partida
    int idNuevo = 1;
    NodoRegistro* aux = historial.ObtenerCabeza();
    while (aux != NULL) {
        if (aux->registro.idPartida >= idNuevo) idNuevo = aux->registro.idPartida + 1;
        aux = aux->siguiente;
    }

    // Insertamos la fila de cada jugador en la lista enlazada del historial
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

/*
 * Funcion: ImprimirLineasTiempo
 * Descripcion: Imprime la cronologia por oleada de cada jugador llamando a BitacoraEventos.
 * Entradas: Ninguna
 * Salidas: Resumen por consola
 */
void SesionJuego::ImprimirLineasTiempo() const {
    if (bitacora == NULL) return;
    for (int i = 0; i < totalJugadores; i++) {
        bitacora->ImprimirLineaTiempo(jugadores[i]->alias);
    }
}
