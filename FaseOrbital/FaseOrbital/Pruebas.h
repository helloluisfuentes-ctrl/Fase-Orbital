#pragma once

/*
 * Archivo: Pruebas.h
 * Descripcion: Declaracion de la bateria de 14 pruebas unitarias exigidas en la especificacion.
 * Cada funcion valida de forma aislada una estructura de datos o mecanica del juego
 * (listas doblemente enlazadas, colas FIFO, listas circulares, ordenamiento Merge Sort, etc.).
 */

// Funciones de prueba individuales (P01 a P14)
bool ProbarP01();
bool ProbarP02();
bool ProbarP03();
bool ProbarP04();
bool ProbarP05();
bool ProbarP06();
bool ProbarP07();
bool ProbarP08();
bool ProbarP09();
bool ProbarP10();
bool ProbarP11();
bool ProbarP12();
bool ProbarP13();
bool ProbarP14();

// Ejecuta las 14 pruebas e imprime el reporte de resultados en la consola
void EjecutarTodasLasPruebas();
