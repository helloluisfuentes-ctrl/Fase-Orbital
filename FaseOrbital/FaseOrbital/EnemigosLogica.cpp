#include "EnemigosLogica.h"

// calcula la velocidad en la que el enemigo avanza hacia el nucleo, segun el tipo y la oleada
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

// retorna la cantidad de puntos que da matar un enemigo segun el tipo
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

// inicializa un enemigo que sale de ColaSpawn con sus atributos 
Enemigo CrearEnemigo(const Spawn& spawn, int NumeroOleada, int id) {
	Enemigo e;
	e.id = id;
	e.tipo = spawn.tipo;
	e.color = spawn.color;
	e.r = 300.0; // radio de aparicion = 300
	e.theta = static_cast<double>(spawn.angulo);
	e.ticksColor = 0;
	e.omega = 0.0;

	switch (spawn.tipo) {
		case DRON:
			e.vida = 1;
			e.radio = 6;
			break;

		case GIRADOR:
			e.vida = 1;
			e.radio = 6;
			{
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
		
		// los minis aparecen cuando muere un divisor, no se crean desde ColaSpawn
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

// actualiza la posicion de un enemigo por cada tick ademas de las "cinematicas" de los giradores y los camaleones
void MoverEnemigo(Enemigo& e, int NumeroOleada) {
	// avance hacia el nucleo
	double velocidadRadial = ObtenerVelocidadRadial(e.tipo, NumeroOleada);
	e.r -= velocidadRadial;
	
	// gira en coordenadas polares, solo los giradores tienen omega distinto de 0
	e.theta += e.omega;

	// asegura que theta quede en rango de [0, 360]
	while (e.theta >= 360.0) e.theta -= 360.0;
	while (e.theta < 0.0) e.theta += 360.0;

	if (e.tipo == CAMALEON) {
		e.ticksColor++;
		if (e.ticksColor >= 42) {
			e.ticksColor = 0;
			// cambia el color del camaleon en secuencia CIAN -> MAGENTA -> AMBAR -> CIAN
			switch (e.color) {
				case CIAN: e.color = MAGENTA; break;
				case MAGENTA: e.color = AMBAR; break;
				case AMBAR: e.color = CIAN; break;
			}
		}
	}
}

// obtiene los 2 colores que sean diferentes al color del padre para los minis
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

// genera los 2 minis que salen cuando muere un divisor y los agrega a la lista de enemigos
void GenerarMinis(NodoEnemigo* nodoDivisor, ListaEnemigos& listaEnemigos, int NumeroOleada, int& SiguienteID) {
	if (nodoDivisor == NULL) return;

	Enemigo& divisor = nodoDivisor->enemigo;
	Color color1, color2;
	ObtenerColoresDistintos(divisor.color, color1, color2);
	
	// crea el primer mini
	Enemigo mini1;
	mini1.id = SiguienteID++;
	mini1.tipo = MINI;
	mini1.color = color1;
	mini1.r = divisor.r;
	mini1.vida = 1;
	mini1.radio = 4;

	mini1.theta = divisor.theta - 4.0; // se separan 4 grados
	while (mini1.theta < 0.0) mini1.theta += 360.0;

	mini1.ticksColor = 0;
	mini1.omega = -1.7;
	
	// crea el segundo mini
	Enemigo mini2;
	mini2.id = SiguienteID++;
	mini2.tipo = MINI;
	mini2.color = color2;
	mini2.r = divisor.r;
	mini2.vida = 1;
	mini2.radio = 4;

	mini2.theta = divisor.theta + 4.0; // se separan 4 grados
	while (mini2.theta >= 360.0) mini2.theta -= 360.0;

	mini2.ticksColor = 0;
	mini2.omega = +1.7;

	// se crean los nodos
	NodoEnemigo* nodoMini1 = new(NodoEnemigo);
	nodoMini1->enemigo = mini1;
	nodoMini1->anterior = NULL;
	nodoMini1->siguiente = NULL;

	NodoEnemigo* nodoMini2 = new(NodoEnemigo);
	nodoMini2->enemigo = mini2;
	nodoMini2->anterior = NULL;
	nodoMini2->siguiente = NULL;

	// se agregan a la lista de enemigos
	listaEnemigos.InsertarDespues(nodoDivisor, nodoMini1);
	listaEnemigos.InsertarDespues(nodoDivisor, nodoMini2);

}
