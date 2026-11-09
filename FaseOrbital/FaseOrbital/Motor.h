#pragma once
#include <cmath>
#include "Globales.h"
#include "ListaEnemigos.h"
#include "ListaBalas.h"
#include "ColaSpawn.h"
#include "RondaJugadores.h"
#include "BitacoraEventos.h"
#include "Bot.h"

/*
 * Archivo: Motor.h
 * Descripcion: Modulo central que ejecuta las reglas, colisiones y fisica del juego
 * de manera desacoplada de la interfaz grafica de Allegro.
 * Sigue un ciclo estricto de 10 pasos por tick (aparicion, cinemática, colisiones,
 * pulso electromagnetico, limpieza de balas y condiciones de fin de turno).
 */

// Estados posibles que devuelve el motor tras procesar un tick
enum EstadoMotor {
	ESTADO_EN_JUEGO,
	ESTADO_FIN_OLEADA,
	ESTADO_JUGADOR_ELIMINADO
};

class Motor {
private:
	Jugador* jugador;	       // Puntero al jugador que tiene el turno activo
	int NumeroOleada;
	int tickActual;
	int siguienteIdEnemigo;    // Identificador incremental para cada enemigo nuevo

	ListaEnemigos listaEnemigos;
	ListaBalas listaBalas;
	ColaSpawn colaSpawn;	   // Copia profunda de la oleada consumida en este turno
	BitacoraEventos* bitacora; // Puntero a la bitacora de eventos compartida

	bool pulsoActivo;
	double rDeOndaPulso;

	int disparosTurno;
	int correctosTurno;
	int erroneosTurno;
	int comboMaxTurno;
	int correctosColor[3];
	int impactosColor[3];

	// Metodos matematicos y geometricos auxiliares
	double CalcularGiroNave(double anguloActual, double anguloObjetivo);
	void PolarACartesiano(double r, double thetaGrados, double& x, double& y) const;
	double DistanciaMasCorta(double x1, double y1, double x2, double y2) const;
	int CalcularMultiplicador(int combo) const;

public:
	Motor();
	~Motor();

	// Regla de Tres: bloqueamos copia y asignacion para evitar duplicar el motor del juego
	Motor(const Motor&) = delete;
	Motor& operator=(const Motor&) = delete;

	// Inicializacion del turno de un jugador con su oleada
	void IniciarTurno(Jugador* j, int oleada, const ColaSpawn& colaMaestra, BitacoraEventos* bitacoraPartida);

	// Procesa 1 tick de simulacion aplicando los 10 pasos obligatorios
	EstadoMotor ActualizarTick(const AccionTick& accion);

	// Metodos de consulta (getters)
	const ListaEnemigos& GetListaEnemigos() const { return listaEnemigos; }
	const ListaBalas& GetListaBalas() const { return listaBalas; }
	const ColaSpawn& GetColaSpawn() const { return colaSpawn; }
	Jugador* GetJugador() const { return jugador; }
	int GetTickActual() const { return tickActual; }
	int GetNumeroOleada() const { return NumeroOleada; }
	bool EsPulsoActivo() const { return pulsoActivo; }
	double GetRDeOndaPulso() const { return rDeOndaPulso; }

	int GetDisparosTurno() const { return disparosTurno; }
	int GetCorrectosTurno() const { return correctosTurno; }
	int GetErroneosTurno() const { return erroneosTurno; }
	int GetComboMaxTurno() const { return comboMaxTurno; }
	int GetCorrectosColor(Color c) const { return correctosColor[static_cast<int>(c)]; }
	int GetImpactosColor(Color c) const { return impactosColor[static_cast<int>(c)]; }
	int GetMultiplicador() const { return CalcularMultiplicador(jugador ? jugador->combo : 0); }
};
