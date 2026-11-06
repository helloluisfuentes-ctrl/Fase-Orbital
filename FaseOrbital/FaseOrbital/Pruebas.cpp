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

bool ProbarP01() {
    ListaEnemigos lista;

    // Insertar 1000 enemigos con ID del 1 al 1000
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

    // Eliminar los de ID par durante el recorrido
    NodoEnemigo* aux = lista.Primero();
    while (aux != NULL) {
        if (aux->enemigo.id % 2 == 0) {
            // Eliminar retorna el siguiente nodo, permitiendo continuar sin saltos
            aux = lista.Eliminar(aux);
        } else {
            aux = aux->siguiente;
        }
    }

    // verificacion, el tamaño debe ser exactamente 500
    if (lista.Tamano() != 500) {
        return false;
    }

    //Verificación, recorrido hacia adelante (contar y validar que todos sean impares)
    int conteoAdelante = 0;
    aux = lista.Primero();
    while (aux != NULL) {
        if (aux->enemigo.id % 2 == 0) return false; // No debe quedar ningún par
        conteoAdelante++;
        aux = aux->siguiente;
    }

    // Verificación  recorrido hacia atrás usando punteros 'anterior'
    int conteoAtras = 0;
    aux = lista.Ultimo();
    while (aux != NULL) {
        conteoAtras++;
        aux = aux->anterior;
    }

    // Ambos recorridos deben coincidir en 500 nodos
    return (conteoAdelante == 500 && conteoAtras == 500);
}

bool ProbarP02() {
    ListaEnemigos lista;

    //Sub-prueba Eliminar un nodo unico 
    NodoEnemigo* n1 = new NodoEnemigo;
    n1->enemigo.id = 99;
    lista.InsertarFinal(n1);

    // Debe tener tamaño 1 y cabeza igual a cola
    if (lista.Tamano() != 1 || lista.Primero() != lista.Ultimo()) return false;

    // Eliminamos el único nodo
    lista.Eliminar(lista.Primero());

    // La lista DEBE quedar vacía y con cabeza y cola en NULL
    if (lista.Tamano() != 0 || lista.Primero() != NULL || lista.Ultimo() != NULL || !lista.Vacia()) {
        return false;
    }

    // Sub-prueba Eliminar CABEZA y COLA en lista con varios nodos 
    NodoEnemigo* nodoA = new NodoEnemigo; nodoA->enemigo.id = 1;
    NodoEnemigo* nodoB = new NodoEnemigo; nodoB->enemigo.id = 2;
    NodoEnemigo* nodoC = new NodoEnemigo; nodoC->enemigo.id = 3;

    lista.InsertarFinal(nodoA);
    lista.InsertarFinal(nodoB);
    lista.InsertarFinal(nodoC);

    // Eliminar la CABEZA 
    lista.Eliminar(nodoA);

    if (lista.Primero() != nodoB) return false;          // Nueva cabeza debe ser B
    if (lista.Primero()->anterior != NULL) return false; // Su 'anterior' debe ser NULL
    if (lista.Tamano() != 2) return false;

    // Eliminar la COLA 
    lista.Eliminar(nodoC);

    if (lista.Ultimo() != nodoB) return false;          // Nueva cola debe ser B
    if (lista.Ultimo()->siguiente != NULL) return false; // Su 'siguiente' debe ser NULL
    if (lista.Tamano() != 1) return false;

    //Eliminar el nodo restante 
    lista.Eliminar(nodoB);

    if (lista.Tamano() != 0 || lista.Primero() != NULL || lista.Ultimo() != NULL) {
        return false;
    }

    return true;
}

bool ProbarP03() {
    ListaEnemigos lista;

    // Insertamos 2 nodos iniciales
    NodoEnemigo* n1 = new NodoEnemigo; n1->enemigo.id = 1;
    NodoEnemigo* n2 = new NodoEnemigo; n2->enemigo.id = 2;
    lista.InsertarFinal(n1);
    lista.InsertarFinal(n2);

    // En este momento, la cola es n2
    if (lista.Ultimo() != n2) return false;

    // Insertamos n3 DESPUÉS del último nodo
    NodoEnemigo* n3 = new NodoEnemigo; n3->enemigo.id = 3;
    lista.InsertarDespues(n2, n3);

    // Verificación La cola de la lista DEBE haberse actualizado a n3
    if (lista.Ultimo() != n3) {
        return false;
    }

    // Verificación El tamaño debe ser 3
    if (lista.Tamano() != 3) {
        return false;
    }

    // Verificación Las conexiones de n3 deben ser correctas
    if (n3->siguiente != NULL) return false;
    if (n3->anterior != n2) return false;
    if (n2->siguiente != n3) return false;

    // Verificación Recorrido hacia atrás desde la nueva cola
    NodoEnemigo* aux = lista.Ultimo();
    if (aux->enemigo.id != 3) return false;
    aux = aux->anterior;
    if (aux->enemigo.id != 2) return false;
    aux = aux->anterior;
    if (aux->enemigo.id != 1) return false;
    if (aux->anterior != NULL) return false;

    return true;
}

// Función de condición para marcar balas
bool EsBalaMarcada(Bala b) {
    return b.duenio == 0; // Las balas con dueño 0 deben borrarse
}

bool ProbarP04() {
    ListaBalas lista;

    //Como AgregarInicio inserta por la cabeza, las agregamos en orden inverso:

    // Válida 2 (duenio = 2)
    NodoBala* b6 = new NodoBala; b6->bala.duenio = 2; b6->siguiente = NULL;
    lista.AgregarInicio(b6);

    // Borrar consecutiva (duenio = 0)
    NodoBala* b5 = new NodoBala; b5->bala.duenio = 0; b5->siguiente = NULL;
    lista.AgregarInicio(b5);

    // Borrar (duenio = 0)
    NodoBala* b4 = new NodoBala; b4->bala.duenio = 0; b4->siguiente = NULL;
    lista.AgregarInicio(b4);

    // Válida 1 (duenio = 1)
    NodoBala* b3 = new NodoBala; b3->bala.duenio = 1; b3->siguiente = NULL;
    lista.AgregarInicio(b3);

    // Borrar consecutiva (duenio = 0)
    NodoBala* b2 = new NodoBala; b2->bala.duenio = 0; b2->siguiente = NULL;
    lista.AgregarInicio(b2);

    // Borrar CABEZA (duenio = 0)
    NodoBala* b1 = new NodoBala; b1->bala.duenio = 0; b1->siguiente = NULL;
    lista.AgregarInicio(b1);

    // Estado inicial 6 balas, 4 marcadas para borrar 
    if (lista.Tamano() != 6) return false;

    // Ejecutamos tu función con doble puntero
    lista.EliminarSi(EsBalaMarcada);

    // Verificación  El tamaño resultante debe ser exactamente 2
    if (lista.Tamano() != 2) {
        return false;
    }

    // Verificación La nueva cabeza debe ser Válida 1 
    NodoBala* aux = lista.ObtenerCabeza();
    if (aux == NULL || aux->bala.duenio != 1) {
        return false;
    }

    // Verificación El segundo nodo debe ser Válida 2 
    aux = aux->siguiente;
    if (aux == NULL || aux->bala.duenio != 2) {
        return false;
    }

    // Verificación El siguiente debe ser NULL
    if (aux->siguiente != NULL) {
        return false;
    }

    return true;
}

bool ProbarP05() {
    ColaSpawn cola;
    Spawn temp;

    // Verificación inicial desencolar en vacía DEBE retornar false
    if (cola.Desencolar(temp) != false) return false;
    if (cola.VerFrente() != NULL) return false;
    if (!cola.Vacia() || cola.Tamano() != 0) return false;

    // Encolamos 3 enemigos con diferentes ticks
    Spawn s1 = { 60, DRON, CIAN, 0, 0 };
    Spawn s2 = { 85, GIRADOR, MAGENTA, 45, 0 };
    Spawn s3 = { 110, DIVISOR, AMBAR, 90, 0 };

    cola.Encolar(s1);
    cola.Encolar(s2);
    cola.Encolar(s3);

    if (cola.Tamano() != 3) return false;

    // Desencolar en orden FIFO
    // Sale el primero (tick 60)
    if (!cola.Desencolar(temp) || temp.tick != 60) return false;
    if (cola.Tamano() != 2) return false;

    // Sale el segundo (tick 85)
    if (!cola.Desencolar(temp) || temp.tick != 85) return false;
    if (cola.Tamano() != 1) return false;

    // Sale el tercero y ÚLTIMO (tick 110)
    if (!cola.Desencolar(temp) || temp.tick != 110) return false;

    // Verificación final: La cola quedó completamente vacía
    if (cola.Tamano() != 0) return false;
    if (!cola.Vacia()) return false;
    if (cola.VerFrente() != NULL) return false;

    // Desencolar de nuevo en vacía debe retornar false
    if (cola.Desencolar(temp) != false) return false;

    return true;
}

bool ProbarP06() {
    ColaSpawn original;

    // Llenamos la cola original con 3 enemigos
    Spawn s1 = { 10, DRON, CIAN, 0, 0 };
    Spawn s2 = { 20, TANQUE, MAGENTA, 180, 0 };
    Spawn s3 = { 30, DIVISOR, AMBAR, 90, 0 };

    original.Encolar(s1);
    original.Encolar(s2);
    original.Encolar(s3);

    if (original.Tamano() != 3) return false;

    // Bloque anidado para probar ciclo de vida y destrucción de la copia
    {
        // Se ejecuta tu Constructor de Copia Profunda
        ColaSpawn copia(original);

        // Verificamos que la copia tenga los datos
        if (copia.Tamano() != 3) return false;
        if (copia.VerFrente() == NULL || copia.VerFrente()->tick != 10) return false;

        // vaciamos la copia completamente
        Spawn salida;
        while (!copia.Vacia()) {
            copia.Desencolar(salida);
        }

        if (copia.Tamano() != 0 || !copia.Vacia()) return false;

        // verificacion La original DEBE seguir intacta
        if (original.Tamano() != 3) return false;
        if (original.VerFrente() == NULL || original.VerFrente()->tick != 10) return false;

        
    }

    //  Fuera del bloque la copia ya murió y liberó su memoria.
    // La original debe seguir viva sin punteros rotos ni doble liberación.
    if (original.Tamano() != 3) return false;

    // Vaciamos la original y comprobamos que sus nodos siguen perfectos
    Spawn temp;
    if (!original.Desencolar(temp) || temp.tick != 10) return false;
    if (!original.Desencolar(temp) || temp.tick != 20) return false;
    if (!original.Desencolar(temp) || temp.tick != 30) return false;

    if (original.Tamano() != 0 || !original.Vacia()) return false;

    return true;
}

bool ProbarP07() {
    RondaJugadores ronda;

    // Creamos 3 jugadores de prueba
    Jugador* j1 = new Jugador; j1->alias = "Ana";
    Jugador* j2 = new Jugador; j2->alias = "Luis";
    Jugador* j3 = new Jugador; j3->alias = "Carlos";

    ronda.Agregar(j1);
    ronda.Agregar(j2);
    ronda.Agregar(j3);

    if (ronda.Tamano() != 3) return false;

    // Probar rotación en ambos sentidos
    if (ronda.VerActual() != j1) return false;

    // Horario
    ronda.Avanzar(true);
    if (ronda.VerActual() != j2) return false;
    ronda.Avanzar(true);
    if (ronda.VerActual() != j3) return false;
    ronda.Avanzar(true);
    if (ronda.VerActual() != j1) return false;

    // Antihorario
    ronda.Avanzar(false);
    if (ronda.VerActual() != j3) return false; 
    ronda.Avanzar(false);
    if (ronda.VerActual() != j2) return false;

    // Eliminar al jugador actual y avanzar horario
    Jugador* eliminado1 = ronda.EliminarActual(true);
    if (eliminado1 != j2) return false;
    if (ronda.Tamano() != 2) return false;
    if (ronda.VerActual() != j3) return false;

    // Eliminar a J3 y dejar un ÚNICO jugador
    Jugador* eliminado2 = ronda.EliminarActual(true);
    if (eliminado2 != j3) return false;
    if (ronda.Tamano() != 1) return false;
    if (ronda.VerActual() != j1) return false;

    // Con 1 solo jugador, rotar hacia adelante o hacia atrás debe mantenerse en J1
    ronda.Avanzar(true);
    if (ronda.VerActual() != j1) return false;
    ronda.Avanzar(false);
    if (ronda.VerActual() != j1) return false;

    // 4. Eliminar al ÚLTIMO y ÚNICO jugador
    Jugador* eliminado3 = ronda.EliminarActual(true);
    if (eliminado3 != j1) return false;

    // La ronda debe quedar totalmente vacía y limpia
    if (ronda.Tamano() != 0 || !ronda.Vacia() || ronda.VerActual() != NULL) {
        return false;
    }

    // Liberamos la memoria de los structs de los jugadores
    delete j1;
    delete j2;
    delete j3;

    return true;
}

bool ProbarP08() {
    uint32_t semilla = 42;
    int oleada = 1;

    // Ejecucion 1
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

    // Ejecucion 2 con misma semilla e identicas acciones
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

    // Comparacion estricta de estado final
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

    // Comparacion de eventos en la bitacora
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
    j.combo = 5; // re-asignar combo inicial

    for (int t = 0; t < 25; t++) {
        AccionTick act;
        act.anguloObjetivo = 270.0;
        act.colorObjetivo = CIAN; // Color Cian dispara contra Tanque Magenta
        act.activarPulso = false;
        motor.ActualizarTick(act);
        if (motor.GetErroneosTurno() > 0) break;
    }

    // Verificaciones:
    if (motor.GetErroneosTurno() != 1) return false;
    if (j.combo != 0) return false; // Combo reseteado a 0
    if (motor.GetListaEnemigos().Tamano() != 1) return false;
    NodoEnemigo* nodoTanque = motor.GetListaEnemigos().Primero();
    if (nodoTanque == NULL) return false;
    if (nodoTanque->enemigo.vida != 3) return false; // Vida sin cambio
    if (nodoTanque->enemigo.r < 285.0) return false; // r aumento 9 u

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

    int siguienteId = 10;
    GenerarMinis(nDivisor, lista, 1, siguienteId);
    lista.Eliminar(nDivisor);

    if (lista.Tamano() != 4) return false;

    NodoEnemigo* primero = lista.Primero();
    if (primero != nA) return false;

    NodoEnemigo* m1 = primero->siguiente;
    if (m1 == NULL || m1->enemigo.tipo != MINI) return false;

    NodoEnemigo* m2 = m1->siguiente;
    if (m2 == NULL || m2->enemigo.tipo != MINI) return false;

    if (m2->siguiente != nB) return false;
    if (lista.Ultimo() != nB) return false;

    if (std::abs(m1->enemigo.r - 180.0) > 0.001) return false;
    if (std::abs(m2->enemigo.r - 180.0) > 0.001) return false;

    double t1 = m1->enemigo.theta;
    double t2 = m2->enemigo.theta;
    bool angulosCorrectos = (std::abs(t1 - 116.0) < 0.001 && std::abs(t2 - 124.0) < 0.001) ||
                            (std::abs(t1 - 124.0) < 0.001 && std::abs(t2 - 116.0) < 0.001);
    if (!angulosCorrectos) return false;

    Color c1 = m1->enemigo.color;
    Color c2 = m2->enemigo.color;
    if (c1 == CIAN || c2 == CIAN) return false;
    if (c1 == c2) return false;
    if (!((c1 == MAGENTA && c2 == AMBAR) || (c1 == AMBAR && c2 == MAGENTA))) return false;

    if (nB->anterior != m2 || m2->anterior != m1 || m1->anterior != nA) return false;

    return true;
}

bool ProbarP11() {
    // Parte 1: Carga menor a 100 (sin efecto)
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

    // Parte 2: Carga igual a 100 (activacion y destruccion en banda)
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




// Función auxiliar para verificar si una lista enlazada está ordenada ascendente
bool ListaEstaOrdenada(NodoPuntaje* cabeza) {
    if (cabeza == NULL) return true;
    NodoPuntaje* actual = cabeza;
    while (actual->siguiente != NULL) {
        if (actual->valor > actual->siguiente->valor) {
            return false; // Encontró un elemento mayor antes que uno menor
        }
        actual = actual->siguiente;
    }
    return true;
}

bool ProbarP12() {
    // Caso Lista VACÍA
    {
        ListaPuntajes l;
        l.MergeSort();
        if (l.Tamano() != 0 || l.ObtenerMediana() != 0.0) return false;
    }

    //Caso Lista de 1 ELEMENTO
    {
        ListaPuntajes l;
        l.InsertarFinal(42);
        l.MergeSort();
        if (!ListaEstaOrdenada(l.ObtenerCabeza()) || l.ObtenerMediana() != 42.0) return false;
    }

    //Caso Lista de 2 ELEMENTOS desordenados
    {
        ListaPuntajes l;
        l.InsertarFinal(50);
        l.InsertarFinal(10);
        l.MergeSort();
        if (!ListaEstaOrdenada(l.ObtenerCabeza())) return false;
        // Mediana con N par (10 y 50): (10 + 50) / 2.0 = 30.0
        if (l.ObtenerMediana() != 30.0) return false;
    }

    //Caso Lista YA ORDENADA
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

    //Caso Lista INVERSA 
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

    //Caso Lista con REPETIDOS y N par 
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

    return true; // Pasó todos los escenarios
}

bool ProbarP13() {
    const std::string archivoPrueba = "test_historial_temp.csv";

    // Crear historial original y agregar 2 registros válidos
    HistorialPartidas original;

    RegistroPartida r1 = { 1, 42, "por_turnos", "Ana", false, 1, 2500, 5, 100, 80, 20, 15, 2, 22, 1800, 10, 4, 2, 4, 1, 0 };
    RegistroPartida r2 = { 2, 42, "por_turnos", "Luis", false, 2, 1800, 4, 90, 60, 30, 8, 1, 35, 1500, 8, 2, 1, 2, 0, 1 };

    original.AgregarFinal(r1);
    original.AgregarFinal(r2);

    // Guardamos en el archivo CSV
    if (!original.GuardarCSV(archivoPrueba)) {
        return false;
    }

    // Inyectamos a propósito una LÍNEA INVÁLIDA 
    {
        std::ofstream corruptor(archivoPrueba, std::ios::app);
        corruptor << "linea_corrupta,invalida,999\n";
    }

    // Simular "reiniciar el programa, Creamos una instancia nueva e independiente
    HistorialPartidas cargado;

    // Cargamos el archivo (debe omitir la línea corrupta y reportarla)
    if (!cargado.CargarCSV(archivoPrueba)) {
        std::remove(archivoPrueba.c_str());
        return false;
    }

    // Verificación: Solo deben existir los 2 registros válidos (la línea corrupta fue omitida)
    if (cargado.Tamano() != 2) {
        std::remove(archivoPrueba.c_str());
        return false;
    }

    // Borramos el archivo temporal para no dejar basura en el disco
    std::remove(archivoPrueba.c_str());

    return true;
}

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
