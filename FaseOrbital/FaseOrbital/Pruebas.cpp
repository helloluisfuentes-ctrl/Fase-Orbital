#include "Pruebas.h"
#include "ColaSpawn.h"
#include "ListaEnemigos.h"
#include "RondaJugadores.h"
#include "ListaBalas.h"
#include "HistorialPartidas.h"
#include <iostream>
#include <fstream>
#include <cstdio>

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
    if (cola.VerFrente() != nullptr) return false;
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
    if (cola.VerFrente() != nullptr) return false;

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
        if (copia.VerFrente() == nullptr || copia.VerFrente()->tick != 10) return false;

        // vaciamos la copia completamente
        Spawn salida;
        while (!copia.Vacia()) {
            copia.Desencolar(salida);
        }

        if (copia.Tamano() != 0 || !copia.Vacia()) return false;

        // verificacion La original DEBE seguir intacta
        if (original.Tamano() != 3) return false;
        if (original.VerFrente() == nullptr || original.VerFrente()->tick != 10) return false;

        
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
    if (ronda.Tamano() != 0 || !ronda.Vacia() || ronda.VerActual() != nullptr) {
        return false;
    }

    // Liberamos la memoria de los structs de los jugadores
    delete j1;
    delete j2;
    delete j3;

    return true;
}



// Función auxiliar para verificar si una lista enlazada está ordenada ascendente
bool ListaEstaOrdenada(NodoPuntaje* cabeza) {
    if (cabeza == nullptr) return true;
    NodoPuntaje* actual = cabeza;
    while (actual->siguiente != nullptr) {
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

void EjecutarTodasLasPruebas() {
    std::cout << "========================================\n";
    std::cout << "      BANCO DE PRUEBAS MINIMAS          \n";
    std::cout << "========================================\n\n";

    if (ProbarP01()) {
        std::cout << "[PASO] P01: ListaEnemigos - Insercion y eliminacion de 1000 nodos\n";
    } else {
        std::cout << "[FALLO] P01: ListaEnemigos\n";
    }

    if (ProbarP02()) {
        std::cout << "[PASO] P02: ListaEnemigos - Eliminacion de cabeza, cola y nodo unico\n";
    }
    else {
        std::cout << "[FALLO] P02: ListaEnemigos\n";
    }

    if (ProbarP03()) {
        std::cout << "[PASO] P03: ListaEnemigos - InsertarDespues sobre la cola\n";
    }
    else {
        std::cout << "[FALLO] P03: ListaEnemigos\n";
    }

    if (ProbarP04()) {
        std::cout << "[PASO] P04: ListaBalas - Eliminacion consecutiva con doble puntero\n";
    }
    else {
        std::cout << "[FALLO] P04: ListaBalas\n";
    }

    if (ProbarP05()) {
        std::cout << "[PASO] P05: ColaSpawn - Encolar y desencolar hasta vaciar\n";
    }
    else {
        std::cout << "[FALLO] P05: ColaSpawn\n";
    }

    if (ProbarP06()) {
        std::cout << "[PASO] P06: ColaSpawn - Copia profunda sin doble liberacion\n";
    }
    else {
        std::cout << "[FALLO] P06: ColaSpawn\n";
    }

    if (ProbarP07()) {
        std::cout << "[PASO] P07: RondaJugadores - Rotacion y eliminacion circular\n";
    }
    else {
        std::cout << "[FALLO] P07: RondaJugadores\n";
    }


    if (ProbarP12()) {
        std::cout << "[PASO] P12: Estadisticas - Merge Sort y calculo de Mediana\n";
    }
    else {
        std::cout << "[FALLO] P12: Estadisticas (Merge Sort)\n";
    }

    if (ProbarP13()) {
        std::cout << "[PASO] P13: Persistencia - Guardar, cargar y omision de lineas invalidas\n";
    }
    else {
        std::cout << "[FALLO] P13: Persistencia\n";
    }
}