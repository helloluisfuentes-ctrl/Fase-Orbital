#include "Pruebas.h"
#include "ColaSpawn.h"
#include "ListaEnemigos.h"
#include "RondaJugadores.h"
#include "ListaBalas.h"
#include "HistorialPartidas.h"
#include "Motor.h"
#include "EnemigosLogica.h"
#include "Oleada.h"
#include "BitacoraEventos.h"
#include "SesionJuego.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>

/*
 * Archivo: Pruebas.cpp
 * Descripcion: Implementacion de la suite de 14 pruebas unitarias minimas
 * exigidas en el enunciado del proyecto. Validamos cada estructura de datos
 * (listas simples con doble puntero, listas dobles, listas circulares, colas FIFO),
 * el algoritmo Merge Sort sobre listas, persistencia y la logica de colisiones y turnos.
 */

/*
 * Funcion: ProbarP01
 * Descripcion: Prueba la lista doblemente enlazada insertando 1000 nodos y eliminando
 * los nodos con ID par durante el recorrido. Verifica que el tamano final sea 500
 * y que los recorridos hacia adelante y hacia atras con punteros 'anterior' coincidan.
 * Entradas: Ninguna
 * Salidas: True si la lista mantiene la consistencia bidireccional, false si falla
 */
bool ProbarP01() {
    ListaEnemigos lista;

    // Insertamos 1000 enemigos con identificadores secuenciales del 1 al 1000
    for (int i = 1; i <= 1000; i++) {
        NodoEnemigo* n = new NodoEnemigo;
        n->enemigo.id = i;
        n->enemigo.tipo = DRON;
        n->enemigo.color = CIAN;
        n->enemigo.r = 300.0;
        n->enemigo.theta = 0.0;
        n->enemigo.omega = 0.0;
        n->enemigo.vida = 1;
        n->enemigo.radio = 6;
        n->enemigo.ticksColor = 0;
        lista.InsertarFinal(n);
    }

    // Recorremos la lista y borramos los pares; Eliminar retorna el siguiente nodo para no perder el puntero
    NodoEnemigo* aux = lista.Primero();
    while (aux != NULL) {
        if (aux->enemigo.id % 2 == 0) {
            aux = lista.Eliminar(aux);
        } else {
            aux = aux->siguiente;
        }
    }

    // Validamos que el contador interno de la lista haya quedado exactamente en 500
    if (lista.Tamano() != 500) {
        return false;
    }

    // Recorrido hacia adelante: verificamos que todos los elementos restantes sean impares
    int conteoAdelante = 0;
    aux = lista.Primero();
    while (aux != NULL) {
        if (aux->enemigo.id % 2 == 0) return false;
        conteoAdelante++;
        aux = aux->siguiente;
    }

    // Recorrido hacia atras: usamos los punteros 'anterior' desde la cola para comprobar enlaces inversos
    int conteoAtras = 0;
    aux = lista.Ultimo();
    while (aux != NULL) {
        conteoAtras++;
        aux = aux->anterior;
    }

    // Ambos recorridos deben contar exactamente los mismos 500 nodos
    return (conteoAdelante == 500 && conteoAtras == 500);
}

/*
 * Funcion: ProbarP02
 * Descripcion: Prueba los casos limite de eliminacion en la lista doblemente enlazada:
 * eliminar un nodo unico, eliminar la cabeza, eliminar la cola y vaciar la lista por completo.
 * Entradas: Ninguna
 * Salidas: True si los punteros cabeza, cola y enlaces anterior/siguiente se actualizan bien
 */
bool ProbarP02() {
    ListaEnemigos lista;

    // Caso 1: Insertar y eliminar en una lista de un solo elemento
    NodoEnemigo* n1 = new NodoEnemigo;
    n1->enemigo.id = 99;
    lista.InsertarFinal(n1);

    if (lista.Tamano() != 1 || lista.Primero() != lista.Ultimo()) return false;

    lista.Eliminar(lista.Primero());

    // La lista debe quedar limpia con cabeza y cola apuntando a NULL
    if (lista.Tamano() != 0 || lista.Primero() != NULL || lista.Ultimo() != NULL || !lista.Vacia()) {
        return false;
    }

    // Caso 2: Insertar tres elementos para probar eliminacion de extremos (cabeza y cola)
    NodoEnemigo* nodoA = new NodoEnemigo; nodoA->enemigo.id = 1;
    NodoEnemigo* nodoB = new NodoEnemigo; nodoB->enemigo.id = 2;
    NodoEnemigo* nodoC = new NodoEnemigo; nodoC->enemigo.id = 3;

    lista.InsertarFinal(nodoA);
    lista.InsertarFinal(nodoB);
    lista.InsertarFinal(nodoC);

    // Eliminamos la cabeza (nodoA); nodoB debe ser la nueva cabeza con anterior en NULL
    lista.Eliminar(nodoA);

    if (lista.Primero() != nodoB) return false;
    if (lista.Primero()->anterior != NULL) return false;
    if (lista.Tamano() != 2) return false;

    // Eliminamos la cola (nodoC); nodoB pasa a ser tambien la cola con siguiente en NULL
    lista.Eliminar(nodoC);

    if (lista.Ultimo() != nodoB) return false;
    if (lista.Ultimo()->siguiente != NULL) return false;
    if (lista.Tamano() != 1) return false;

    // Eliminamos el ultimo nodo restante
    lista.Eliminar(nodoB);

    if (lista.Tamano() != 0 || lista.Primero() != NULL || lista.Ultimo() != NULL) {
        return false;
    }

    return true;
}

/*
 * Funcion: ProbarP03
 * Descripcion: Prueba el metodo InsertarDespues en la lista doble cuando se inserta
 * justamente despues del ultimo nodo (cola), verificando que la cola se actualice.
 * Entradas: Ninguna
 * Salidas: True si la cola y todos los enlaces bidireccionales quedan consistentes
 */
bool ProbarP03() {
    ListaEnemigos lista;

    // Insertamos 2 nodos iniciales
    NodoEnemigo* n1 = new NodoEnemigo; n1->enemigo.id = 1;
    NodoEnemigo* n2 = new NodoEnemigo; n2->enemigo.id = 2;
    lista.InsertarFinal(n1);
    lista.InsertarFinal(n2);

    if (lista.Ultimo() != n2) return false;

    // Insertamos n3 justo despues de n2 (que es la cola actual)
    NodoEnemigo* n3 = new NodoEnemigo; n3->enemigo.id = 3;
    lista.InsertarDespues(n2, n3);

    // La cola de la estructura debe actualizarse a n3
    if (lista.Ultimo() != n3) {
        return false;
    }

    if (lista.Tamano() != 3) {
        return false;
    }

    // Validamos los enlaces del nuevo nodo en el extremo
    if (n3->siguiente != NULL) return false;
    if (n3->anterior != n2) return false;
    if (n2->siguiente != n3) return false;

    // Recorrido inverso para verificar que los punteros hacia atras esten conectados
    NodoEnemigo* aux = lista.Ultimo();
    if (aux->enemigo.id != 3) return false;
    aux = aux->anterior;
    if (aux->enemigo.id != 2) return false;
    aux = aux->anterior;
    if (aux->enemigo.id != 1) return false;
    if (aux->anterior != NULL) return false;

    return true;
}

/*
 * Funcion: EsBalaMarcada
 * Descripcion: Predicado auxiliar para probar el filtrado de balas; marca como eliminables las de duenio 0.
 * Entradas: b (instancia de Bala)
 * Salidas: True si el duenio es 0, false de lo contrario
 */
bool EsBalaMarcada(Bala b) {
    return b.duenio == 0;
}

/*
 * Funcion: ProbarP04
 * Descripcion: Prueba la eliminacion condicional en la lista simple de balas usando
 * puntero doble (EliminarSi). Valida la eliminacion de nodos consecutivos en cabeza y en medio.
 * Entradas: Ninguna
 * Salidas: True si los elementos marcados se eliminan sin romper la cadena
 */
bool ProbarP04() {
    ListaBalas lista;

    // Insertamos al inicio en orden inverso para obtener la secuencia deseada:
    // b1 (borrar), b2 (borrar), b3 (valida 1), b4 (borrar), b5 (borrar), b6 (valida 2)

    NodoBala* b6 = new NodoBala; b6->bala.duenio = 2; b6->siguiente = NULL;
    lista.AgregarInicio(b6);

    NodoBala* b5 = new NodoBala; b5->bala.duenio = 0; b5->siguiente = NULL;
    lista.AgregarInicio(b5);

    NodoBala* b4 = new NodoBala; b4->bala.duenio = 0; b4->siguiente = NULL;
    lista.AgregarInicio(b4);

    NodoBala* b3 = new NodoBala; b3->bala.duenio = 1; b3->siguiente = NULL;
    lista.AgregarInicio(b3);

    NodoBala* b2 = new NodoBala; b2->bala.duenio = 0; b2->siguiente = NULL;
    lista.AgregarInicio(b2);

    NodoBala* b1 = new NodoBala; b1->bala.duenio = 0; b1->siguiente = NULL;
    lista.AgregarInicio(b1);

    if (lista.Tamano() != 6) return false;

    // Ejecutamos la eliminacion con puntero doble
    lista.EliminarSi(EsBalaMarcada);

    // Deben quedar unicamente las 2 balas validas
    if (lista.Tamano() != 2) {
        return false;
    }

    // Comprobamos que la cabeza sea la bala valida 1
    NodoBala* aux = lista.ObtenerCabeza();
    if (aux == NULL || aux->bala.duenio != 1) {
        return false;
    }

    // Comprobamos que el segundo elemento sea la bala valida 2
    aux = aux->siguiente;
    if (aux == NULL || aux->bala.duenio != 2) {
        return false;
    }

    // El enlace del ultimo debe apuntar a NULL
    if (aux->siguiente != NULL) {
        return false;
    }

    return true;
}

/*
 * Funcion: ProbarP05
 * Descripcion: Prueba el comportamiento FIFO de la cola de aparicion (ColaSpawn).
 * Encola 3 elementos, valida el orden de salida por tick y prueba intentos de desencolar en vacio.
 * Entradas: Ninguna
 * Salidas: True si el orden FIFO y los valores coinciden exactamente
 */
bool ProbarP05() {
    ColaSpawn cola;
    Spawn temp;

    // Desencolar de una cola vacia debe retornar false sin caerse
    if (cola.Desencolar(temp) != false) return false;
    if (cola.VerFrente() != NULL) return false;
    if (!cola.Vacia() || cola.Tamano() != 0) return false;

    // Encolamos 3 eventos de aparicion con ticks crecientes
    Spawn s1 = { 60, DRON, CIAN, 0, 0 };
    Spawn s2 = { 85, GIRADOR, MAGENTA, 45, 0 };
    Spawn s3 = { 110, DIVISOR, AMBAR, 90, 0 };

    cola.Encolar(s1);
    cola.Encolar(s2);
    cola.Encolar(s3);

    if (cola.Tamano() != 3) return false;

    // Desencolamos verificando el orden de llegada (FIFO)
    if (!cola.Desencolar(temp) || temp.tick != 60) return false;
    if (cola.Tamano() != 2) return false;

    if (!cola.Desencolar(temp) || temp.tick != 85) return false;
    if (cola.Tamano() != 1) return false;

    if (!cola.Desencolar(temp) || temp.tick != 110) return false;

    // Comprobamos que la cola quedo completamente vacia
    if (cola.Tamano() != 0) return false;
    if (!cola.Vacia()) return false;
    if (cola.VerFrente() != NULL) return false;

    if (cola.Desencolar(temp) != false) return false;

    return true;
}

/*
 * Funcion: ProbarP06
 * Descripcion: Prueba que el constructor de copia de ColaSpawn realice una copia profunda
 * real de cada nodo. Destruir o vaciar la copia no debe alterar ni corromper la cola original.
 * Entradas: Ninguna
 * Salidas: True si la cola original sobrevive intacta tras la destruccion de la copia
 */
bool ProbarP06() {
    ColaSpawn original;

    // Cargamos 3 enemigos en la cola original
    Spawn s1 = { 10, DRON, CIAN, 0, 0 };
    Spawn s2 = { 20, TANQUE, MAGENTA, 180, 0 };
    Spawn s3 = { 30, DIVISOR, AMBAR, 90, 0 };

    original.Encolar(s1);
    original.Encolar(s2);
    original.Encolar(s3);

    if (original.Tamano() != 3) return false;

    // Abrimos un bloque anidado para que la variable 'copia' se destruya al salir del alcance
    {
        ColaSpawn copia(original);

        if (copia.Tamano() != 3) return false;
        if (copia.VerFrente() == NULL || copia.VerFrente()->tick != 10) return false;

        // Vaciamos la copia entera
        Spawn salida;
        while (!copia.Vacia()) {
            copia.Desencolar(salida);
        }

        if (copia.Tamano() != 0 || !copia.Vacia()) return false;

        // La cola original no debe haber sido tocada
        if (original.Tamano() != 3) return false;
        if (original.VerFrente() == NULL || original.VerFrente()->tick != 10) return false;
    }

    // Fuera del bloque, la copia ya llamo a su destructor; verificamos que la original no sufrio doble liberacion
    if (original.Tamano() != 3) return false;

    Spawn temp;
    if (!original.Desencolar(temp) || temp.tick != 10) return false;
    if (!original.Desencolar(temp) || temp.tick != 20) return false;
    if (!original.Desencolar(temp) || temp.tick != 30) return false;

    if (original.Tamano() != 0 || !original.Vacia()) return false;

    return true;
}

/*
 * Funcion: ProbarP07
 * Descripcion: Prueba la lista circular doblemente enlazada de jugadores (RondaJugadores).
 * Valida rotacion horaria y antihoraria, eliminacion del jugador de turno y desocupado total.
 * Entradas: Ninguna
 * Salidas: True si el orden circular y las bajas funcionan de acuerdo a la especificacion
 */
bool ProbarP07() {
    RondaJugadores ronda;

    // Creamos 3 jugadores dinamicos de prueba
    Jugador* j1 = new Jugador; j1->alias = "Ana";
    Jugador* j2 = new Jugador; j2->alias = "Luis";
    Jugador* j3 = new Jugador; j3->alias = "Carlos";

    ronda.Agregar(j1);
    ronda.Agregar(j2);
    ronda.Agregar(j3);

    if (ronda.Tamano() != 3) return false;

    if (ronda.VerActual() != j1) return false;

    // Probamos rotacion horaria (Ana -> Luis -> Carlos -> Ana)
    ronda.Avanzar(true);
    if (ronda.VerActual() != j2) return false;
    ronda.Avanzar(true);
    if (ronda.VerActual() != j3) return false;
    ronda.Avanzar(true);
    if (ronda.VerActual() != j1) return false;

    // Probamos rotacion antihoraria (Ana -> Carlos -> Luis)
    ronda.Avanzar(false);
    if (ronda.VerActual() != j3) return false; 
    ronda.Avanzar(false);
    if (ronda.VerActual() != j2) return false;

    // Eliminamos a Luis (actual) en sentido horario; el siguiente debe ser Carlos
    Jugador* eliminado1 = ronda.EliminarActual(true);
    if (eliminado1 != j2) return false;
    if (ronda.Tamano() != 2) return false;
    if (ronda.VerActual() != j3) return false;

    // Eliminamos a Carlos; debe quedar solo Ana
    Jugador* eliminado2 = ronda.EliminarActual(true);
    if (eliminado2 != j3) return false;
    if (ronda.Tamano() != 1) return false;
    if (ronda.VerActual() != j1) return false;

    // Con un unico jugador, rotar hacia cualquier sentido debe mantenerlo en si mismo
    ronda.Avanzar(true);
    if (ronda.VerActual() != j1) return false;
    ronda.Avanzar(false);
    if (ronda.VerActual() != j1) return false;

    // Eliminamos al ultimo jugador; la estructura debe quedar vacia
    Jugador* eliminado3 = ronda.EliminarActual(true);
    if (eliminado3 != j1) return false;

    if (ronda.Tamano() != 0 || !ronda.Vacia() || ronda.VerActual() != NULL) {
        return false;
    }

    // Liberamos los registros de jugadores creados en memoria dinamica
    delete j1;
    delete j2;
    delete j3;

    return true;
}

/*
 * Funcion: ProbarP08
 * Descripcion: Prueba el determinismo del motor simulando dos turnos independientes
 * con la misma semilla (42) y las mismas acciones; valida que el estado y los eventos coincidan.
 * Entradas: Ninguna
 * Salidas: True si ambas simulaciones producen resultados identicos tick por tick
 */
bool ProbarP08() {
    uint32_t semilla = 42;
    int oleada = 1;

    // Primera simulacion
    Jugador j1;
    j1.alias = "TestBot";
    j1.esBot = true;
    j1.vidaNucleo = 100;
    j1.puntaje = 0;
    j1.combo = 0;
    j1.cargaPulso = 0;
    j1.faseColor = CIAN;
    j1.anguloNave = 270.0;

    ColaSpawn cola1;
    GenerarOleada(semilla, oleada, cola1);
    BitacoraEventos bitacora1;
    Motor motor1;
    motor1.IniciarTurno(&j1, oleada, cola1, &bitacora1);

    for (int t = 0; t < 150; t++) {
        AccionTick act;
        act.anguloObjetivo = (t % 2 == 0) ? 90.0 : 180.0;
        act.colorObjetivo = (t < 50) ? CIAN : ((t < 100) ? MAGENTA : AMBAR);
        act.activarPulso = false;
        motor1.ActualizarTick(act);
    }

    // Segunda simulacion con datos y comandos identicos
    Jugador j2;
    j2.alias = "TestBot";
    j2.esBot = true;
    j2.vidaNucleo = 100;
    j2.puntaje = 0;
    j2.combo = 0;
    j2.cargaPulso = 0;
    j2.faseColor = CIAN;
    j2.anguloNave = 270.0;

    ColaSpawn cola2;
    GenerarOleada(semilla, oleada, cola2);
    BitacoraEventos bitacora2;
    Motor motor2;
    motor2.IniciarTurno(&j2, oleada, cola2, &bitacora2);

    for (int t = 0; t < 150; t++) {
        AccionTick act;
        act.anguloObjetivo = (t % 2 == 0) ? 90.0 : 180.0;
        act.colorObjetivo = (t < 50) ? CIAN : ((t < 100) ? MAGENTA : AMBAR);
        act.activarPulso = false;
        motor2.ActualizarTick(act);
    }

    // Verificamos que las variables del jugador y las listas del motor sean identicas
    if (j1.vidaNucleo != j2.vidaNucleo) return false;
    if (j1.puntaje != j2.puntaje) return false;
    if (j1.combo != j2.combo) return false;
    if (j1.cargaPulso != j2.cargaPulso) return false;
    if (std::abs(j1.anguloNave - j2.anguloNave) > 0.001) return false;
    if (motor1.GetListaEnemigos().Tamano() != motor2.GetListaEnemigos().Tamano()) return false;
    if (motor1.GetListaBalas().Tamano() != motor2.GetListaBalas().Tamano()) return false;
    if (motor1.GetDisparosTurno() != motor2.GetDisparosTurno()) return false;
    if (motor1.GetCorrectosTurno() != motor2.GetCorrectosTurno()) return false;
    if (motor1.GetErroneosTurno() != motor2.GetErroneosTurno()) return false;

    // Verificamos que cada evento registrado en la bitacora LIFO sea exactamente igual
    if (bitacora1.Tamano() != bitacora2.Tamano()) return false;
    NodoEvento* e1 = bitacora1.ObtenerCabeza();
    NodoEvento* e2 = bitacora2.ObtenerCabeza();
    while (e1 != NULL && e2 != NULL) {
        if (e1->evento.tick != e2->evento.tick) return false;
        if (e1->evento.tipoEvento != e2->evento.tipoEvento) return false;
        if (e1->evento.tipoEnemigo != e2->evento.tipoEnemigo) return false;
        if (e1->evento.color != e2->evento.color) return false;
        if (e1->evento.valor != e2->evento.valor) return false;
        e1 = e1->siguiente;
        e2 = e2->siguiente;
    }

    return true;
}

/*
 * Funcion: ProbarP09
 * Descripcion: Prueba la penalizacion de afinidad de color errónea: disparar bala de
 * color Cian contra Tanque Magenta debe aumentar su radio (+9 u), resetear combo a 0
 * y no reducir la vida del enemigo.
 * Entradas: Ninguna
 * Salidas: True si se cumplen las penalizaciones y se registra el evento IMPACTO_ERRONEO
 */
bool ProbarP09() {
    Jugador j;
    j.alias = "TestColor";
    j.esBot = false;
    j.vidaNucleo = 100;
    j.puntaje = 0;
    j.combo = 5;
    j.cargaPulso = 0;
    j.faseColor = CIAN;
    j.anguloNave = 270.0;

    ColaSpawn cola;
    Spawn s = { 0, TANQUE, MAGENTA, 270, 0 };
    cola.Encolar(s);

    BitacoraEventos bitacora;
    Motor motor;
    motor.IniciarTurno(&j, 1, cola, &bitacora);
    j.combo = 5;

    // Disparamos con la fase en Cian directo hacia el Tanque Magenta en 270 grados
    for (int t = 0; t < 25; t++) {
        AccionTick act;
        act.anguloObjetivo = 270.0;
        act.colorObjetivo = CIAN;
        act.activarPulso = false;
        motor.ActualizarTick(act);
        if (motor.GetErroneosTurno() > 0) break;
    }

    // Validamos penalizaciones de la regla del juego
    if (motor.GetErroneosTurno() != 1) return false;
    if (j.combo != 0) return false;
    if (motor.GetListaEnemigos().Tamano() != 1) return false;
    NodoEnemigo* nodoTanque = motor.GetListaEnemigos().Primero();
    if (nodoTanque == NULL) return false;
    if (nodoTanque->enemigo.vida != 3) return false;
    if (nodoTanque->enemigo.r < 285.0) return false;

    // Comprobamos que el evento haya quedado registrado en la bitacora
    bool eventoRegistrado = false;
    NodoEvento* ev = bitacora.ObtenerCabeza();
    while (ev != NULL) {
        if (ev->evento.tipoEvento == IMPACTO_ERRONEO && ev->evento.color == MAGENTA) {
            eventoRegistrado = true;
            break;
        }
        ev = ev->siguiente;
    }

    return eventoRegistrado;
}

/*
 * Funcion: ProbarP10
 * Descripcion: Prueba la division de un enemigo Divisor: al ser destruido debe generar
 * 2 enemigos Mini a +-4 grados con colores distintos entre si y distintos al Divisor original,
 * insertandolos ordenadamente en la lista doble.
 * Entradas: Ninguna
 * Salidas: True si los 2 Minis quedan insertados con radios y angulos validos
 */
bool ProbarP10() {
    ListaEnemigos lista;

    NodoEnemigo* nA = new NodoEnemigo;
    nA->enemigo.id = 1;
    nA->enemigo.tipo = DRON;
    nA->enemigo.color = CIAN;
    nA->enemigo.r = 200.0;
    nA->enemigo.theta = 0.0;
    nA->enemigo.vida = 1;
    nA->enemigo.radio = 6;
    nA->anterior = NULL; nA->siguiente = NULL;
    lista.InsertarFinal(nA);

    NodoEnemigo* nDivisor = new NodoEnemigo;
    nDivisor->enemigo.id = 2;
    nDivisor->enemigo.tipo = DIVISOR;
    nDivisor->enemigo.color = CIAN;
    nDivisor->enemigo.r = 180.0;
    nDivisor->enemigo.theta = 120.0;
    nDivisor->enemigo.vida = 1;
    nDivisor->enemigo.radio = 8;
    nDivisor->anterior = NULL; nDivisor->siguiente = NULL;
    lista.InsertarFinal(nDivisor);

    NodoEnemigo* nB = new NodoEnemigo;
    nB->enemigo.id = 3;
    nB->enemigo.tipo = DRON;
    nB->enemigo.color = AMBAR;
    nB->enemigo.r = 250.0;
    nB->enemigo.theta = 180.0;
    nB->enemigo.vida = 1;
    nB->enemigo.radio = 6;
    nB->anterior = NULL; nB->siguiente = NULL;
    lista.InsertarFinal(nB);

    if (lista.Tamano() != 3) return false;

    // Generamos los 2 Minis y removemos el Divisor
    int siguienteId = 10;
    GenerarMinis(nDivisor, lista, 1, siguienteId);
    lista.Eliminar(nDivisor);

    // La lista ahora debe tener 4 elementos (nA, mini1, mini2, nB)
    if (lista.Tamano() != 4) return false;

    NodoEnemigo* primero = lista.Primero();
    if (primero != nA) return false;

    NodoEnemigo* m1 = primero->siguiente;
    if (m1 == NULL || m1->enemigo.tipo != MINI) return false;

    NodoEnemigo* m2 = m1->siguiente;
    if (m2 == NULL || m2->enemigo.tipo != MINI) return false;

    if (m2->siguiente != nB) return false;
    if (lista.Ultimo() != nB) return false;

    // Verificamos que conserven el mismo radio que el divisor original
    if (std::abs(m1->enemigo.r - 180.0) > 0.001) return false;
    if (std::abs(m2->enemigo.r - 180.0) > 0.001) return false;

    // Verificamos separacion angular de 4 grados (120 +- 4 = 116 y 124)
    double t1 = m1->enemigo.theta;
    double t2 = m2->enemigo.theta;
    bool angulosCorrectos = (std::abs(t1 - 116.0) < 0.001 && std::abs(t2 - 124.0) < 0.001) ||
                            (std::abs(t1 - 124.0) < 0.001 && std::abs(t2 - 116.0) < 0.001);
    if (!angulosCorrectos) return false;

    // Verificamos que los colores de los Minis sean diferentes al Cian del Divisor y diferentes entre si
    Color c1 = m1->enemigo.color;
    Color c2 = m2->enemigo.color;
    if (c1 == CIAN || c2 == CIAN) return false;
    if (c1 == c2) return false;
    if (!((c1 == MAGENTA && c2 == AMBAR) || (c1 == AMBAR && c2 == MAGENTA))) return false;

    // Verificamos enlaces hacia atras
    if (nB->anterior != m2 || m2->anterior != m1 || m1->anterior != nA) return false;

    return true;
}

/*
 * Funcion: ProbarP11
 * Descripcion: Prueba el funcionamiento del pulso electromagnetico (EMP).
 * Con carga < 100 no se activa; con carga = 100 se activa la onda expansiva,
 * destruye enemigos dentro de la banda de choque y resetea la carga a 0.
 * Entradas: Ninguna
 * Salidas: True si la onda se propaga y destruye enemigos registrando el evento
 */
bool ProbarP11() {
    // Parte 1: Carga menor a 100 no debe disparar el pulso
    {
        Jugador j;
        j.alias = "TestPulso1";
        j.esBot = false;
        j.vidaNucleo = 100;
        j.puntaje = 0;
        j.combo = 0;
        j.cargaPulso = 80;
        j.faseColor = CIAN;
        j.anguloNave = 270.0;

        ColaSpawn cola;
        Spawn s = { 0, DRON, CIAN, 90, 0 };
        cola.Encolar(s);

        BitacoraEventos bitacora;
        Motor motor;
        motor.IniciarTurno(&j, 1, cola, &bitacora);
        j.cargaPulso = 80;

        AccionTick act;
        act.anguloObjetivo = 270.0;
        act.colorObjetivo = CIAN;
        act.activarPulso = true;

        motor.ActualizarTick(act);

        if (motor.EsPulsoActivo()) return false;
        if (j.cargaPulso != 80) return false;
        if (motor.GetListaEnemigos().Tamano() != 1) return false;
    }

    // Parte 2: Carga al 100% activa la onda y elimina enemigos
    {
        Jugador j;
        j.alias = "TestPulso2";
        j.esBot = false;
        j.vidaNucleo = 100;
        j.puntaje = 0;
        j.combo = 0;
        j.cargaPulso = 100;
        j.faseColor = CIAN;
        j.anguloNave = 270.0;

        ColaSpawn cola;
        Spawn s = { 0, DRON, AMBAR, 90, 0 };
        cola.Encolar(s);

        BitacoraEventos bitacora;
        Motor motor;
        motor.IniciarTurno(&j, 1, cola, &bitacora);
        j.cargaPulso = 100;

        AccionTick act;
        act.anguloObjetivo = 270.0;
        act.colorObjetivo = CIAN;
        act.activarPulso = true;
        motor.ActualizarTick(act);

        if (!motor.EsPulsoActivo()) return false;
        if (j.cargaPulso != 0) return false;

        // Avanzamos los ticks necesarios para que la onda de radio alcance al enemigo
        for (int t = 1; t < 30; t++) {
            act.activarPulso = false;
            motor.ActualizarTick(act);
            if (motor.GetListaEnemigos().Vacia()) break;
        }

        if (!motor.GetListaEnemigos().Vacia()) return false;
        if (j.puntaje != 100) return false;

        bool huboPulso = false, huboBaja = false;
        NodoEvento* ev = bitacora.ObtenerCabeza();
        while (ev != NULL) {
            if (ev->evento.tipoEvento == PULSO) huboPulso = true;
            if (ev->evento.tipoEvento == BAJA) huboBaja = true;
            ev = ev->siguiente;
        }
        if (!huboPulso || !huboBaja) return false;
    }

    return true;
}

/*
 * Funcion: ListaEstaOrdenada
 * Descripcion: Helper que recorre una lista simple de puntajes para verificar si esta en orden ascendente.
 * Entradas: cabeza (puntero al primer nodo de la lista)
 * Salidas: True si la lista esta ordenada de menor a mayor, false en caso contrario
 */
bool ListaEstaOrdenada(NodoPuntaje* cabeza) {
    if (cabeza == NULL) return true;
    NodoPuntaje* actual = cabeza;
    while (actual->siguiente != NULL) {
        if (actual->valor > actual->siguiente->valor) {
            return false;
        }
        actual = actual->siguiente;
    }
    return true;
}

/*
 * Funcion: ProbarP12
 * Descripcion: Prueba el algoritmo Merge Sort y el calculo de la mediana en ListaPuntajes.
 * Cubre casos de lista vacia, 1 elemento, elementos desordenados, ordenados, orden inverso y longitud par con repetidos.
 * Entradas: Ninguna
 * Salidas: True si todos los casos de ordenamiento y calculo estadistico son correctos
 */
bool ProbarP12() {
    // Caso 1: Lista vacia
    {
        ListaPuntajes l;
        l.MergeSort();
        if (l.Tamano() != 0 || l.ObtenerMediana() != 0.0) return false;
    }

    // Caso 2: Lista con 1 solo elemento
    {
        ListaPuntajes l;
        l.InsertarFinal(42);
        l.MergeSort();
        if (!ListaEstaOrdenada(l.ObtenerCabeza()) || l.ObtenerMediana() != 42.0) return false;
    }

    // Caso 3: Lista de 2 elementos desordenados (mediana con N par)
    {
        ListaPuntajes l;
        l.InsertarFinal(50);
        l.InsertarFinal(10);
        l.MergeSort();
        if (!ListaEstaOrdenada(l.ObtenerCabeza())) return false;
        if (l.ObtenerMediana() != 30.0) return false;
    }

    // Caso 4: Lista ya ordenada
    {
        ListaPuntajes l;
        l.InsertarFinal(10);
        l.InsertarFinal(20);
        l.InsertarFinal(30);
        l.InsertarFinal(40);
        l.InsertarFinal(50);
        l.MergeSort();
        if (!ListaEstaOrdenada(l.ObtenerCabeza())) return false;
        if (l.ObtenerMediana() != 30.0) return false;
    }

    // Caso 5: Lista en orden inverso
    {
        ListaPuntajes l;
        l.InsertarFinal(100);
        l.InsertarFinal(80);
        l.InsertarFinal(60);
        l.InsertarFinal(40);
        l.InsertarFinal(20);
        l.MergeSort();
        if (!ListaEstaOrdenada(l.ObtenerCabeza())) return false;
        if (l.ObtenerMediana() != 60.0) return false;
    }

    // Caso 6: Lista con duplicados y longitud par
    {
        ListaPuntajes l;
        l.InsertarFinal(7);
        l.InsertarFinal(2);
        l.InsertarFinal(7);
        l.InsertarFinal(1);
        l.InsertarFinal(9);
        l.InsertarFinal(2);
        l.MergeSort();
        if (!ListaEstaOrdenada(l.ObtenerCabeza())) return false;
        if (l.ObtenerMediana() != 4.5) return false;
    }

    return true;
}

/*
 * Funcion: ProbarP13
 * Descripcion: Prueba la persistencia en disco guardando un CSV, inyectando manualmente
 * una linea corrupta para simular un archivo danado, y verificando que CargarCSV omita la linea invalida sin caerse.
 * Entradas: Ninguna
 * Salidas: True si los datos validos persisten y la linea defectuosa es descartada correctamente
 */
bool ProbarP13() {
    const std::string archivoPrueba = "test_historial_temp.csv";

    // Guardamos 2 registros validos en el archivo de prueba
    HistorialPartidas original;

    RegistroPartida r1 = { 1, 42, "por_turnos", "Ana", false, 1, 2500, 5, 100, 80, 20, 15, 2, 22, 1800, 10, 4, 2, 4, 1, 0 };
    RegistroPartida r2 = { 2, 42, "por_turnos", "Luis", false, 2, 1800, 4, 90, 60, 30, 8, 1, 35, 1500, 8, 2, 1, 2, 0, 1 };

    original.AgregarFinal(r1);
    original.AgregarFinal(r2);

    if (!original.GuardarCSV(archivoPrueba)) {
        return false;
    }

    // Inyectamos intencionalmente una linea con formato incompleto
    {
        std::ofstream corruptor(archivoPrueba, std::ios::app);
        corruptor << "linea_corrupta,invalida,999\n";
    }

    // Creamos una nueva instancia limpia para comprobar la lectura
    HistorialPartidas cargado;

    if (!cargado.CargarCSV(archivoPrueba)) {
        std::remove(archivoPrueba.c_str());
        return false;
    }

    // Comprobamos que unicamente se cargaron los 2 registros validos
    if (cargado.Tamano() != 2) {
        std::remove(archivoPrueba.c_str());
        return false;
    }

    // Limpiamos el archivo de prueba del disco
    std::remove(archivoPrueba.c_str());

    return true;
}

/*
 * Funcion: ProbarP14
 * Descripcion: Prueba de integracion completa: simula una partida entera con 4 bots
 * a lo largo de 10 oleadas para asegurar que no se produzcan bloqueos, ciclos infinitos ni caidas de memoria.
 * Entradas: Ninguna
 * Salidas: True si la partida finaliza normalmente y todos los jugadores quedan clasificados
 */
bool ProbarP14() {
    SesionJuego sesion;
    sesion.RegistrarJugador("BOT1", true);
    sesion.RegistrarJugador("BOT2", true);
    sesion.RegistrarJugador("BOT3", true);
    sesion.RegistrarJugador("BOT4", true);

    uint32_t semilla = 42;
    int oleadas = 10;
    sesion.Iniciar(semilla, oleadas);

    int totalTicksSimulados = 0;
    const int MAX_TICKS_SEGURIDAD = 60000;

    // Bucle de ejecucion de la sesion por turnos
    while (!sesion.EstaTerminada() && totalTicksSimulados < MAX_TICKS_SEGURIDAD) {
        if (sesion.ComenzarTurno()) {
            while (sesion.TurnoEnCurso() && totalTicksSimulados < MAX_TICKS_SEGURIDAD) {
                AccionTick act = sesion.DecidirAccionBot();
                EstadoMotor estado = sesion.EjecutarTick(act);
                totalTicksSimulados++;
                if (estado != ESTADO_EN_JUEGO) {
                    sesion.CerrarTurno(estado);
                    break;
                }
            }
        } else {
            break;
        }
    }

    sesion.Finalizar();

    if (!sesion.EstaTerminada()) return false;
    for (int p = 1; p <= 4; p++) {
        int idx = sesion.GetIndiceEnPosicion(p);
        if (idx < 0 || idx >= 4) return false;
    }

    return true;
}

/*
 * Funcion: EjecutarTodasLasPruebas
 * Descripcion: Ejecuta secuencialmente las 14 pruebas unitarias de la suite y muestra en consola
 * el resultado detallado de cada una junto con el balance total de pruebas superadas.
 * Entradas: Ninguna
 * Salidas: Imprime el reporte en la salida estandar
 */
void EjecutarTodasLasPruebas() {
    std::cout << "========================================\n";
    std::cout << "      BANCO DE PRUEBAS MINIMAS (14/14)  \n";
    std::cout << "========================================\n\n";

    int exitosas = 0;
    int total = 14;

    if (ProbarP01()) { std::cout << "[PASO] P01: ListaEnemigos - Insercion y eliminacion de 1000 nodos\n"; exitosas++; }
    else { std::cout << "[FALLO] P01: ListaEnemigos\n"; }

    if (ProbarP02()) { std::cout << "[PASO] P02: ListaEnemigos - Eliminacion de cabeza, cola y nodo unico\n"; exitosas++; }
    else { std::cout << "[FALLO] P02: ListaEnemigos\n"; }

    if (ProbarP03()) { std::cout << "[PASO] P03: ListaEnemigos - InsertarDespues sobre la cola\n"; exitosas++; }
    else { std::cout << "[FALLO] P03: ListaEnemigos\n"; }

    if (ProbarP04()) { std::cout << "[PASO] P04: ListaBalas - Eliminacion consecutiva con doble puntero\n"; exitosas++; }
    else { std::cout << "[FALLO] P04: ListaBalas\n"; }

    if (ProbarP05()) { std::cout << "[PASO] P05: ColaSpawn - Encolar y desencolar hasta vaciar\n"; exitosas++; }
    else { std::cout << "[FALLO] P05: ColaSpawn\n"; }

    if (ProbarP06()) { std::cout << "[PASO] P06: ColaSpawn - Copia profunda sin doble liberacion\n"; exitosas++; }
    else { std::cout << "[FALLO] P06: ColaSpawn\n"; }

    if (ProbarP07()) { std::cout << "[PASO] P07: RondaJugadores - Rotacion y eliminacion circular\n"; exitosas++; }
    else { std::cout << "[FALLO] P07: RondaJugadores\n"; }

    if (ProbarP08()) { std::cout << "[PASO] P08: Motor - Reproducibilidad determinista con misma semilla\n"; exitosas++; }
    else { std::cout << "[FALLO] P08: Motor (Reproducibilidad)\n"; }

    if (ProbarP09()) { std::cout << "[PASO] P09: Motor - Impacto con color erroneo (r+9, combo=0, vida igual)\n"; exitosas++; }
    else { std::cout << "[FALLO] P09: Motor (Impacto erroneo)\n"; }

    if (ProbarP10()) { std::cout << "[PASO] P10: Motor - Baja de Divisor (2 Minis con colores distintos)\n"; exitosas++; }
    else { std::cout << "[FALLO] P10: Motor (Baja de Divisor)\n"; }

    if (ProbarP11()) { std::cout << "[PASO] P11: Motor - Pulso con carga < 100 vs = 100 y bajas en banda\n"; exitosas++; }
    else { std::cout << "[FALLO] P11: Motor (Pulso)\n"; }

    if (ProbarP12()) { std::cout << "[PASO] P12: Estadisticas - Merge Sort sobre lista y mediana (par/impar)\n"; exitosas++; }
    else { std::cout << "[FALLO] P12: Estadisticas (Merge Sort)\n"; }

    if (ProbarP13()) { std::cout << "[PASO] P13: Persistencia - Guardar, cargar y omision de lineas invalidas\n"; exitosas++; }
    else { std::cout << "[FALLO] P13: Persistencia\n"; }

    if (ProbarP14()) { std::cout << "[PASO] P14: Memoria - Partida completa con 4 bots y 10 oleadas\n"; exitosas++; }
    else { std::cout << "[FALLO] P14: Memoria (Simulacion 4 bots)\n"; }

    std::cout << "\n----------------------------------------\n";
    std::cout << "Resultado: " << exitosas << " / " << total << " pruebas pasadas con exito.\n";
    std::cout << "----------------------------------------\n";
}
