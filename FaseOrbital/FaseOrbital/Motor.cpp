#include "Motor.h"
#include "EnemigosLogica.h"

const double PI = 3.14159265358979323846;

Motor::Motor() {
	jugador = nullptr;
	NumeroOleada = 1;
	tickActual = 0;
	siguienteIdEnemigo = 1;
	pulsoActivo = false;
	rDeOndaPulso = 20.0;
	bitacora = nullptr;
	disparosTurno = 0;
	correctosTurno = 0;
	erroneosTurno = 0;
	comboMaxTurno = 0;
	for (int i = 0; i < 3; i++) {
		correctosColor[i] = 0;
		impactosColor[i] = 0;
	}
}

Motor::~Motor() {
	// no hace falta hacer nada porque las estructuras tienen sus propios destructores y Motor no tiene memoria dinámica propia
}

void Motor::PolarACartesiano(double r, double thetaGrados, double& x, double& y) const {
	double thetaRadianes = thetaGrados * (PI / 180.0);
	x = r * cos(thetaRadianes);
	y = r * sin(thetaRadianes);
}

double Motor::DistanciaMasCorta(double x1, double y1, double x2, double y2) const {
	double dx = x2 - x1;
	double dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

int Motor::CalcularMultiplicador(int combo) const {
	int mult = 1 + (combo / 6);
	return (mult > 8) ? 8 : mult;
}

double Motor::CalcularGiroNave(double anguloActual, double anguloObjetivo){
	double dif = anguloObjetivo - anguloActual;
	while (dif > 180.0) dif -= 360.0;
	while (dif < -180.0) dif += 360.0;

	double nuevoAngulo = anguloActual;
	if (abs(dif) <= 6.0) {
		nuevoAngulo = anguloObjetivo;
	}
	else if (dif > 0.0) {
		nuevoAngulo += 6.0;
	}
	else {
		nuevoAngulo -= 6.0;
	}

	while (nuevoAngulo >= 360.0) nuevoAngulo -= 360.0;
	while (nuevoAngulo < 0.0) nuevoAngulo += 360.0;

	return nuevoAngulo;
}

void Motor::IniciarTurno(Jugador* j, int oleada, const ColaSpawn& colaMaestra, BitacoraEventos* bitacoraPartida) {
	jugador = j;
	NumeroOleada = oleada;
	tickActual = 0;
	siguienteIdEnemigo = 1;
	bitacora = bitacoraPartida;
	pulsoActivo = false;
	rDeOndaPulso = 20.0;
	disparosTurno = 0;
	correctosTurno = 0;
	erroneosTurno = 0;
	comboMaxTurno = 0;
	for (int i = 0; i < 3; i++) {
		correctosColor[i] = 0;
		impactosColor[i] = 0;
	}

	jugador->anguloNave = 270.0; // inicia siempre en 270
	jugador->combo = 0; // el combo se reinicia al inicio de cada turno

	// Limpiar cualquier enemigo o bala remanente del turno anterior
	listaEnemigos.Vaciar();
	listaBalas.Vaciar();

	colaSpawn = colaMaestra; // copia de la cola maestra para el turno actual
}

EstadoMotor Motor::ActualizarTick(const AccionTick& accion) {
	// actualizar la fase del color
	jugador->faseColor = accion.colorObjetivo;

	// actualizar el pulso (se activa con carga 100)
	if (accion.activarPulso && jugador->cargaPulso >= 100 && !pulsoActivo) {
		pulsoActivo = true;
		rDeOndaPulso = 20.0;
		jugador->cargaPulso = 0; // vuelve a 0 al activarse el pulso
		
		if (bitacora != nullptr) {
			bitacora->AgregarFinal({tickActual, NumeroOleada, jugador->alias, PULSO, DRON, jugador->faseColor, 0});
		}

	}

	// actualizar angulo de la nave
	jugador->anguloNave = CalcularGiroNave(jugador->anguloNave, accion.anguloObjetivo);

	// descolar de ColaSpawn hacia ListaEnemigos
	while (!colaSpawn.Vacia() && colaSpawn.VerFrente()->tick <= tickActual) {
		Spawn sp;
		colaSpawn.Desencolar(sp);

		// crear enemigo e insertarlo al final de la lista de enemigos
		Enemigo nuevoE = CrearEnemigo(sp, NumeroOleada, siguienteIdEnemigo++);
		NodoEnemigo* nuevoNodo = new NodoEnemigo;
		nuevoNodo->enemigo = nuevoE;
		nuevoNodo->anterior = nullptr;
		nuevoNodo->siguiente = nullptr;
		listaEnemigos.InsertarFinal(nuevoNodo);

	}


	// disparo automatico cuando el tickActual es multiplo de 4
	if (tickActual % 4 == 0) {
		disparosTurno++;
		double rad = jugador->anguloNave * (PI / 180.0);

		Bala b;

		// se crea en la orbita (r = 100)
		b.cord_x = 100.0 * cos(rad);
		b.cord_y = 100.0 * sin(rad);

		// se mueve 9 hacia afuera por tick
		b.speed_x = 9.0 * cos(rad);
		b.speed_y = 9.0 * sin(rad);
	
		b.color = jugador->faseColor;
		b.duenio = 1;

		NodoBala* nuevoNodo = new NodoBala;
		nuevoNodo->bala = b;
		nuevoNodo->siguiente = nullptr;
		listaBalas.AgregarInicio(nuevoNodo);
	}

	// mover enemigos y ciclos de camaleones
	NodoEnemigo* nodoE = listaEnemigos.Primero();
	while (nodoE != nullptr) {
		MoverEnemigo(nodoE->enemigo, NumeroOleada);
		nodoE = nodoE->siguiente;
	}

	// resolver los choques entre enemigos y nucleo, y enemigos y naves
	double naveX, naveY;
	PolarACartesiano(100.0, jugador->anguloNave, naveX, naveY);

	nodoE = listaEnemigos.Primero();
	while (nodoE != nullptr) {
		Enemigo& e = nodoE->enemigo;
		double ex, ey;
		PolarACartesiano(e.r, e.theta, ex, ey);

		// choque con nucleo
		if (e.r <= (20.0 + 0.4 * e.radio)) {
			int dano = (e.tipo == TANQUE) ? 22 : 10;
			jugador->vidaNucleo -= dano;
			jugador->combo = 0; // se pierde el combo al recibir daño

			if (bitacora != nullptr) {
				bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, DANO_NUCLEO, e.tipo, e.color, dano });
			}

			nodoE = listaEnemigos.Eliminar(nodoE);

		}

		// enemigo choca con la nave
		else if (DistanciaMasCorta(ex, ey, naveX, naveY) <= (e.radio + 5.0)) {
			// choque con la nave
			int dano = 12;
			jugador->vidaNucleo -= dano;
			jugador->combo = 0; // se pierde el combo al recibir daño
			
			if (bitacora != nullptr) {
				bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, CHOQUE_NAVE, e.tipo, e.color, dano });
			}
			
			nodoE = listaEnemigos.Eliminar(nodoE);
		
		}
		
		
		else {
			nodoE = nodoE->siguiente;
		}

	}


	// mover balas y resolver choques con enemigos
	NodoBala** ptrBala = listaBalas.ObtenerPtrCabeza();

	while (*ptrBala != nullptr) {
		Bala& b = (*ptrBala)->bala;

		// mover la bala
		b.cord_x += b.speed_x;
		b.cord_y += b.speed_y;

		double distCentroSq = b.cord_x * b.cord_x + b.cord_y * b.cord_y;

		// si la bala esta fuera de la orbita, se elimina
		if (distCentroSq > (320.0 * 320.0)) {
			NodoBala* victima = *ptrBala;
			*ptrBala = victima->siguiente;
			delete victima;
			continue; // ptrBala no avanza porque ya apunta al siguiente
		}

		// verificar choque con enemigos
		bool balaDestruida = false;
		NodoEnemigo* auxE = listaEnemigos.Primero();

		while (auxE != nullptr && !balaDestruida) {
			Enemigo& e = auxE->enemigo;
			double ex, ey;
			PolarACartesiano(e.r, e.theta, ex, ey);

			double distBalaEnemigo = DistanciaMasCorta(b.cord_x, b.cord_y, ex, ey);




			// si la bala entra en el radio del enemigo
			if (distBalaEnemigo <= (e.radio + 3.5)) {
				balaDestruida = true;
				Color colorBala = b.color;
				impactosColor[static_cast<int>(colorBala)]++;

				// bala desaparece despues de chocar
				NodoBala* victima = *ptrBala;
				*ptrBala = victima->siguiente;
				delete victima;

				// caso 1 de color correcto
				if (colorBala == e.color) {
					correctosTurno++;
					correctosColor[static_cast<int>(colorBala)]++;
					e.vida -= 1;

					if (jugador->combo + 1 > comboMaxTurno) {
						comboMaxTurno = jugador->combo + 1;
					}

					if (e.vida <= 0) {
						int mult = CalcularMultiplicador(jugador->combo);
						int puntos = ObtenerPuntosEnemigo(e.tipo) * mult;

						jugador->puntaje += puntos;
						jugador->combo += 1;

						// carga de pulso
						int carga = (e.tipo == TANQUE) ? 14 : 5;
						jugador->cargaPulso = min(100, jugador->cargaPulso + carga);

						if (bitacora != nullptr) {
							bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, BAJA, e.tipo, e.color, puntos });
						}


						// si es divisor se insertan los minis antes de eliminar al divisor
						if (e.tipo == DIVISOR) {
							GenerarMinis(auxE, listaEnemigos, NumeroOleada, siguienteIdEnemigo);
						}

						listaEnemigos.Eliminar(auxE);

					}

				}

				// caso 2 de color incorrecto
				else {
					erroneosTurno++;
					e.r += 9.0;         // se retrocede 9 hacia afuera
					jugador->combo = 0;  // se resetea el combo
					if (bitacora != nullptr) {
						bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, IMPACTO_ERRONEO, e.tipo, e.color, 0 });
					}
				}
			}

			if (!balaDestruida) {
				auxE = auxE->siguiente;
			}
		}

		// si la bala no choco con ningún enemigo, pasa al siguiente nodo
		if (!balaDestruida) {
			ptrBala = &((*ptrBala)->siguiente);
		}
	}
	



	// avanzar onda de pulso y resolver las bajas
	if (pulsoActivo) {

		rDeOndaPulso += 12.0;
		
		if (rDeOndaPulso > 320.0) {
			pulsoActivo = false;
		}

		else {
			// destruye a todo enemigo con |r - r_onda| < radio + 3 sin importar el color

			NodoEnemigo* auxE = listaEnemigos.Primero();
			
			while (auxE != nullptr) {
				Enemigo& e = auxE->enemigo;

				if (abs(e.r - rDeOndaPulso) < (e.radio + 3.0)) {
					// suma puntos multiplicados (no sube combo ni pulso)
					int mult = CalcularMultiplicador(jugador->combo);
					int puntos = ObtenerPuntosEnemigo(e.tipo) * mult;
					jugador->puntaje += puntos;

					if (bitacora != nullptr) {
						bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, BAJA, e.tipo, e.color, puntos });
					}
					
					if (e.tipo == DIVISOR) {
						GenerarMinis(auxE, listaEnemigos, NumeroOleada, siguienteIdEnemigo);
					}
					
					auxE = listaEnemigos.Eliminar(auxE);
				}
				
				else {
					auxE = auxE->siguiente;
				}
			}
		}
	}

	// chequear fin de oleada y fin de partida
	if (jugador->vidaNucleo <= 0) {
		jugador->vidaNucleo = 0;

		if (bitacora != nullptr) {
			bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, ELIMINACION, DRON, jugador->faseColor, 0});
		}

		return ESTADO_JUGADOR_ELIMINADO;
	}

	// se completo la oleada??
	if (colaSpawn.Vacia() && listaEnemigos.Vacia()) {
		// se le da +8 al nucleo al terminar la oleada con max 100
		jugador->vidaNucleo = min(100, jugador->vidaNucleo + 8);

		if (bitacora != nullptr) {
			bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, FIN_OLEADA, DRON, jugador->faseColor, 0 });
		}

		return ESTADO_FIN_OLEADA;

	}


	// avanza un tick
	tickActual++;
	return ESTADO_EN_JUEGO;
	


}

