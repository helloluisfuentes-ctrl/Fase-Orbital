#pragma once
#include <string>
#include <cstdint>
#include "Globales.h"
#include "Motor.h"
#include "Partida.h"
#include "BitacoraEventos.h"
#include "RondaJugadores.h"
#include "Bot.h"

// SesionJuego une la Partida (turnos y oleadas) con el Motor (ticks) y lleva las metricas de cada jugador.
// NO depende de Allegro: la interfaz grafica la usa, y el modo por lotes tambien la puede usar.

const int MAX_JUGADORES = 6;

// metricas acumuladas de un jugador durante toda la partida (seccion 6.1)
struct MetricasJugador {
    int oleadaAlcanzada = 0;
    int disparos = 0;
    int correctos = 0;
    int erroneos = 0;
    int comboMax = 0;
    int pulsos = 0;
    int danoTotal = 0;
    int ticks = 0;
    int bajasTipo[6] = { 0, 0, 0, 0, 0, 0 };   // indice = TipoEnemigo
    int bajasColor[3] = { 0, 0, 0 };          // indice = Color
    int danoTipo[6] = { 0, 0, 0, 0, 0, 0 };    // dano recibido por tipo de enemigo
    int correctosColor[3] = { 0, 0, 0 };
    int impactosColor[3] = { 0, 0, 0 };
    int posicion = 0;
    bool eliminado = false;
};

// resumen de un turno (una oleada jugada por un jugador), se calcula recorriendo la bitacora
struct ResumenOleada {
    std::string alias;
    bool esBot = false;
    int oleada = 0;
    int bajas = 0;
    int puntosOleada = 0;
    int danoRecibido = 0;
    int erroneos = 0;
    int ticks = 0;
    int vidaNucleo = 0;
    int puntajeTotal = 0;
    bool eliminado = false;
};

class SesionJuego {
private:
    Jugador* jugadores[MAX_JUGADORES];      // en el orden en que se registraron (la sesion es la duena de la memoria)
    MetricasJugador metricas[MAX_JUGADORES];
    int totalJugadores;

    Partida* partida;
    Motor* motor;
    BitacoraEventos* bitacora;

    uint32_t semilla;
    int limiteOleadas;
    bool iniciada;
    bool turnoEnCurso;
    int indiceEnTurno;           // indice en jugadores[] del que esta jugando
    int orden[MAX_JUGADORES];    // orden[p] = indice del jugador que quedo en la posicion p+1
    ResumenOleada ultimoResumen;

    int IndiceDe(const Jugador* j) const;
    void CalcularMetricasDesdeBitacora();
    void CalcularPosiciones();
    double Precision(int i) const;

public:
    SesionJuego();
    ~SesionJuego();

    // no se copia: es duena de jugadores, partida, motor y bitacora
    SesionJuego(const SesionJuego&) = delete;
    SesionJuego& operator=(const SesionJuego&) = delete;

    // libera todo y deja la sesion vacia, lista para registrar jugadores otra vez
    void Reiniciar();

    // registro (antes de Iniciar). Un alias que empieza con BOT siempre es bot
    bool RegistrarJugador(const std::string& alias, bool esBot);
    bool QuitarUltimoJugador();
    bool ExisteAlias(const std::string& alias) const;
    static bool EsAliasDeBot(const std::string& alias);

    void Iniciar(uint32_t semillaPartida, int maxOleadas);

    // flujo de turnos
    bool ComenzarTurno();                              // prepara al jugador en turno; false si la partida termino
    EstadoMotor EjecutarTick(const AccionTick& accion); // un tick del motor
    AccionTick DecidirAccionBot() const;               // heuristica de la seccion 5.2
    void CerrarTurno(EstadoMotor estadoFinal);          // acumula metricas y avanza la ronda
    void Finalizar();                                   // calcula metricas finales y posiciones

    // persistencia y reportes
    bool GuardarHistorial(const std::string& rutaCSV, std::string& mensaje) const;
    void ImprimirLineasTiempo() const;

    // consultas para la vista
    bool EstaIniciada() const { return iniciada; }
    bool EstaTerminada() const;
    bool TurnoEnCurso() const { return turnoEnCurso; }
    int GetTotalJugadores() const { return totalJugadores; }
    const Jugador* GetJugador(int i) const { return jugadores[i]; }
    const MetricasJugador& GetMetricas(int i) const { return metricas[i]; }
    int GetIndiceEnTurno() const { return indiceEnTurno; }
    const Jugador* GetJugadorEnTurno() const;
    int GetOleadaActual() const;
    bool EsSentidoHorario() const;
    int GetLimiteOleadas() const { return limiteOleadas; }
    uint32_t GetSemilla() const { return semilla; }
    const Motor* GetMotor() const { return motor; }
    const BitacoraEventos* GetBitacora() const { return bitacora; }
    const ResumenOleada& GetUltimoResumen() const { return ultimoResumen; }
    double GetPrecision(int i) const { return Precision(i); }

    // indice del jugador que quedo en la posicion p (1 = primero), -1 si no existe
    int GetIndiceEnPosicion(int p) const;
};
