#include "EnemigosLogica.h"

/*
 * Funcion: ObtenerVelocidadRadial
 * Descripcion: Calcula la velocidad radial v = 0.8 + 0.08*n hacia el nucleo y aplica el factor
 *              segun el tipo (Dron: 1.0v, Girador: 0.85v, Divisor: 0.80v, Mini: 1.25v, Tanque: 0.50v, Camaleon: 0.85v).
 * Entradas: TipoEnemigo tipo, int NumeroOleada
 * Salidas: Valor decimal de la velocidad radial en unidades/tick
 */
double ObtenerVelocidadRadial(TipoEnemigo tipo, int NumeroOleada) {
	double v = 0.8 + 0.08 * static_cast<double>(NumeroOleada);

	switch (tipo) {
		case DRON: return 1.00 * v;
		case GIRADOR: return 0.85 * v;
		case DIVISOR: return 0.8 * v;
		case CAMALEON: return 0.85 * v;
		case MINI: return 1.25 * v;
		case TANQUE: return 0.5 * v;
		default: return v;
	}
}

/*
 * Funcion: ObtenerPuntosEnemigo
 * Descripcion: Devuelve los puntos base otorgados por la destruccion de cada enemigo (Dron: 100,
 *              Girador: 150, Divisor: 200, Mini: 75, Tanque: 400, Camaleon: 300).
 * Entradas: TipoEnemigo enemigo
 * Salidas: Puntos base enteros
 */
int ObtenerPuntosEnemigo(TipoEnemigo enemigo) {
	switch (enemigo) {
		case DRON: return 100;
		case GIRADOR: return 150;
		case DIVISOR: return 200;
		case CAMALEON: return 300;
		case MINI: return 75;
		case TANQUE: return 400;
		default: return 0;
	}
}

/*
 * Funcion: CrearEnemigo
 * Descripcion: Construye una estructura Enemigo a partir del nodo de la ColaSpawn, fijando su radio r = 300,
 *              su angulo theta, vida, radio de colision y velocidad angular omega segun la oleada.
 * Entradas: const Spawn& spawn, int NumeroOleada, int id
 * Salidas: Estructura Enemigo inicializada
 */
Enemigo CrearEnemigo(const Spawn& spawn, int NumeroOleada, int id) {
	Enemigo e;
	e.id = id;
	e.tipo = spawn.tipo;
	e.color = spawn.color;
	e.r = 300.0; // Radio inicial de aparicion obligatorio = 300
	e.theta = static_cast<double>(spawn.angulo);
	e.ticksColor = 0;
	e.omega = 0.0;

	// Configuracion particular segun la tabla de enemigos (seccion 3.3)
	switch (spawn.tipo) {
		case DRON:
			e.vida = 1;
			e.radio = 6;
			break;

		case GIRADOR:
			e.vida = 1;
			e.radio = 6;
			{
				// Velocidad angular: omega = +-(0.9 + 0.05*n) grados/tick
				double magnitud = 0.9 + 0.05 * static_cast<double>(NumeroOleada);
				e.omega = (spawn.sentido == 0) ? -magnitud : magnitud;
			}
			break;

		case DIVISOR:
			e.vida = 1;
			e.radio = 8;
			break;

		case CAMALEON:
			e.vida = 1;
			e.radio = 7;
			break;
		
		// Los Minis se generan al morir un Divisor, no se crean directamente de ColaSpawn
		case MINI:
			e.vida = 1;
			e.radio = 4;
			break;

		case TANQUE:
			e.vida = 3;
			e.radio = 10;
			break;
	}

	return e;
}

/*
 * Funcion: MoverEnemigo
 * Descripcion: Actualiza la cinematica polar del enemigo en cada tick: resta la velocidad radial a r,
 *              suma la velocidad angular omega a theta y ejecuta el ciclo de color del Camaleon cada 42 ticks.
 * Entradas: Enemigo& e (referencia al enemigo a mover), int NumeroOleada
 * Salidas: Modifica las coordenadas polares y atributos del enemigo
 */
void MoverEnemigo(Enemigo& e, int NumeroOleada) {
	// 1. Avance radial hacia el nucleo
	double velocidadRadial = ObtenerVelocidadRadial(e.tipo, NumeroOleada);
	e.r -= velocidadRadial;
	
	// 2. Giro en coordenadas polares (solo Girador y Mini tienen omega distinto de 0)
	e.theta += e.omega;

	// Normalizamos el angulo en el rango [0, 360)
	while (e.theta >= 360.0) e.theta -= 360.0;
	while (e.theta < 0.0) e.theta += 360.0;

	// 3. Comportamiento especial del Camaleon: ciclo C -> M -> A -> C cada 42 ticks
	if (e.tipo == CAMALEON) {
		e.ticksColor++;
		if (e.ticksColor >= 42) {
			e.ticksColor = 0;
			switch (e.color) {
				case CIAN: e.color = MAGENTA; break;
				case MAGENTA: e.color = AMBAR; break;
				case AMBAR: e.color = CIAN; break;
			}
		}
	}
}

/*
 * Funcion: ObtenerColoresDistintos
 * Descripcion: Determina los dos colores complementarios distintos al color del Divisor padre.
 * Entradas: Color colorPadre, Color& color1, Color& color2 (referencias de salida)
 * Salidas: Modifica color1 y color2 con los dos colores restantes
 */
void ObtenerColoresDistintos(Color colorPadre, Color& color1, Color& color2) {
	switch (colorPadre) {
		case CIAN:
			color1 = MAGENTA;
			color2 = AMBAR;
			break;
		case MAGENTA:
			color1 = CIAN;
			color2 = AMBAR;
			break;
		case AMBAR:
			color1 = CIAN;
			color2 = MAGENTA;
			break;
	}
}

/*
 * Funcion: GenerarMinis
 * Descripcion: Crea dos enemigos Mini con angulos theta +- 4 grados, omega +- 1.7 grados/tick y colores
 *              distintos al padre, insertandolos con InsertarDespues sobre el nodo Divisor antes de borrarlo.
 * Entradas: NodoEnemigo* nodoDivisor, ListaEnemigos& listaEnemigos, int NumeroOleada, int& SiguienteID
 * Salidas: Inserta 2 nuevos nodos en la lista doblemente enlazada
 */
void GenerarMinis(NodoEnemigo* nodoDivisor, ListaEnemigos& listaEnemigos, int NumeroOleada, int& SiguienteID) {
	if (nodoDivisor == NULL) return;

	Enemigo& divisor = nodoDivisor->enemigo;
	Color color1, color2;
	ObtenerColoresDistintos(divisor.color, color1, color2);
	
	// Configuramos el primer Mini: theta - 4 grados, omega -1.7
	Enemigo mini1;
	mini1.id = SiguienteID++;
	mini1.tipo = MINI;
	mini1.color = color1;
	mini1.r = divisor.r;
	mini1.vida = 1;
	mini1.radio = 4;

	mini1.theta = divisor.theta - 4.0;
	while (mini1.theta < 0.0) mini1.theta += 360.0;

	mini1.ticksColor = 0;
	mini1.omega = -1.7;
	
	// Configuramos el segundo Mini: theta + 4 grados, omega +1.7
	Enemigo mini2;
	mini2.id = SiguienteID++;
	mini2.tipo = MINI;
	mini2.color = color2;
	mini2.r = divisor.r;
	mini2.vida = 1;
	mini2.radio = 4;

	mini2.theta = divisor.theta + 4.0;
	while (mini2.theta >= 360.0) mini2.theta -= 360.0;

	mini2.ticksColor = 0;
	mini2.omega = +1.7;

	// Reservamos los nodos dinamicos
	NodoEnemigo* nodoMini1 = new NodoEnemigo;
	nodoMini1->enemigo = mini1;
	nodoMini1->anterior = NULL;
	nodoMini1->siguiente = NULL;

	NodoEnemigo* nodoMini2 = new NodoEnemigo;
	nodoMini2->enemigo = mini2;
	nodoMini2->anterior = NULL;
	nodoMini2->siguiente = NULL;

	// Insercion con InsertarDespues inmediatamente tras el nodo Divisor
	listaEnemigos.InsertarDespues(nodoDivisor, nodoMini1);
	listaEnemigos.InsertarDespues(nodoDivisor, nodoMini2);
}
