#include "VistaAllegro.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>
#include <cmath>
#include <ctime>

/*
 * Archivo: VistaAllegro.cpp
 * Descripcion: Implementacion de la interfaz grafica usando Allegro 5.
 * Mapea las coordenadas polares del juego a coordenadas cartesianas de pantalla,
 * captura eventos de teclado y mouse, reproduce sonidos segun los eventos de la bitacora
 * y maneja las transiciones entre el menu, juego, resumen, ranking y estadisticas.
 */

// Dimensiones de la ventana y centro donde ubicamos el nucleo
const int ANCHO = 1000;
const int ALTO = 700;
const float CX = 500;
const float CY = 350;
const double PI_V = 3.14159265358979;
const int OLEADAS = 10;

// Arreglos auxiliares para recordar las posiciones de los enemigos antes de cada tick
// Con esto podemos dibujar una animacion de destello justo donde murieron
const int MAX_FOTO = 300;
static int fotoId[MAX_FOTO];
static float fotoX[MAX_FOTO], fotoY[MAX_FOTO];
static Color fotoColor[MAX_FOTO];
static int totalFoto = 0;

/*
 * Funcion: ColorFase
 * Descripcion: Devuelve el color RGB de Allegro asociado a cada color del juego (Cian, Magenta, Ambar).
 * Entradas: c (Color del enum)
 * Salidas: Estructura ALLEGRO_COLOR correspondiente
 */
static ALLEGRO_COLOR ColorFase(Color c) {
    if (c == CIAN) return al_map_rgb(0, 220, 255);
    if (c == MAGENTA) return al_map_rgb(255, 60, 220);
    return al_map_rgb(255, 180, 0);
}

/*
 * Funcion: Blanco
 * Descripcion: Helper que retorna un color blanco suave para textos y lineas.
 * Entradas: Ninguna
 * Salidas: ALLEGRO_COLOR blanco suave
 */
static ALLEGRO_COLOR Blanco() { return al_map_rgb(230, 230, 240); }

/*
 * Funcion: Gris
 * Descripcion: Helper que retorna un color gris para las orbitas de referencia.
 * Entradas: Ninguna
 * Salidas: ALLEGRO_COLOR gris
 */
static ALLEGRO_COLOR Gris() { return al_map_rgb(130, 130, 150); }

// ============================== INICIO Y CIERRE ==============================

/*
 * Funcion: VistaAllegro (Constructor)
 * Descripcion: Inicializa todos los punteros de Allegro en NULL y setea las variables de control en falso o valores iniciales.
 * Entradas: Ninguna
 * Salidas: Instancia de VistaAllegro lista para inicializar
 */
VistaAllegro::VistaAllegro() {
    display = NULL;
    timer = NULL;
    cola = NULL;
    fuente = NULL;
    fuenteGrande = NULL;
    sonidoBaja = NULL;
    sonidoError = NULL;
    sonidoDano = NULL;
    sonidoPulso = NULL;
    hayAudio = false;

    pantalla = PANTALLA_MENU;
    corriendo = false;
    alias = "";
    aliasEsBot = false;
    izquierda = false;
    derecha = false;
    usarMouse = false;
    anguloMouse = 270;
    fase = CIAN;
    pulso = false;
    ultimoEvento = NULL;
    historial = NULL;
    ranking = NULL;
    totalNombres = 0;
    seleccionado = 0;

    for (int i = 0; i < MAX_DESTELLOS; i++) destellos[i].activo = false;
}

/*
 * Funcion: ~VistaAllegro (Destructor)
 * Descripcion: Libera todas las fuentes, sonidos, cola de eventos y la ventana para evitar fugas de memoria.
 * Entradas: Ninguna
 * Salidas: Ninguna
 */
VistaAllegro::~VistaAllegro() {
    Liberar();
}

/*
 * Funcion: Cargar (Fuente)
 * Descripcion: Intenta cargar una fuente TrueType desde disco e imprime un aviso si no la encuentra.
 * Entradas: f (referencia al puntero de la fuente), ruta (ruta del archivo .ttf), tam (tamano en pixeles)
 * Salidas: Booleano indicando si la carga fue exitosa
 */
bool VistaAllegro::Cargar(ALLEGRO_FONT*& f, const char* ruta, int tam) {
    f = al_load_ttf_font(ruta, tam, 0);
    if (f == NULL) std::cout << "Falta el recurso: " << ruta << "\n";
    return f != NULL;
}

/*
 * Funcion: Cargar (Sonido)
 * Descripcion: Intenta cargar un archivo de sonido .wav; si el sistema no tiene audio habilitado, simplemente ignora el fallo.
 * Entradas: s (referencia al puntero del sample), ruta (ruta del archivo de audio)
 * Salidas: Booleano indicando si la carga fue exitosa o si se juega en silencio
 */
bool VistaAllegro::Cargar(ALLEGRO_SAMPLE*& s, const char* ruta) {
    if (!hayAudio) return true;   // Si no hay tarjeta de sonido permitimos jugar en silencio
    s = al_load_sample(ruta);
    if (s == NULL) std::cout << "Falta el recurso: " << ruta << "\n";
    return s != NULL;
}

/*
 * Funcion: Inicializar
 * Descripcion: Arranca los subsistemas de Allegro (graficos, fuentes, teclado, mouse y sonido) y carga todos los recursos.
 * Entradas: Ninguna
 * Salidas: True si se pudo crear la ventana y cargar los archivos necesarios, false de lo contrario
 */
bool VistaAllegro::Inicializar() {
    if (!al_init() || !al_init_primitives_addon() || !al_init_font_addon() || !al_init_ttf_addon()
        || !al_install_keyboard() || !al_install_mouse()) {
        std::cout << "No se pudo inicializar Allegro\n";
        return false;
    }

    hayAudio = al_install_audio() && al_init_acodec_addon() && al_reserve_samples(8);
    if (!hayAudio) std::cout << "Aviso: no hay audio, se juega sin sonido\n";

    display = al_create_display(ANCHO, ALTO);
    timer = al_create_timer(1.0 / 30.0);
    cola = al_create_event_queue();
    if (display == NULL || timer == NULL || cola == NULL) {
        std::cout << "No se pudo crear la ventana, el timer o la cola\n";
        return false;
    }
    al_set_window_title(display, "Fase Orbital");

    if (!Cargar(fuente, "recursos/fuente.ttf", 16)) return false;
    if (!Cargar(fuenteGrande, "recursos/fuente_negrita.ttf", 32)) return false;
    if (!Cargar(sonidoBaja, "recursos/baja.wav")) return false;
    if (!Cargar(sonidoError, "recursos/error.wav")) return false;
    if (!Cargar(sonidoDano, "recursos/dano.wav")) return false;
    if (!Cargar(sonidoPulso, "recursos/pulso.wav")) return false;

    al_register_event_source(cola, al_get_display_event_source(display));
    al_register_event_source(cola, al_get_timer_event_source(timer));
    al_register_event_source(cola, al_get_keyboard_event_source());
    al_register_event_source(cola, al_get_mouse_event_source());
    return true;
}

/*
 * Funcion: Liberar
 * Descripcion: Destruye los recursos asignados de Allegro y llama a LiberarHistorial para limpiar memoria dinamica.
 * Entradas: Ninguna
 * Salidas: Punteros seteados a NULL
 */
void VistaAllegro::Liberar() {
    LiberarHistorial();
    if (sonidoBaja) al_destroy_sample(sonidoBaja);
    if (sonidoError) al_destroy_sample(sonidoError);
    if (sonidoDano) al_destroy_sample(sonidoDano);
    if (sonidoPulso) al_destroy_sample(sonidoPulso);
    sonidoBaja = sonidoError = sonidoDano = sonidoPulso = NULL;
    if (hayAudio) {
        al_uninstall_audio();
        hayAudio = false;
    }
    if (fuente) al_destroy_font(fuente);
    if (fuenteGrande) al_destroy_font(fuenteGrande);
    fuente = fuenteGrande = NULL;
    if (timer) al_destroy_timer(timer);
    if (cola) al_destroy_event_queue(cola);
    if (display) al_destroy_display(display);
    timer = NULL;
    cola = NULL;
    display = NULL;
}

// ============================== BUCLE PRINCIPAL ==============================

/*
 * Funcion: Ejecutar
 * Descripcion: Bucle principal que espera eventos de la cola de Allegro (teclas, mouse, reloj) y redibuja la pantalla a 30 FPS.
 * Entradas: Ninguna
 * Salidas: Ninguna (corre hasta cerrar la ventana o elegir salir)
 */
void VistaAllegro::Ejecutar() {
    corriendo = true;
    bool redibujar = true;
    al_start_timer(timer);

    // Ciclo de eventos principal
    while (corriendo) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(cola, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            corriendo = false;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            // Cada pulso del temporizador avanza un tick en el motor de juego
            if (pantalla == PANTALLA_JUEGO) TickJuego();
            for (int i = 0; i < MAX_DESTELLOS; i++) {
                if (destellos[i].activo && --destellos[i].vida <= 0) destellos[i].activo = false;
            }
            redibujar = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            Tecla(ev.keyboard.keycode);
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) izquierda = false;
            if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) derecha = false;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
            Caracter(ev.keyboard.keycode, ev.keyboard.unichar);
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES && pantalla == PANTALLA_JUEGO) {
            // Convertimos la posicion del cursor en el angulo polar correspondiente hacia el nucleo
            double dx = ev.mouse.x - CX;
            double dy = CY - ev.mouse.y;   // Invertimos Y porque en la pantalla los pixeles crecen hacia abajo
            anguloMouse = atan2(dy, dx) * 180.0 / PI_V;
            if (anguloMouse < 0) anguloMouse += 360;
            usarMouse = true;
        }

        // Redibujamos solo cuando ya procesamos todos los eventos que estaban encolados
        if (redibujar && al_is_event_queue_empty(cola)) {
            redibujar = false;
            Dibujar();
            al_flip_display();
        }
    }
}

// ============================== TECLADO ==============================

/*
 * Funcion: Tecla
 * Descripcion: Gestiona las pulsaciones de teclas directas segun la pantalla en la que nos encontremos (navegacion, rotacion, cambio de fase).
 * Entradas: tecla (codigo de tecla de Allegro)
 * Salidas: Modifica variables de control o cambia de pantalla
 */
void VistaAllegro::Tecla(int tecla) {
    bool enter = (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_PAD_ENTER);

    switch (pantalla) {
        case PANTALLA_TURNO:
            if (enter) ComenzarTurno();
            break;

        case PANTALLA_JUEGO:
            if (tecla == ALLEGRO_KEY_P || tecla == ALLEGRO_KEY_ESCAPE) IrA(PANTALLA_PAUSA);
            if (tecla == ALLEGRO_KEY_LEFT) { izquierda = true; usarMouse = false; }
            if (tecla == ALLEGRO_KEY_RIGHT) { derecha = true; usarMouse = false; }
            if (tecla == ALLEGRO_KEY_1) fase = CIAN;
            if (tecla == ALLEGRO_KEY_2) fase = MAGENTA;
            if (tecla == ALLEGRO_KEY_3) fase = AMBAR;
            if (tecla == ALLEGRO_KEY_SPACE) pulso = true;
            break;

        case PANTALLA_PAUSA:
            if (tecla == ALLEGRO_KEY_P || tecla == ALLEGRO_KEY_ESCAPE) IrA(PANTALLA_JUEGO);
            if (tecla == ALLEGRO_KEY_Q) {
                sesion.Reiniciar();
                IrA(PANTALLA_MENU);
            }
            break;

        case PANTALLA_RESUMEN:
            if (enter) SiguienteTurno();
            break;

        case PANTALLA_ESTADISTICAS:
            if (tecla == ALLEGRO_KEY_UP && seleccionado > 0) seleccionado--;
            if (tecla == ALLEGRO_KEY_DOWN && seleccionado < totalNombres - 1) seleccionado++;
            if (tecla == ALLEGRO_KEY_ESCAPE) IrA(PANTALLA_MENU);
            break;

        case PANTALLA_FIN:
        case PANTALLA_RANKING:
            if (enter || tecla == ALLEGRO_KEY_ESCAPE) IrA(PANTALLA_MENU);
            break;

        default:
            break;
    }
}

/*
 * Funcion: Caracter
 * Descripcion: Procesa la entrada de caracteres alfanumericos para escribir el alias de los jugadores y navegar opciones del menu.
 * Entradas: tecla (codigo de tecla), c (caracter unicode tipeado)
 * Salidas: Modifica el string de alias, agrega jugadores a la sesion o cambia de pantalla
 */
void VistaAllegro::Caracter(int tecla, int c) {
    // Manejo de opciones numericas en el menu principal
    if (pantalla == PANTALLA_MENU) {
        if (c == '1') {
            sesion.Reiniciar();
            alias = "";
            mensaje = "";
            IrA(PANTALLA_REGISTRO);
        }
        else if (c == '2') IrA(PANTALLA_ESTADISTICAS);
        else if (c == '3') IrA(PANTALLA_RANKING);
        else if (c == '4') corriendo = false;
        return;
    }
    if (pantalla != PANTALLA_REGISTRO) return;

    // Manejo de escritura en la pantalla de registro
    if (tecla == ALLEGRO_KEY_ESCAPE) {
        IrA(PANTALLA_MENU);
    }
    else if (tecla == ALLEGRO_KEY_TAB) {
        aliasEsBot = !aliasEsBot;
    }
    else if (tecla == ALLEGRO_KEY_BACKSPACE) {
        if (!alias.empty()) alias.pop_back();
    }
    else if (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_PAD_ENTER) {
        if (alias.empty()) {
            if (sesion.GetTotalJugadores() == 0) {
                mensaje = "Agregue al menos un jugador";
                return;
            }
            // Obtenemos una semilla dinamica con el tiempo actual para que las partidas sean variadas
            uint32_t semillaPartida = static_cast<uint32_t>(time(NULL));
            sesion.Iniciar(semillaPartida, OLEADAS);
            ultimoEvento = NULL;
            IrA(PANTALLA_TURNO);
            return;
        }
        std::string nombre = alias;
        if (aliasEsBot && !SesionJuego::EsAliasDeBot(nombre)) nombre = "BOT" + nombre;
        if (sesion.RegistrarJugador(nombre, aliasEsBot)) mensaje = nombre + " agregado";
        else mensaje = "No se pudo agregar (repetido o ya hay 6)";
        alias = "";
    }
    else if (alias.size() < 12 && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
        alias += static_cast<char>(c);
    }
}

// ============================== FLUJO ==============================

/*
 * Funcion: IrA
 * Descripcion: Cambia de pantalla y se encarga de cargar o liberar el historial segun sea necesario.
 * Entradas: p (Pantalla destino)
 * Salidas: Actualiza la variable pantalla
 */
void VistaAllegro::IrA(Pantalla p) {
    if (p == PANTALLA_ESTADISTICAS || p == PANTALLA_RANKING) CargarHistorial();
    if (p == PANTALLA_MENU) LiberarHistorial();
    pantalla = p;
}

/*
 * Funcion: ComenzarTurno
 * Descripcion: Configura el inicio del turno del jugador actual en la sesion y pasa a la pantalla de juego.
 * Entradas: Ninguna
 * Salidas: Modifica el estado interno de la sesion y cambia la pantalla a PANTALLA_JUEGO
 */
void VistaAllegro::ComenzarTurno() {
    if (!sesion.ComenzarTurno()) {
        SiguienteTurno();
        return;
    }
    fase = sesion.GetJugadorEnTurno()->faseColor;
    izquierda = derecha = usarMouse = pulso = false;
    for (int i = 0; i < MAX_DESTELLOS; i++) destellos[i].activo = false;
    totalFoto = 0;
    IrA(PANTALLA_JUEGO);
}

/*
 * Funcion: SiguienteTurno
 * Descripcion: Verifica si la partida sigue o termino; si termino, guarda el CSV y muestra la pantalla final.
 * Entradas: Ninguna
 * Salidas: Transicion a PANTALLA_TURNO o PANTALLA_FIN con guardado de datos
 */
void VistaAllegro::SiguienteTurno() {
    if (!sesion.EstaTerminada()) {
        IrA(PANTALLA_TURNO);
        return;
    }
    sesion.Finalizar();
    sesion.GuardarHistorial("historial.csv", mensaje);
    sesion.ImprimirLineasTiempo();
    IrA(PANTALLA_FIN);
}

// ============================== JUEGO ==============================

/*
 * Funcion: TickJuego
 * Descripcion: Ejecuta un ciclo del motor. Obtiene la accion del jugador o bot, aplica el tick, revisa bajas para destellos y suena eventos.
 * Entradas: Ninguna
 * Salidas: Actualiza el motor, las listas de balas y enemigos, y cambia a resumen si el turno termino
 */
void VistaAllegro::TickJuego() {
    const Motor* motor = sesion.GetMotor();
    const Jugador* j = motor->GetJugador();

    // Guardamos una fotografia de los enemigos antes del tick para detectar cuales murieron
    totalFoto = 0;
    for (NodoEnemigo* n = motor->GetListaEnemigos().Primero(); n != NULL && totalFoto < MAX_FOTO; n = n->siguiente) {
        fotoId[totalFoto] = n->enemigo.id;
        fotoColor[totalFoto] = n->enemigo.color;
        Polar(n->enemigo.r, n->enemigo.theta, fotoX[totalFoto], fotoY[totalFoto]);
        totalFoto++;
    }

    // Calculamos la accion: el bot decide solo, o leemos las teclas/mouse del usuario
    AccionTick accion;
    if (j->esBot) {
        accion = sesion.DecidirAccionBot();
    }
    else {
        accion.colorObjetivo = fase;
        accion.activarPulso = pulso;
        accion.anguloObjetivo = j->anguloNave;
        if (izquierda) accion.anguloObjetivo = j->anguloNave + 90;
        else if (derecha) accion.anguloObjetivo = j->anguloNave - 90;
        else if (usarMouse) accion.anguloObjetivo = anguloMouse;
        if (accion.anguloObjetivo >= 360) accion.anguloObjetivo -= 360;
        if (accion.anguloObjetivo < 0) accion.anguloObjetivo += 360;
    }
    pulso = false;

    // Ejecutamos el tick del motor y reproducimos los sonidos de nuevos eventos
    EstadoMotor estado = sesion.EjecutarTick(accion);
    LeerEventos();

    // Comparamos con la foto anterior: los que ya no estan en la lista fueron destruidos
    for (int i = 0; i < totalFoto; i++) {
        bool sigue = false;
        for (NodoEnemigo* n = motor->GetListaEnemigos().Primero(); n != NULL; n = n->siguiente) {
            if (n->enemigo.id == fotoId[i]) {
                sigue = true;
                break;
            }
        }
        if (sigue) continue;
        for (int k = 0; k < MAX_DESTELLOS; k++) {
            if (!destellos[k].activo) {
                destellos[k] = { fotoX[i], fotoY[i], 12, fotoColor[i], true };
                break;
            }
        }
    }

    // Si el jugador perdio o completo los ticks de la oleada, pasamos a la pantalla de resumen
    if (estado != ESTADO_EN_JUEGO) {
        sesion.CerrarTurno(estado);
        IrA(PANTALLA_RESUMEN);
    }
}

/*
 * Funcion: LeerEventos
 * Descripcion: Recorre los nodos recien agregados a la bitacora de eventos para reproducir los efectos de sonido adecuados.
 * Entradas: Ninguna
 * Salidas: Actualiza el puntero ultimoEvento y reproduce sonidos
 */
void VistaAllegro::LeerEventos() {
    NodoEvento* n = (ultimoEvento == NULL) ? sesion.GetBitacora()->ObtenerCabeza() : ultimoEvento->siguiente;
    while (n != NULL) {
        TipoEvento t = n->evento.tipoEvento;
        if (t == BAJA) Sonar(sonidoBaja);
        else if (t == IMPACTO_ERRONEO) Sonar(sonidoError);
        else if (t == DANO_NUCLEO || t == CHOQUE_NAVE) Sonar(sonidoDano);
        else if (t == PULSO) Sonar(sonidoPulso);
        ultimoEvento = n;
        n = n->siguiente;
    }
}

/*
 * Funcion: Sonar
 * Descripcion: Helper para reproducir un sample de sonido si el sistema de audio esta activo.
 * Entradas: s (puntero al sample de audio)
 * Salidas: Reproduccion del audio
 */
void VistaAllegro::Sonar(ALLEGRO_SAMPLE* s) {
    if (hayAudio && s != NULL) al_play_sample(s, 0.6f, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
}

// ============================== HISTORIAL ==============================

/*
 * Funcion: CargarHistorial
 * Descripcion: Instancia dinamicamente el historial y ranking para leer el CSV y armar las listas de estadisticas.
 * Entradas: Ninguna
 * Salidas: Inicializa los punteros historial y ranking en el heap
 */
void VistaAllegro::CargarHistorial() {
    LiberarHistorial();
    historial = new HistorialPartidas();
    ranking = new Ranking();
    historial->CargarCSV("historial.csv");
    totalNombres = ObtenerAliasHistorial(*historial, nombres, MAX_ALIAS_HISTORIAL);
    ConstruirRanking(*historial, *ranking);
    seleccionado = 0;
}

/*
 * Funcion: LiberarHistorial
 * Descripcion: Libera la memoria dinamica de los objetos HistorialPartidas y Ranking.
 * Entradas: Ninguna
 * Salidas: Deja los punteros en NULL y limpia el conteo de nombres
 */
void VistaAllegro::LiberarHistorial() {
    delete historial;
    delete ranking;
    historial = NULL;
    ranking = NULL;
    totalNombres = 0;
}

// ============================== DIBUJO ==============================

/*
 * Funcion: Polar
 * Descripcion: Convierte coordenadas polares (radio r y angulo en grados) a coordenadas de pantalla (x, y).
 * Entradas: r (radio), grados (angulo), x (referencia para salida X), y (referencia para salida Y)
 * Salidas: Coordenadas de pantalla calculadas en x e y
 */
void VistaAllegro::Polar(double r, double grados, float& x, float& y) const {
    double rad = grados * PI_V / 180.0;
    x = CX + static_cast<float>(r * cos(rad));
    y = CY - static_cast<float>(r * sin(rad));
}

/*
 * Funcion: Escribir
 * Descripcion: Dibuja una cadena de texto en pantalla usando la fuente estandar.
 * Entradas: x, y (posicion), s (texto a dibujar), alineacion (0 = izquierda, alineacion de Allegro)
 * Salidas: Dibuja el texto en el buffer de la pantalla
 */
void VistaAllegro::Escribir(float x, float y, const std::string& s, int alineacion) {
    al_draw_text(fuente, Blanco(), x, y, alineacion, s.c_str());
}

/*
 * Funcion: Titulo
 * Descripcion: Dibuja un texto centrado usando la fuente grande para encabezados de pantallas.
 * Entradas: y (posicion vertical), s (texto del encabezado)
 * Salidas: Dibuja el titulo centrado horizontalmente
 */
void VistaAllegro::Titulo(float y, const std::string& s) {
    al_draw_text(fuenteGrande, Blanco(), ANCHO / 2, y, ALLEGRO_ALIGN_CENTRE, s.c_str());
}

/*
 * Funcion: Dibujar
 * Descripcion: Funcion principal de renderizado. Evalua la pantalla actual y dibuja la interfaz o elementos graficos correspondientes.
 * Entradas: Ninguna
 * Salidas: Dibuja la escena completa en el buffer de Allegro
 */
void VistaAllegro::Dibujar() {
    al_clear_to_color(al_map_rgb(10, 10, 25));

    switch (pantalla) {
        case PANTALLA_MENU:
            Titulo(150, "FASE ORBITAL");
            Escribir(ANCHO / 2, 280, "1. Nueva partida", ALLEGRO_ALIGN_CENTRE);
            Escribir(ANCHO / 2, 315, "2. Estadisticas", ALLEGRO_ALIGN_CENTRE);
            Escribir(ANCHO / 2, 350, "3. Ranking", ALLEGRO_ALIGN_CENTRE);
            Escribir(ANCHO / 2, 385, "4. Salir", ALLEGRO_ALIGN_CENTRE);
            break;

        case PANTALLA_REGISTRO:
            Titulo(80, "REGISTRO DE JUGADORES");
            Escribir(300, 170, "Alias: " + alias + "_");
            Escribir(300, 200, std::string("Tipo: ") + (aliasEsBot ? "BOT" : "HUMANO") + "   (TAB para cambiar)");
            Escribir(300, 240, mensaje);
            for (int i = 0; i < sesion.GetTotalJugadores(); i++) {
                const Jugador* j = sesion.GetJugador(i);
                Escribir(300, 290 + i * 28.0f, std::to_string(i + 1) + ". " + j->alias + (j->esBot ? " (bot)" : ""));
            }
            Escribir(ANCHO / 2, 640, "ENTER agrega  |  ENTER con alias vacio empieza  |  ESC vuelve", ALLEGRO_ALIGN_CENTRE);
            break;

        case PANTALLA_TURNO: {
            const Jugador* j = sesion.GetJugadorEnTurno();
            Titulo(200, "Oleada " + std::to_string(sesion.GetOleadaActual()));
            Titulo(260, "Turno de " + j->alias);
            Escribir(ANCHO / 2, 310, "Semilla: " + std::to_string(sesion.GetSemilla()), ALLEGRO_ALIGN_CENTRE);
            Escribir(ANCHO / 2, 340, sesion.EsSentidoHorario() ? "Ronda en sentido horario" : "Ronda en sentido antihorario", ALLEGRO_ALIGN_CENTRE);
            Escribir(ANCHO / 2, 410, "Presione ENTER", ALLEGRO_ALIGN_CENTRE);
            break;
        }

        case PANTALLA_JUEGO:
            DibujarJuego();
            break;

        case PANTALLA_PAUSA:
            DibujarJuego();
            al_draw_filled_rectangle(0, 0, ANCHO, ALTO, al_map_rgba(0, 0, 0, 160));
            Titulo(280, "PAUSA");
            Escribir(ANCHO / 2, 340, "P para seguir  |  Q para salir al menu", ALLEGRO_ALIGN_CENTRE);
            break;

        case PANTALLA_RESUMEN: {
            const ResumenOleada& r = sesion.GetUltimoResumen();
            Titulo(150, r.eliminado ? r.alias + " fue eliminado" : r.alias + " supero la oleada " + std::to_string(r.oleada));
            Escribir(380, 240, "Bajas: " + std::to_string(r.bajas));
            Escribir(380, 270, "Puntos de la oleada: " + std::to_string(r.puntosOleada));
            Escribir(380, 300, "Dano recibido: " + std::to_string(r.danoRecibido));
            Escribir(380, 330, "Vida del nucleo: " + std::to_string(r.vidaNucleo));
            Escribir(380, 360, "Puntaje total: " + std::to_string(r.puntajeTotal));
            Escribir(ANCHO / 2, 450, "Presione ENTER", ALLEGRO_ALIGN_CENTRE);
            break;
        }

        case PANTALLA_FIN:
            Titulo(100, "FIN DE LA PARTIDA");
            for (int p = 1; p <= sesion.GetTotalJugadores(); p++) {
                int i = sesion.GetIndiceEnPosicion(p);
                Escribir(320, 170 + p * 32.0f, std::to_string(p) + ". " + sesion.GetJugador(i)->alias);
                Escribir(620, 170 + p * 32.0f, std::to_string(sesion.GetJugador(i)->puntaje) + " pts", ALLEGRO_ALIGN_RIGHT);
            }
            Escribir(ANCHO / 2, 480, mensaje, ALLEGRO_ALIGN_CENTRE);
            Escribir(ANCHO / 2, 640, "ENTER para volver al menu", ALLEGRO_ALIGN_CENTRE);
            break;

        case PANTALLA_ESTADISTICAS: {
            Titulo(40, "ESTADISTICAS");
            for (int i = 0; i < totalNombres && i < 18; i++) {
                Escribir(60, 110 + i * 28.0f, (i == seleccionado ? "> " : "  ") + nombres[i]);
            }
            ReporteJugador r;
            if (totalNombres > 0 && CalcularReporteJugador(*historial, nombres[seleccionado], r)) {
                std::string datos[8] = {
                    "Partidas: " + std::to_string(r.partidas),
                    "Victorias: " + std::to_string(r.victorias),
                    "Mejor: " + std::to_string(r.mejor) + "   Peor: " + std::to_string(r.peor),
                    "Media: " + std::to_string(static_cast<int>(r.media)),
                    "Mediana: " + std::to_string(static_cast<int>(r.mediana)),
                    "Desviacion: " + std::to_string(static_cast<int>(r.desviacion)),
                    "Precision: " + std::to_string(static_cast<int>(r.precision * 100)) + "%",
                    "Histograma de puntajes:"
                };
                for (int k = 0; k < 8; k++) Escribir(320, 110 + k * 30.0f, datos[k]);

                // Dibujamos las barras del histograma normalizadas segun el maximo conteo
                int maximo = 1;
                for (int k = 0; k < 10; k++) if (r.histograma[k] > maximo) maximo = r.histograma[k];
                for (int k = 0; k < 10; k++) {
                    float alto = 150.0f * r.histograma[k] / maximo;
                    al_draw_filled_rectangle(320 + k * 40.0f, 500 - alto, 350 + k * 40.0f, 500, ColorFase(CIAN));
                }
            }
            else {
                Escribir(320, 110, "No hay historial.csv o no tiene datos");
            }
            Escribir(ANCHO / 2, 640, "Flechas para elegir  |  ESC vuelve", ALLEGRO_ALIGN_CENTRE);
            break;
        }

        case PANTALLA_RANKING: {
            Titulo(60, "RANKING TOP 10");
            int pos = 1;
            for (NodoRanking* n = (ranking ? ranking->ObtenerCabeza() : NULL); n != NULL && pos <= 10; n = n->siguiente) {
                Escribir(340, 120 + pos * 34.0f, std::to_string(pos) + ". " + n->dato.alias);
                Escribir(660, 120 + pos * 34.0f, std::to_string(n->dato.puntaje), ALLEGRO_ALIGN_RIGHT);
                pos++;
            }
            if (pos == 1) Escribir(ANCHO / 2, 200, "No hay registros", ALLEGRO_ALIGN_CENTRE);
            Escribir(ANCHO / 2, 640, "ENTER o ESC para volver", ALLEGRO_ALIGN_CENTRE);
            break;
        }
    }
}

/*
 * Funcion: DibujarJuego
 * Descripcion: Dibuja todos los elementos de la simulacion espacial: orbitas, nucleo con arco de salud, onda de pulso, balas, enemigos y la nave.
 * Entradas: Ninguna
 * Salidas: Dibuja los graficos del juego activo
 */
void VistaAllegro::DibujarJuego() {
    const Motor* motor = sesion.GetMotor();
    const Jugador* j = motor->GetJugador();

    // Dibujamos el anillo de orbita de la nave y el limite donde aparecen los enemigos
    al_draw_circle(CX, CY, 100, Gris(), 1);
    al_draw_circle(CX, CY, 300, al_map_rgb(50, 50, 70), 1);

    // Dibujamos el nucleo central con un arco circular que refleja su porcentaje de vida
    al_draw_filled_circle(CX, CY, 20, al_map_rgb(60, 80, 140));
    float vida = j->vidaNucleo / 100.0f;
    ALLEGRO_COLOR colorVida = vida > 0.5f ? al_map_rgb(80, 220, 120) : al_map_rgb(240, 70, 70);
    if (vida > 0) al_draw_arc(CX, CY, 27, -PI_V / 2, 2 * PI_V * vida, colorVida, 4);

    // Si el pulso EMP esta expandiendose, dibujamos su onda de choque circular
    if (motor->EsPulsoActivo()) {
        al_draw_circle(CX, CY, static_cast<float>(motor->GetRDeOndaPulso()), Blanco(), 3);
    }

    // Recorremos la lista enlazada simple de balas para dibujarlas en pantalla
    for (NodoBala* b = motor->GetListaBalas().ObtenerCabeza(); b != NULL; b = b->siguiente) {
        al_draw_filled_circle(CX + static_cast<float>(b->bala.cord_x), CY - static_cast<float>(b->bala.cord_y), 3, ColorFase(b->bala.color));
    }

    // Recorremos la lista doblemente enlazada de enemigos activos
    for (NodoEnemigo* n = motor->GetListaEnemigos().Primero(); n != NULL; n = n->siguiente) {
        DibujarEnemigo(n->enemigo);
    }

    // Dibujamos los destellos de explosion de los enemigos destruidos recientemente
    for (int i = 0; i < MAX_DESTELLOS; i++) {
        if (destellos[i].activo) {
            al_draw_circle(destellos[i].x, destellos[i].y, 25.0f - destellos[i].vida * 1.5f, ColorFase(destellos[i].color), 2);
        }
    }

    // Calculamos las coordenadas del triangulo de la nave sobre el radio de orbita 100
    float nx, ny, px, py, ax, ay, bx, by;
    Polar(100, j->anguloNave, nx, ny);
    Polar(114, j->anguloNave, px, py);
    Polar(92, j->anguloNave + 7, ax, ay);
    Polar(92, j->anguloNave - 7, bx, by);
    al_draw_filled_triangle(px, py, ax, ay, bx, by, ColorFase(j->faseColor));
    al_draw_triangle(px, py, ax, ay, bx, by, Blanco(), 1);

    // Dibujamos los datos del turno y los controles en pantalla
    const char* nombresFase[3] = { "Cian", "Magenta", "Ambar" };
    Escribir(20, 20, "Jugador: " + j->alias + (j->esBot ? " (bot)" : ""));
    Escribir(20, 45, "Oleada: " + std::to_string(motor->GetNumeroOleada()));
    Escribir(ANCHO - 20, 20, "Semilla: " + std::to_string(sesion.GetSemilla()), ALLEGRO_ALIGN_RIGHT);
    Escribir(20, 70, "Puntaje: " + std::to_string(j->puntaje));
    Escribir(20, 95, "Multiplicador: x" + std::to_string(motor->GetMultiplicador()));
    Escribir(20, 120, "Nucleo: " + std::to_string(j->vidaNucleo));
    Escribir(20, 145, "Pulso: " + std::to_string(j->cargaPulso) + (j->cargaPulso >= 100 ? "  (ESPACIO)" : ""));
    Escribir(20, 170, std::string("Fase: ") + nombresFase[j->faseColor]);
    Escribir(20, ALTO - 30, "Mouse/flechas girar  |  1 2 3 fase  |  ESPACIO pulso  |  P pausa");
}

/*
 * Funcion: DibujarEnemigo
 * Descripcion: Dibuja la figura geometrica especifica de cada enemigo segun su tipo (circulo, triangulo, rectangulo o rombo).
 * Entradas: e (referencia constante al Enemigo)
 * Salidas: Dibuja la forma geometrica en la pantalla
 */
void VistaAllegro::DibujarEnemigo(const Enemigo& e) {
    float x, y;
    Polar(e.r, e.theta, x, y);
    float r = e.radio * 1.5f;
    ALLEGRO_COLOR c = ColorFase(e.color);

    switch (e.tipo) {
        case DRON:
            al_draw_filled_circle(x, y, r, c);
            break;
        case GIRADOR:
            al_draw_filled_triangle(x, y - r, x - r, y + r, x + r, y + r, c);
            break;
        case DIVISOR:
            al_draw_filled_rectangle(x - r, y - r, x + r, y + r, c);
            break;
        case MINI:
            al_draw_filled_triangle(x, y + r, x - r, y - r, x + r, y - r, c);
            break;
        case TANQUE:
            al_draw_circle(x, y, r, c, 4);
            al_draw_text(fuente, c, x, y - 9, ALLEGRO_ALIGN_CENTRE, std::to_string(e.vida).c_str());
            break;
        case CAMALEON:
            al_draw_filled_triangle(x, y - r, x - r, y, x + r, y, c);
            al_draw_filled_triangle(x, y + r, x - r, y, x + r, y, c);
            break;
    }
}
