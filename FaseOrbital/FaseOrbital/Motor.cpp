#include "Motor.h"
#include "EnemigosLogica.h"

const double PI = 3.14159265358979323846;

/*
 * Funcion: Constructor Motor
 * Descripcion: Inicializa las variables de simulacion, contadores de aciertos/errores y banderas del motor.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
Motor::Motor() {
	jugador = NULL;
	NumeroOleada = 1;
	tickActual = 0;
	siguienteIdEnemigo = 1;
	pulsoActivo = false;
	rDeOndaPulso = 20.0;
	bitacora = NULL;
	disparosTurno = 0;
	correctosTurno = 0;
	erroneosTurno = 0;
	comboMaxTurno = 0;
	for (int i = 0; i < 3; i++) {
		correctosColor[i] = 0;
		impactosColor[i] = 0;
	}
}

/*
 * Funcion: Destructor ~Motor
 * Descripcion: Las estructuras miembros (listas y cola) liberan su propia memoria con sus destructores.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
Motor::~Motor() {
}

/*
 * Funcion: PolarACartesiano
 * Descripcion: Convierte coordenadas polares (r, theta en grados) a coordenadas cartesianas (x, y) en pantalla.
 * Entradas: double r, double thetaGrados, double& x, double& y (referencias de salida)
 * Salidas: Modifica x e y con las componentes calculadas
 */
void Motor::PolarACartesiano(double r, double thetaGrados, double& x, double& y) const {
	double thetaRadianes = thetaGrados * (PI / 180.0);
	x = r * cos(thetaRadianes);
	y = r * sin(thetaRadianes);
}

/*
 * Funcion: DistanciaMasCorta
 * Descripcion: Calcula la distancia euclidiana entre dos puntos (x1, y1) y (x2, y2).
 * Entradas: double x1, double y1, double x2, double y2
 * Salidas: Retorna la distancia en linea recta
 */
double Motor::DistanciaMasCorta(double x1, double y1, double x2, double y2) const {
	double dx = x2 - x1;
	double dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

/*
 * Funcion: CalcularMultiplicador
 * Descripcion: Aplica la formula de multiplicador de puntaje: min(8, 1 + floor(combo / 6)).
 * Entradas: int combo (racha de aciertos consecutivos del jugador)
 * Salidas: Factor multiplicador entero entre 1 y 8
 */
int Motor::CalcularMultiplicador(int combo) const {
	int mult = 1 + (combo / 6);
	return (mult > 8) ? 8 : mult;
}

/*
 * Funcion: CalcularGiroNave
 * Descripcion: Gira la nave hacia el angulo objetivo por el camino mas corto a razon de maximo 6 grados por tick.
 * Entradas: double anguloActual, double anguloObjetivo
 * Salidas: Nuevo angulo de la nave en el rango [0, 360)
 */
double Motor::CalcularGiroNave(double anguloActual, double anguloObjetivo) {
	double dif = anguloObjetivo - anguloActual;
	// Normalizamos la diferencia al rango [-180, 180] para tomar la ruta de giro mas corta
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

	// Aseguramos que el resultado quede entre 0 y 360 grados
	while (nuevoAngulo >= 360.0) nuevoAngulo -= 360.0;
	while (nuevoAngulo < 0.0) nuevoAngulo += 360.0;

	return nuevoAngulo;
}

/*
 * Funcion: IniciarTurno
 * Descripcion: Prepara el motor para el turno de un jugador: resetea balas y enemigos remanentes,
 *              asigna la copia de la oleada y pone la nave en 270 grados.
 * Entradas: Jugador* j, int oleada, const ColaSpawn& colaMaestra, BitacoraEventos* bitacoraPartida
 * Salidas: Motor listo para simular los ticks de la oleada
 */
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

	jugador->anguloNave = 270.0; // Inicia siempre apuntando hacia abajo (270 grados)
	jugador->combo = 0;           // El combo se reinicia al principio de cada turno

	// Vaciamos listas para asegurar que no queden balas ni enemigos del turno previo
	listaEnemigos.Vaciar();
	listaBalas.Vaciar();

	colaSpawn = colaMaestra; // Copia independiente de la oleada para este jugador
}

/*
 * Funcion: ActualizarTick
 * Descripcion: Ejecuta los 10 pasos obligatorios del ciclo de tick definidos en la seccion 3.7 del PDF:
 *              1. Leer acciones (color, pulso).
 *              2. Rotar la nave maximo 6 grados.
 *              3. Desencolar de ColaSpawn e insertar al final de ListaEnemigos.
 *              4. Disparo automatico cada 4 ticks insertando al inicio de ListaBalas.
 *              5. Mover enemigos y camaleones.
 *              6. Colisiones enemigo-nucleo y enemigo-nave.
 *              7. Mover balas y colisiones bala-enemigo con puntero a puntero.
 *              8. Expansion del pulso orbital.
 *              9. Verificar eliminacion por vida del nucleo.
 *              10. Verificar fin de oleada (cola y lista vacias).
 * Entradas: const AccionTick& accion (comandos del teclado, bot o script para este tick)
 * Salidas: EstadoMotor (ESTADO_EN_JUEGO, ESTADO_FIN_OLEADA o ESTADO_JUGADOR_ELIMINADO)
 */
EstadoMotor Motor::ActualizarTick(const AccionTick& accion) {
	// Paso 1: Leer acciones - actualizar color de fase
	jugador->faseColor = accion.colorObjetivo;

	// Paso 1: Leer acciones - activar onda de pulso si tiene carga 100
	if (accion.activarPulso && jugador->cargaPulso >= 100 && !pulsoActivo) {
		pulsoActivo = true;
		rDeOndaPulso = 20.0;
		jugador->cargaPulso = 0; // Vuelve a 0 al dispararse
		
		if (bitacora != NULL) {
			bitacora->AgregarFinal({tickActual, NumeroOleada, jugador->alias, PULSO, DRON, jugador->faseColor, 0});
		}
	}

	// Paso 2: Actualizar el angulo de la nave hacia el objetivo
	jugador->anguloNave = CalcularGiroNave(jugador->anguloNave, accion.anguloObjetivo);

	// Paso 3: Desencolar de ColaSpawn e insertar al final de ListaEnemigos segun el tick
	while (!colaSpawn.Vacia() && colaSpawn.VerFrente()->tick <= tickActual) {
		Spawn sp;
		colaSpawn.Desencolar(sp);

		// Creamos el enemigo e insertamos al final de la lista doblemente enlazada
		Enemigo nuevoE = CrearEnemigo(sp, NumeroOleada, siguienteIdEnemigo++);
		NodoEnemigo* nuevoNodo = new NodoEnemigo;
		nuevoNodo->enemigo = nuevoE;
		nuevoNodo->anterior = NULL;
		nuevoNodo->siguiente = NULL;
		listaEnemigos.InsertarFinal(nuevoNodo);
	}

	// Paso 4: Disparo automatico de la nave cada 4 ticks
	if (tickActual % 4 == 0) {
		disparosTurno++;
		double rad = jugador->anguloNave * (PI / 180.0);

		Bala b;
		// Se crea en la orbita de la nave (r = 100)
		b.cord_x = 100.0 * cos(rad);
		b.cord_y = 100.0 * sin(rad);

		// Velocidad hacia afuera: 9 unidades por tick
		b.speed_x = 9.0 * cos(rad);
		b.speed_y = 9.0 * sin(rad);
	
		b.color = jugador->faseColor;
		b.duenio = 1;

		NodoBala* nuevoNodo = new NodoBala;
		nuevoNodo->bala = b;
		nuevoNodo->siguiente = NULL;
		listaBalas.AgregarInicio(nuevoNodo);
	}

	// Paso 5: Mover enemigos en coordenadas polares y actualizar camaleones
	NodoEnemigo* nodoE = listaEnemigos.Primero();
	while (nodoE != NULL) {
		MoverEnemigo(nodoE->enemigo, NumeroOleada);
		nodoE = nodoE->siguiente;
	}

	// Paso 6: Resolver colisiones con el nucleo y con la nave
	double naveX, naveY;
	PolarACartesiano(100.0, jugador->anguloNave, naveX, naveY);

	nodoE = listaEnemigos.Primero();
	while (nodoE != NULL) {
		Enemigo& e = nodoE->enemigo;
		double ex, ey;
		PolarACartesiano(e.r, e.theta, ex, ey);

		// Colision contra el nucleo: r <= 20 + 0.4*radio
		if (e.r <= (20.0 + 0.4 * e.radio)) {
			int dano = (e.tipo == TANQUE) ? 22 : 10;
			jugador->vidaNucleo -= dano;
			jugador->combo = 0; // Recibir dano corta la racha de combo

			if (bitacora != NULL) {
				bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, DANO_NUCLEO, e.tipo, e.color, dano });
			}

			nodoE = listaEnemigos.Eliminar(nodoE);
		}
		// Colision contra la nave
		else if (DistanciaMasCorta(ex, ey, naveX, naveY) <= (e.radio + 5.0)) {
			int dano = 12;
			jugador->vidaNucleo -= dano;
			jugador->combo = 0;
			
			if (bitacora != NULL) {
				bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, CHOQUE_NAVE, e.tipo, e.color, dano });
			}
			
			nodoE = listaEnemigos.Eliminar(nodoE);
		}
		else {
			nodoE = nodoE->siguiente;
		}
	}

	// Paso 7: Mover balas y resolver impactos contra enemigos usando puntero a puntero
	NodoBala** ptrBala = listaBalas.ObtenerPtrCabeza();

	while (*ptrBala != NULL) {
		Bala& b = (*ptrBala)->bala;

		// Desplazamos la bala
		b.cord_x += b.speed_x;
		b.cord_y += b.speed_y;

		double distCentroSq = b.cord_x * b.cord_x + b.cord_y * b.cord_y;

		// Si sale del mapa (r > 320), la eliminamos con el puntero doble
		if (distCentroSq > (320.0 * 320.0)) {
			NodoBala* victima = *ptrBala;
			*ptrBala = victima->siguiente;
			delete victima;
			continue;
		}

		// Verificamos si impacta a algun enemigo
		bool balaDestruida = false;
		NodoEnemigo* auxE = listaEnemigos.Primero();

		while (auxE != NULL && !balaDestruida) {
			Enemigo& e = auxE->enemigo;
			double ex, ey;
			PolarACartesiano(e.r, e.theta, ex, ey);

			double distBalaEnemigo = DistanciaMasCorta(b.cord_x, b.cord_y, ex, ey);

			// Comprobamos choque dentro del radio de colision
			if (distBalaEnemigo <= (e.radio + 3.5)) {
				balaDestruida = true;
				Color colorBala = b.color;
				impactosColor[static_cast<int>(colorBala)]++;

				// La bala se destruye de inmediato tras el impacto
				NodoBala* victima = *ptrBala;
				*ptrBala = victima->siguiente;
				delete victima;

				// Caso de impacto con color correcto
				if (colorBala == e.color) {
					correctosTurno++;
					correctosColor[static_cast<int>(colorBala)]++;
					e.vida -= 1;

					if (jugador->combo + 1 > comboMaxTurno) {
						comboMaxTurno = jugador->combo + 1;
					}

					// Si la vida llega a cero, el enemigo muere
					if (e.vida <= 0) {
						int mult = CalcularMultiplicador(jugador->combo);
						int puntos = ObtenerPuntosEnemigo(e.tipo) * mult;

						jugador->puntaje += puntos;
						jugador->combo += 1;

						// Incremento de carga de pulso
						int carga = (e.tipo == TANQUE) ? 14 : 5;
						jugador->cargaPulso = min(100, jugador->cargaPulso + carga);

						if (bitacora != NULL) {
							bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, BAJA, e.tipo, e.color, puntos });
						}

						// Si era un Divisor, insertamos sus 2 Minis antes de borrar el nodo
						if (e.tipo == DIVISOR) {
							GenerarMinis(auxE, listaEnemigos, NumeroOleada, siguienteIdEnemigo);
						}

						listaEnemigos.Eliminar(auxE);
					}
				}
				// Caso de impacto con color incorrecto (castigo)
				else {
					erroneosTurno++;
					e.r += 9.0;         // El enemigo retrocede 9 unidades hacia afuera
					jugador->combo = 0;  // Se reinicia el combo a cero
					if (bitacora != NULL) {
						bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, IMPACTO_ERRONEO, e.tipo, e.color, 0 });
					}
				}
			}

			if (!balaDestruida) {
				auxE = auxE->siguiente;
			}
		}

		// Si la bala no impacto, avanzamos el puntero doble al siguiente nodo
		if (!balaDestruida) {
			ptrBala = &((*ptrBala)->siguiente);
		}
	}

	// Paso 8: Avanzar la onda de pulso y destruir enemigos en su radio
	if (pulsoActivo) {
		rDeOndaPulso += 12.0;
		
		if (rDeOndaPulso > 320.0) {
			pulsoActivo = false;
		}
		else {
			NodoEnemigo* auxE = listaEnemigos.Primero();
			
			while (auxE != NULL) {
				Enemigo& e = auxE->enemigo;

				// Destruye a cualquier enemigo dentro de la banda |r - r_onda| < radio + 3
				if (abs(e.r - rDeOndaPulso) < (e.radio + 3.0)) {
					// Puntos con multiplicador, pero sin alterar combo ni carga de pulso
					int mult = CalcularMultiplicador(jugador->combo);
					int puntos = ObtenerPuntosEnemigo(e.tipo) * mult;
					jugador->puntaje += puntos;

					if (bitacora != NULL) {
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

	// Paso 9: Verificar si el jugador fue eliminado
	if (jugador->vidaNucleo <= 0) {
		jugador->vidaNucleo = 0;

		if (bitacora != NULL) {
			bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, ELIMINACION, DRON, jugador->faseColor, 0});
		}

		return ESTADO_JUGADOR_ELIMINADO;
	}

	// Paso 10: Verificar si se completo la oleada
	if (colaSpawn.Vacia() && listaEnemigos.Vacia()) {
		// Bonificacion de +8 al nucleo por completar la oleada (maximo 100)
		jugador->vidaNucleo = min(100, jugador->vidaNucleo + 8);

		if (bitacora != NULL) {
			bitacora->AgregarFinal({ tickActual, NumeroOleada, jugador->alias, FIN_OLEADA, DRON, jugador->faseColor, 0 });
		}

		return ESTADO_FIN_OLEADA;
	}

	// Si no hubo fin de oleada ni muerte, avanzamos el contador de ticks
	tickActual++;
	return ESTADO_EN_JUEGO;
}
