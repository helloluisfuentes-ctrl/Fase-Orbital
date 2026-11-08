#include "HistorialPartidas.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>

// ==========================================
// METODOS DE ListaPuntajes
// ==========================================

/*
 * Funcion: Constructor ListaPuntajes
 * Descripcion: Inicializa la lista auxiliar de puntajes vacia con cabeza en NULL y tamano en cero.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
ListaPuntajes::ListaPuntajes() {
    cabeza = NULL;
    tam = 0;
}

/*
 * Funcion: Destructor ~ListaPuntajes
 * Descripcion: Recorre la lista de puntajes liberando la memoria de cada nodo con delete.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
ListaPuntajes::~ListaPuntajes() {
    NodoPuntaje* aux = cabeza;
    while (aux != NULL) {
        cabeza = cabeza->siguiente;
        delete aux;
        aux = cabeza;
    }
    tam = 0;
}

/*
 * Funcion: InsertarFinal
 * Descripcion: Agrega un nuevo valor numerico al final de la lista auxiliar.
 * Entradas: int valor (puntaje obtenido en una partida)
 * Salidas: Lista con el nuevo puntaje al final
 */
void ListaPuntajes::InsertarFinal(int valor) {
    NodoPuntaje* nuevo = new NodoPuntaje;
    nuevo->valor = valor;
    nuevo->siguiente = NULL;

    if (cabeza == NULL) {
        cabeza = nuevo;
    } else {
        NodoPuntaje* aux = cabeza;
        while (aux->siguiente != NULL) {
            aux = aux->siguiente;
        }
        aux->siguiente = nuevo;
    }
    tam++;
}

// ==========================================
// MERGE SORT SOBRE LISTAS ENLAZADAS
// ==========================================

/*
 * Funcion: Dividir
 * Descripcion: Parte la lista enlazada en dos mitades usando la tecnica de dos punteros (liebre y tortuga).
 *              El puntero rapido avanza de dos en dos y el lento de uno en uno hasta alcanzar el punto medio en O(n).
 * Entradas: NodoPuntaje* fuente (cabeza del segmento de lista a dividir)
 * Salidas: Retorna el puntero al inicio de la segunda mitad y corta el enlace de la primera mitad en NULL
 */
NodoPuntaje* ListaPuntajes::Dividir(NodoPuntaje* fuente) {
    if (fuente == NULL || fuente->siguiente == NULL) {
        return NULL;
    }

    NodoPuntaje* lento = fuente;
    NodoPuntaje* rapido = fuente->siguiente;

    // La liebre avanza 2 nodos mientras la tortuga avanza 1 nodo
    while (rapido != NULL && rapido->siguiente != NULL) {
        lento = lento->siguiente;
        rapido = rapido->siguiente->siguiente;
    }

    NodoPuntaje* mitad = lento->siguiente;
    lento->siguiente = NULL; // Cortamos el enlace para separar formalmente las dos listas

    return mitad;
}

/*
 * Funcion: Fusionar
 * Descripcion: Une dos sublistas ya ordenadas comparando sus valores y reasignando punteros sin crear nodos nuevos.
 * Entradas: NodoPuntaje* a, NodoPuntaje* b (cabezas de las dos sublistas ordenadas)
 * Salidas: Retorna la cabeza de la lista combinada y ordenada
 */
NodoPuntaje* ListaPuntajes::Fusionar(NodoPuntaje* a, NodoPuntaje* b) {
    NodoPuntaje dummy;
    dummy.siguiente = NULL;
    NodoPuntaje* actual = &dummy;

    // Comparamos los valores de ambos frentes y enlazamos el menor
    while (a != NULL && b != NULL) {
        if (a->valor <= b->valor) {
            actual->siguiente = a;
            a = a->siguiente;
        } else {
            actual->siguiente = b;
            b = b->siguiente;
        }
        actual = actual->siguiente;
    }

    // Enganchamos los nodos sobrantes de la lista que no se haya terminado
    if (a != NULL) actual->siguiente = a;
    else if (b != NULL) actual->siguiente = b;

    return dummy.siguiente;
}

/*
 * Funcion: MergeSortRec
 * Descripcion: Metodo recursivo que divide la lista a la mitad y fusiona los resultados ordenados en O(n log n).
 * Entradas: NodoPuntaje* nodo (cabeza del tramo a ordenar)
 * Salidas: Retorna la nueva cabeza del tramo completamente ordenado
 */
NodoPuntaje* ListaPuntajes::MergeSortRec(NodoPuntaje* nodo) {
    // Caso base: lista vacia o de un solo nodo (ya esta ordenada)
    if (nodo == NULL || nodo->siguiente == NULL) {
        return nodo;
    }

    // Dividimos la lista en dos partes
    NodoPuntaje* segundaMitad = Dividir(nodo);

    // Ordenamos recursivamente ambas mitades
    nodo = MergeSortRec(nodo);
    segundaMitad = MergeSortRec(segundaMitad);

    // Fusionamos las dos partes ordenadas
    return Fusionar(nodo, segundaMitad);
}

/*
 * Funcion: MergeSort
 * Descripcion: Metodo publico que inicia el ordenamiento Merge Sort sobre la cabeza de la lista.
 * Entradas: Ninguna
 * Salidas: Modifica la lista reordenando los nodos de menor a mayor
 */
void ListaPuntajes::MergeSort() {
    cabeza = MergeSortRec(cabeza);
}

/*
 * Funcion: ObtenerMediana
 * Descripcion: Calcula la mediana de los puntajes recorriendo la lista previamente ordenada.
 *              Si el tamano es impar toma el valor central; si es par promedia los dos del centro.
 * Entradas: Ninguna
 * Salidas: Valor decimal de la mediana
 */
double ListaPuntajes::ObtenerMediana() const {
    if (cabeza == NULL || tam == 0) {
        return 0.0;
    }

    int indiceMedio = tam / 2;
    NodoPuntaje* aux = cabeza;

    // Caso impar: avanzamos exactamente hasta la posicion central
    if (tam % 2 != 0) {
        for (int i = 0; i < indiceMedio; i++) {
            aux = aux->siguiente;
        }
        return static_cast<double>(aux->valor);
    } 
    // Caso par: necesitamos el promedio entre el nodo indiceMedio-1 y el nodo indiceMedio
    else {
        NodoPuntaje* anterior = NULL;
        for (int i = 0; i < indiceMedio; i++) {
            anterior = aux;
            aux = aux->siguiente;
        }
        return (anterior->valor + aux->valor) / 2.0;
    }
}

// ========================================================================================================
// METODOS DE HistorialPartidas
// ========================================================================================================

/*
 * Funcion: Constructor HistorialPartidas
 * Descripcion: Inicializa la lista de registros vacia con cabeza y cola en NULL y tamano en cero.
 * Entradas: Ninguna
 * Salidas: Instancia creada
 */
HistorialPartidas::HistorialPartidas() {
    cabeza = NULL;
    cola = NULL;
    tam = 0;
}

/*
 * Funcion: Destructor ~HistorialPartidas
 * Descripcion: Recorre la lista de registros y libera la memoria de cada fila cargada.
 * Entradas: Ninguna
 * Salidas: Memoria liberada
 */
HistorialPartidas::~HistorialPartidas() {
    NodoRegistro* aux = cabeza;
    while (aux != NULL) {
        cabeza = cabeza->siguiente;
        delete aux;
        aux = cabeza;
    }
    cola = NULL;
    tam = 0;
}

/*
 * Funcion: AgregarFinal
 * Descripcion: Inserta un registro de partida al final de la lista en O(1) usando el puntero cola.
 * Entradas: const RegistroPartida& reg (datos de la partida finalizada)
 * Salidas: Lista con el registro agregado al final
 */
void HistorialPartidas::AgregarFinal(const RegistroPartida& reg) {
    NodoRegistro* nuevo = new NodoRegistro;
    nuevo->registro = reg;
    nuevo->siguiente = NULL;

    if (cabeza == NULL) {
        cabeza = nuevo;
        cola = nuevo;
    } else {
        cola->siguiente = nuevo;
        cola = nuevo;
    }
    tam++;
}

/*
 * Funcion: CopiarPuntajes
 * Descripcion: Filtra los registros que pertenezcan a un jugador especifico y copia sus puntajes
 *              a una lista auxiliar de puntajes para poder ordenarlos con Merge Sort.
 * Entradas: const std::string& aliasJugador, ListaPuntajes& destino
 * Salidas: Lista auxiliar poblada con los puntajes de ese jugador
 */
void HistorialPartidas::CopiarPuntajes(const std::string& aliasJugador, ListaPuntajes& destino) const {
    NodoRegistro* aux = cabeza;
    while (aux != NULL) {
        if (aux->registro.jugador == aliasJugador) {
            destino.InsertarFinal(aux->registro.puntaje);
        }
        aux = aux->siguiente;
    }
}

/*
 * Funcion: CargarCSV
 * Descripcion: Abre un archivo CSV, valida y parsea cada linea esperando exactamente 21 columnas.
 *              Si una linea esta corrupta o tiene tipos invalidos, la reporta y la omite sin detener el programa.
 * Entradas: const std::string& rutaArchivo (ruta al archivo .csv)
 * Salidas: Retorna true si pudo abrir el archivo (o si estaba vacio), false si no se pudo abrir
 */
bool HistorialPartidas::CargarCSV(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        return false;
    }

    std::string linea;
    // Leer y descartar la cabecera
    if (!std::getline(archivo, linea)) {
        return true; // Archivo vacio
    }

    int numLinea = 1;

    // Procesamos linea por linea
    while (std::getline(archivo, linea)) {
        numLinea++;
        if (linea.empty()) continue;

        std::stringstream ss(linea);
        std::string token;
        std::string campos[21];
        int contadorCampos = 0;

        // Separamos por comas los 21 campos del registro
        while (std::getline(ss, token, ',')) {
            if (contadorCampos < 21) {
                campos[contadorCampos] = token;
            }
            contadorCampos++;
        }

        // Validacion de cantidad de columnas
        if (contadorCampos != 21) {
            std::cout << "Error: Linea " << numLinea << " mal formada (columnas invalidas). Omitiendo.\n";
            continue;
        }

        // Conversion segura de tipos de datos para no botar el programa
        try {
            RegistroPartida reg;
            reg.idPartida = std::stoi(campos[0]);
            reg.semilla = static_cast<uint32_t>(std::stoul(campos[1]));
            reg.modo = campos[2];
            reg.jugador = campos[3];
            reg.esBot = (campos[4] == "1" || campos[4] == "true" || campos[4] == "TRUE");
            reg.posicion = std::stoi(campos[5]);
            reg.puntaje = std::stoi(campos[6]);
            reg.oleada = std::stoi(campos[7]);
            reg.disparos = std::stoi(campos[8]);
            reg.correctos = std::stoi(campos[9]);
            reg.erroneos = std::stoi(campos[10]);
            reg.comboMax = std::stoi(campos[11]);
            reg.pulsos = std::stoi(campos[12]);
            reg.danoTotal = std::stoi(campos[13]);
            reg.ticks = std::stoi(campos[14]);
            reg.bajasDron = std::stoi(campos[15]);
            reg.bajasGirador = std::stoi(campos[16]);
            reg.bajasDivisor = std::stoi(campos[17]);
            reg.bajasMini = std::stoi(campos[18]);
            reg.bajasTanque = std::stoi(campos[19]);
            reg.bajasCamaleon = std::stoi(campos[20]);

            AgregarFinal(reg);
        } catch (...) {
            std::cout << "Error: Linea " << numLinea << " mal formada (datos invalidos). Omitiendo.\n";
            continue;
        }
    }

    return true;
}

/*
 * Funcion: GuardarCSV
 * Descripcion: Escribe todos los registros de la lista enlazada al archivo CSV con la cabecera correspondiente.
 * Entradas: const std::string& rutaArchivo (ruta de destino del CSV)
 * Salidas: Retorna true si se pudo escribir el archivo, false si hubo error al abrirlo
 */
bool HistorialPartidas::GuardarCSV(const std::string& rutaArchivo) const {
    std::ofstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        return false;
    }

    // Escribimos la cabecera oficial
    archivo << "id_partida,semilla,modo,jugador,es_bot,posicion,puntaje,oleada,disparos,correctos,erroneos,combo_max,pulsos,dano_total,ticks,bajas_dron,bajas_girador,bajas_divisor,bajas_mini,bajas_tanque,bajas_camaleon\n";

    // Recorremos secuencialmente la lista volcando cada fila
    NodoRegistro* aux = cabeza;
    while (aux != NULL) {
        const RegistroPartida& r = aux->registro;
        archivo << r.idPartida << ","
                << r.semilla << ","
                << r.modo << ","
                << r.jugador << ","
                << (r.esBot ? "1" : "0") << ","
                << r.posicion << ","
                << r.puntaje << ","
                << r.oleada << ","
                << r.disparos << ","
                << r.correctos << ","
                << r.erroneos << ","
                << r.comboMax << ","
                << r.pulsos << ","
                << r.danoTotal << ","
                << r.ticks << ","
                << r.bajasDron << ","
                << r.bajasGirador << ","
                << r.bajasDivisor << ","
                << r.bajasMini << ","
                << r.bajasTanque << ","
                << r.bajasCamaleon << "\n";
        aux = aux->siguiente;
    }

    return true;
}

/*
 * Funcion: GenerarReporteJugador
 * Descripcion: Recorre el historial calculando las metricas de un jugador (partidas, victorias, mejor/peor puntaje,
 *              media, mediana mediante Merge Sort, desviacion estandar y precision) e imprime el reporte en consola.
 * Entradas: const std::string& aliasJugador (nombre del jugador a evaluar)
 * Salidas: Imprime el reporte estadistico en la consola estandar
 */
void HistorialPartidas::GenerarReporteJugador(const std::string& aliasJugador) const {
    int partidasJugadas = 0;
    int victorias = 0;
    int mejorPuntaje = 0;
    int peorPuntaje = 0;
    long long sumaPuntajes = 0;
    int totalDisparos = 0;
    int totalCorrectos = 0;
    int totalErroneos = 0;

    NodoRegistro* aux = cabeza;
    bool primeraPartida = true;

    // Primer recorrido: calculamos suma de puntajes, mejor/peor y victorias
    while (aux != NULL) {
        if (aux->registro.jugador == aliasJugador) {
            const RegistroPartida& r = aux->registro;
            partidasJugadas++;

            if (r.posicion == 1) {
                victorias++;
            }

            if (primeraPartida) {
                mejorPuntaje = r.puntaje;
                peorPuntaje = r.puntaje;
                primeraPartida = false;
            } else {
                if (r.puntaje > mejorPuntaje) mejorPuntaje = r.puntaje;
                if (r.puntaje < peorPuntaje) peorPuntaje = r.puntaje;
            }

            sumaPuntajes += r.puntaje;
            totalDisparos += r.disparos;
            totalCorrectos += r.correctos;
            totalErroneos += r.erroneos;
        }
        aux = aux->siguiente;
    }

    if (partidasJugadas == 0) {
        std::cout << "\nNo hay partidas registradas para el jugador: " << aliasJugador << "\n";
        return;
    }

    // Media
    double media = static_cast<double>(sumaPuntajes) / partidasJugadas;

    // Mediana usando Merge Sort sobre lista enlazada auxiliar
    ListaPuntajes listaAux;
    CopiarPuntajes(aliasJugador, listaAux);
    listaAux.MergeSort();
    double mediana = listaAux.ObtenerMediana();

    // Desviacion estandar poblacional en segundo recorrido: sqrt( sum( (x_i - mu)^2 ) / N )
    double sumaDiferenciasCuadrado = 0.0;
    aux = cabeza;
    while (aux != NULL) {
        if (aux->registro.jugador == aliasJugador) {
            double dif = aux->registro.puntaje - media;
            sumaDiferenciasCuadrado += (dif * dif);
        }
        aux = aux->siguiente;
    }
    double desviacionEstandar = std::sqrt(sumaDiferenciasCuadrado / partidasJugadas);

    // Precision de color promedio
    double precisionPromedio = 0.0;
    if ((totalCorrectos + totalErroneos) > 0) {
        precisionPromedio = (static_cast<double>(totalCorrectos) / (totalCorrectos + totalErroneos)) * 100.0;
    }

    // Impresion formal del reporte en consola
    std::cout << "\n============================================\n";
    std::cout << " ESTADISTICAS HISTORICAS: " << aliasJugador << "\n";
    std::cout << "============================================\n";
    std::cout << "Partidas jugadas: " << partidasJugadas << "\n";
    std::cout << "Victorias:        " << victorias << "\n";
    std::cout << "Mejor puntaje:    " << mejorPuntaje << "\n";
    std::cout << "Peor puntaje:     " << peorPuntaje << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Media:            " << media << "\n";
    std::cout << "Mediana:          " << mediana << " (calculada con Merge Sort)\n";
    std::cout << "Desv. Estandar:   " << desviacionEstandar << "\n";
    std::cout << "Precision color:  " << precisionPromedio << "%\n";
    std::cout << "============================================\n\n";
}
