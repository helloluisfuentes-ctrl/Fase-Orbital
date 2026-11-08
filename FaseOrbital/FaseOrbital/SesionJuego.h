#pragma once
#include <string>
#include <cstdint>
#include "Globales.h"
#include "Motor.h"
#include "Partida.h"
#include "BitacoraEventos.h"
#include "RondaJugadores.h"
#include "Bot.h"

// Modulo intermediario que conecta la logica de turnos (Partida) con la simulacion de fisica (Motor)
// Es completamente independiente de Allegro: lo utiliza tanto la interfaz grafica como el modo por lotes.

const int MAX_JUGADORES = 6;

// Estructura que acumula todas las metricas de un participante durante la partida completa (seccion 6.1)
struct MetricasJugador {
    int oleadaAlcanzada = 0;
    int disparos = 0;
    int correctos = 0;
    int erroneos = 0;
    int comboMax = 0;
    int pulsos = 0;
    int danoTotal = 0;
    int ticks = 0;
    int bajasTipo[6] = { 0, 0, 0, 0, 0, 0 };   // Bajas desglosadas por TipoEnemigo
    int bajasColor[3] = { 0, 0, 0 };          // Bajas desglosadas por Color
    int danoTipo[6] = { 0, 0, 0, 0, 0, 0 };    // Dano recibido desglosado por TipoEnemigo
    int correctosColor[3] = { 0, 0, 0 };
    int impactosColor[3] = { 0, 0, 0 };
    int posicion = 0;
    bool eliminado = false;
};

// Resumen del turno de una oleada para un jugador, calculado a partir de la bitacora
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

// Clase controladora de la sesion: administra participantes, instancias de partida, motor y persistencia
class SesionJuego {
private:
    Jugador* jugadores[MAX_JUGADORES];      // Arreglo de punteros a jugadores (la sesion es duena de su memoria)
    MetricasJugador metricas[MAX_JUGADORES];
    int totalJugadores;

    Partida* partida;
    Motor* motor;
    BitacoraEventos* bitacora;

    uint32_t semilla;
    int limiteOleadas;
    bool iniciada;
    bool turnoEnCurso;
    int indiceEnTurno;           // Indice del jugador en turno dentro del arreglo jugadores[]
    int orden[MAX_JUGADORES];    // orden[p] guarda el indice del jugador que ocupo la posicion p+1
    ResumenOleada ultimoResumen;

    int IndiceDe(const Jugador* j) const;
    void CalcularMetricasDesdeBitacora();
    void CalcularPosiciones();
    double Precision(int i) const;

public:
    SesionJuego();
    ~SesionJuego();

    // Regla de Tres: bloqueamos copia y asignacion para no duplicar punteros a jugadores ni al motor
    SesionJuego(const SesionJuego&) = delete;
    SesionJuego& operator=(const SesionJuego&) = delete;

    // Libera todos los recursos dinamicos y deja la sesion limpia para registrar nuevos participantes
    void Reiniciar();

    // Gestion de participantes previa al inicio de la partida
    bool RegistrarJugador(const std::string& alias, bool esBot);
    bool QuitarUltimoJugador();
    bool ExisteAlias(const std::string& alias) const;
    static bool EsAliasDeBot(const std::string& alias);

    void Iniciar(uint32_t semillaPartida, int maxOleadas);

    // Flujo del juego y turnos
    bool ComenzarTurno();                              // Prepara al participante en turno; retorna false si la partida termino
    EstadoMotor EjecutarTick(const AccionTick& accion); // Ejecuta un tick en el Motor
    AccionTick DecidirAccionBot() const;               // Aplica la heuristica fija oficial (seccion 5.2)
    void CerrarTurno(EstadoMotor estadoFinal);          // Recopila metricas y avanza en la ronda circular
    void Finalizar();                                   // Calcula metricas consolidadas y posiciones de clasificacion

    // Persistencia y reportes
    bool GuardarHistorial(const std::string& rutaCSV, std::string& mensaje) const;
    void ImprimirLineasTiempo() const;

    // Metodos de consulta para las vistas
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

    int GetIndiceEnPosicion(int p) const;
};
