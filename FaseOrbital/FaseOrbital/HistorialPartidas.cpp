#include "HistorialPartidas.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>

// ==========================================
// MÉTODOS BÁSICOS DE ListaPuntajes
// ==========================================

ListaPuntajes::ListaPuntajes() {
    cabeza = NULL;
    tam = 0;
}

ListaPuntajes::~ListaPuntajes() {
    NodoPuntaje* aux = cabeza;
    while (aux != NULL) {
        cabeza = cabeza->siguiente;
        delete aux;
        aux = cabeza;
    }
    tam = 0;
}

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
// MERGE SORT SOBRE LISTA ENLAZADA
// ==========================================

// Dividir la lista en dos mitades usando la Liebre y la Tortuga
NodoPuntaje* ListaPuntajes::Dividir(NodoPuntaje* fuente) {
    if (fuente == NULL || fuente->siguiente == NULL) {
        return NULL;
    }

    NodoPuntaje* lento = fuente;
    NodoPuntaje* rapido = fuente->siguiente;

    // La liebre avanza 2 nodos, la tortuga avanza 1 nodo
    while (rapido != NULL && rapido->siguiente != NULL) {
        lento = lento->siguiente;
        rapido = rapido->siguiente->siguiente;
    }

    NodoPuntaje* mitad = lento->siguiente;
    lento->siguiente = NULL; // Cortamos el enlace para separar las dos listas

    return mitad;
}

// Fusionar dos listas ordenadas en una sola
NodoPuntaje* ListaPuntajes::Fusionar(NodoPuntaje* a, NodoPuntaje* b) {
    NodoPuntaje dummy;
    dummy.siguiente = NULL;
    NodoPuntaje* actual = &dummy;

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

    if (a != NULL) actual->siguiente = a;
    else if (b != NULL) actual->siguiente = b;

    return dummy.siguiente;
}

// Merge sort recursivo 
NodoPuntaje* ListaPuntajes::MergeSortRec(NodoPuntaje* nodo) {
    if (nodo == NULL || nodo->siguiente == NULL) {
        return nodo;
    }

    NodoPuntaje* segundaMitad = Dividir(nodo);

    nodo = MergeSortRec(nodo);
    segundaMitad = MergeSortRec(segundaMitad);

    return Fusionar(nodo, segundaMitad);
}

// Inicia el Merge Sort
void ListaPuntajes::MergeSort() {
    cabeza = MergeSortRec(cabeza);
}

// Cálculo de la mediana sobre la lista ordenada
double ListaPuntajes::ObtenerMediana() const {
    if (cabeza == NULL || tam == 0) {
        return 0.0;
    }

    int indiceMedio = tam / 2;
    NodoPuntaje* aux = cabeza;

    if (tam % 2 != 0) {
        for (int i = 0; i < indiceMedio; i++) {
            aux = aux->siguiente;
        }
        return static_cast<double>(aux->valor);
    } else {
        NodoPuntaje* anterior = NULL;
        for (int i = 0; i < indiceMedio; i++) {
            anterior = aux;
            aux = aux->siguiente;
        }
        return (anterior->valor + aux->valor) / 2.0;
    }
}

// ========================================================================================================
// MÉTODOS DE HistorialPartidas
// ========================================================================================================

//constructor
HistorialPartidas::HistorialPartidas() {
    cabeza = NULL;
    cola = NULL;
    tam = 0;
}

//destructor
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

//agrega un nodo al fina de la lista
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

//Buscamos el puntaje del alias en la lista y lo copiamos en el destino
void HistorialPartidas::CopiarPuntajes(const std::string& aliasJugador, ListaPuntajes& destino) const {
    NodoRegistro* aux = cabeza;
    while (aux != NULL) {
        if (aux->registro.jugador == aliasJugador) {
            destino.InsertarFinal(aux->registro.puntaje);
        }
        aux = aux->siguiente;
    }
}

//carga el archivo csv
bool HistorialPartidas::CargarCSV(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        return false;
    }

    std::string linea;
    // Leer y descartar la cabecera
    if (!std::getline(archivo, linea)) {
        return true; // Archivo vacío
    }

    int numLinea = 1;

    while (std::getline(archivo, linea)) {
        numLinea++;
        if (linea.empty()) continue;

        std::stringstream ss(linea);
        std::string token;
        std::string campos[21];
        int contadorCampos = 0;

        while (std::getline(ss, token, ',')) {
            if (contadorCampos < 21) {
                campos[contadorCampos] = token;
            }
            contadorCampos++;
        }

        // Si la fila no tiene exactamente 21 columnas
        if (contadorCampos != 21) {
            std::cout << "Error: Linea " << numLinea << " mal formada (columnas invalidas). Omitiendo.\n";
            continue;
        }

        // Conversión segura de tipos
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

//uarda en archivo csv
bool HistorialPartidas::GuardarCSV(const std::string& rutaArchivo) const {
    std::ofstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        return false;
    }

    // Cabecera 
    archivo << "id_partida,semilla,modo,jugador,es_bot,posicion,puntaje,oleada,disparos,correctos,erroneos,combo_max,pulsos,dano_total,ticks,bajas_dron,bajas_girador,bajas_divisor,bajas_mini,bajas_tanque,bajas_camaleon\n";

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

//genera un reporte dado un alias
void HistorialPartidas::GenerarReporteJugador(const std::string& aliasJugador) const {
    int partidasJugadas = 0;
    int victorias = 0;
    int mejorPuntaje = 0;
    int peorPuntaje = 0;
    long long sumaPuntajes = 0;
    int totalDisparos = 0;
    int totalCorrectos = 0;
    int totalErroneos = 0;

    // Conteo para encontrar el tipo de enemigo que causó más daño
    int danoDron = 0, danoGirador = 0, danoDivisor = 0, danoMini = 0, danoTanque = 0, danoCamaleon = 0;

    NodoRegistro* aux = cabeza;
    bool primeraPartida = true;

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

    //Media
    double media = static_cast<double>(sumaPuntajes) / partidasJugadas;

    //Mediana usando Merge Sort sobre Lista Auxiliar 
    ListaPuntajes listaAux;
    CopiarPuntajes(aliasJugador, listaAux);
    listaAux.MergeSort();
    double mediana = listaAux.ObtenerMediana();

    //Desviación estándar poblacional: sqrt( sum( (x_i - mu)^2 ) / N )
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

    // Precisión de color promedio
    double precisionPromedio = 0.0;
    if ((totalCorrectos + totalErroneos) > 0) {
        precisionPromedio = (static_cast<double>(totalCorrectos) / (totalCorrectos + totalErroneos)) * 100.0;
    }

    // Impresión del Reporte Estadístico (Sección 6.3)
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