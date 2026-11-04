#pragma once
#include <cmath>
#include "Globales.h"
#include "ListaEnemigos.h"
#include "ListaBalas.h"
#include "ColaSpawn.h"
#include "RondaJugadores.h"
#include "BitacoraEventos.h"
#include "Bot.h"

enum EstadoMotor {
	ESTADO_EN_JUEGO,
	ESTADO_FIN_OLEADA,
	ESTADO_JUGADOR_ELIMINADO
};

class Motor {

private:
	Jugador* jugador;	// ountero al jugador que esta jugando el turno actualmente
	int NumeroOleada;
	int tickActual;
	int siguienteIdEnemigo;		// contador para asignar un id unico a cada enemigo

	ListaEnemigos listaEnemigos;
	ListaBalas listaBalas;
	ColaSpawn colaSpawn;	// copia de la oleada actual
	BitacoraEventos* bitacora; // puntero a la bitacora de la partida

	bool pulsoActivo;
	double rDeOndaPulso;

	double CalcularGiroNave(double anguloActual, double anguloObjetivo);
	void PolarACartesiano(double r, double thetaGrados, double& x, double& y) const;
	double DistanciaMasCorta(double x1, double y1, double x2, double y2) const;
	int CalcularMultiplicador(int combo) const;


public:
	Motor();
	~Motor();

	// evitar copias del motor
	Motor(const Motor&) = delete; // evita que se cree un Motor copiando otro
	Motor& operator=(const Motor&) = delete; // evita que a un Motor se le asigne otro

	// inicializacion del turno de un jugador con una oleada
	void IniciarTurno(Jugador* j, int oleada, const ColaSpawn& colaMaestra, BitacoraEventos* bitacoraPartida);

	// ejecuta 1 tick del juego, devolviendo el estado del motor
	EstadoMotor ActualizarTick(const AccionTick& accion);

	// Getters
	const ListaEnemigos& GetListaEnemigos() const { return listaEnemigos; };
	const ListaBalas& GetListaBalas() const { return listaBalas; };
	const ColaSpawn& GetColaSpawn() const { return colaSpawn; };
	Jugador* GetJugador() const { return jugador; };
	int GetTickActual() const { return tickActual; };
	int GetNumeroOleada() const { return NumeroOleada; };
	bool EsPulsoActivo() const { return pulsoActivo; };
	double GetRDeOndaPulso() const { return rDeOndaPulso; };

};