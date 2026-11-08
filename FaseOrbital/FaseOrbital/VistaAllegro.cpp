#include "VistaAllegro.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>
#include <cmath>
#include <ctime>

const int ANCHO = 1000;
const int ALTO = 700;
const float CX = 500;     // el nucleo se dibuja en el centro de la ventana
const float CY = 350;
const double PI_V = 3.14159265358979;
const int OLEADAS = 10;

// posiciones de los enemigos antes de cada tick, para dibujar el destello donde murieron
const int MAX_FOTO = 300;
static int fotoId[MAX_FOTO];
static float fotoX[MAX_FOTO], fotoY[MAX_FOTO];
static Color fotoColor[MAX_FOTO];
static int totalFoto = 0;

static ALLEGRO_COLOR ColorFase(Color c) {
    if (c == CIAN) return al_map_rgb(0, 220, 255);
    if (c == MAGENTA) return al_map_rgb(255, 60, 220);
    return al_map_rgb(255, 180, 0);
}

static ALLEGRO_COLOR Blanco() { return al_map_rgb(230, 230, 240); }
static ALLEGRO_COLOR Gris() { return al_map_rgb(130, 130, 150); }

// ============================== INICIO Y CIERRE ==============================

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

VistaAllegro::~VistaAllegro() {
    Liberar();
}

bool VistaAllegro::Cargar(ALLEGRO_FONT*& f, const char* ruta, int tam) {
    f = al_load_ttf_font(ruta, tam, 0);
    if (f == NULL) std::cout << "Falta el recurso: " << ruta << "\n";
    return f != NULL;
}

bool VistaAllegro::Cargar(ALLEGRO_SAMPLE*& s, const char* ruta) {
    if (!hayAudio) return true;   // sin tarjeta de sonido se juega en silencio
    s = al_load_sample(ruta);
    if (s == NULL) std::cout << "Falta el recurso: " << ruta << "\n";
    return s != NULL;
}

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

void VistaAllegro::Ejecutar() {
    corriendo = true;
    bool redibujar = true;
    al_start_timer(timer);

    while (corriendo) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(cola, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            corriendo = false;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (pantalla == PANTALLA_JUEGO) TickJuego();   // un evento = un tick del motor
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
            double dx = ev.mouse.x - CX;
            double dy = CY - ev.mouse.y;   // en pantalla la y crece hacia abajo
            anguloMouse = atan2(dy, dx) * 180.0 / PI_V;
            if (anguloMouse < 0) anguloMouse += 360;
            usarMouse = true;
        }

        // se dibuja solo cuando no quedan eventos pendientes
        if (redibujar && al_is_event_queue_empty(cola)) {
            redibujar = false;
            Dibujar();
            al_flip_display();
        }
    }
}

// ============================== TECLADO ==============================

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

// menu y registro usan KEY_CHAR. En el registro se escribe el alias: TAB cambia humano/bot, ENTER agrega, ENTER vacio empieza
void VistaAllegro::Caracter(int tecla, int c) {
    // el menu tambien va aqui: si fuera en KEY_DOWN, el '1' llegaria despues como letra del alias
    if (pantalla == PANTALLA_MENU) {
        if (c == '1') {
            sesion.Reiniciar();
            alias = "";
            mensaje = "";
            IrA(PANTALLA_REGISTRO);
        }
        else if (c == '2') IrA(PANTALLA_ESTADISTICAS);
        else if (c == '3') IrA(PANTALLA_RANKING);
        else if (c == '4') corriendo = false;   // no se usa ESC: el ESC de otra pantalla llegaria aqui y cerraria el juego
        return;
    }
    if (pantalla != PANTALLA_REGISTRO) return;

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

void VistaAllegro::IrA(Pantalla p) {
    if (p == PANTALLA_ESTADISTICAS || p == PANTALLA_RANKING) CargarHistorial();
    if (p == PANTALLA_MENU) LiberarHistorial();
    pantalla = p;
}

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

void VistaAllegro::TickJuego() {
    const Motor* motor = sesion.GetMotor();
    const Jugador* j = motor->GetJugador();

    // foto de los enemigos antes del tick
    totalFoto = 0;
    for (NodoEnemigo* n = motor->GetListaEnemigos().Primero(); n != NULL && totalFoto < MAX_FOTO; n = n->siguiente) {
        fotoId[totalFoto] = n->enemigo.id;
        fotoColor[totalFoto] = n->enemigo.color;
        Polar(n->enemigo.r, n->enemigo.theta, fotoX[totalFoto], fotoY[totalFoto]);
        totalFoto++;
    }

    // la accion la decide el bot o el teclado/mouse; el motor limita el giro a 6 grados
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

    EstadoMotor estado = sesion.EjecutarTick(accion);
    LeerEventos();

    // el enemigo que estaba en la foto y ya no esta en la lista, desaparecio: destello
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

    if (estado != ESTADO_EN_JUEGO) {
        sesion.CerrarTurno(estado);
        IrA(PANTALLA_RESUMEN);
    }
}

// recorre solo los eventos nuevos de la bitacora para poner sonidos
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

void VistaAllegro::Sonar(ALLEGRO_SAMPLE* s) {
    if (hayAudio && s != NULL) al_play_sample(s, 0.6f, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
}

// ============================== HISTORIAL ==============================

void VistaAllegro::CargarHistorial() {
    LiberarHistorial();
    historial = new HistorialPartidas();
    ranking = new Ranking();
    historial->CargarCSV("historial.csv");
    totalNombres = ObtenerAliasHistorial(*historial, nombres, MAX_ALIAS_HISTORIAL);
    ConstruirRanking(*historial, *ranking);
    seleccionado = 0;
}

void VistaAllegro::LiberarHistorial() {
    delete historial;
    delete ranking;
    historial = NULL;
    ranking = NULL;
    totalNombres = 0;
}

// ============================== DIBUJO ==============================

void VistaAllegro::Polar(double r, double grados, float& x, float& y) const {
    double rad = grados * PI_V / 180.0;
    x = CX + static_cast<float>(r * cos(rad));
    y = CY - static_cast<float>(r * sin(rad));
}

void VistaAllegro::Escribir(float x, float y, const std::string& s, int alineacion) {
    al_draw_text(fuente, Blanco(), x, y, alineacion, s.c_str());
}

void VistaAllegro::Titulo(float y, const std::string& s) {
    al_draw_text(fuenteGrande, Blanco(), ANCHO / 2, y, ALLEGRO_ALIGN_CENTRE, s.c_str());
}

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

void VistaAllegro::DibujarJuego() {
    const Motor* motor = sesion.GetMotor();
    const Jugador* j = motor->GetJugador();

    // anillo de orbita y borde de aparicion
    al_draw_circle(CX, CY, 100, Gris(), 1);
    al_draw_circle(CX, CY, 300, al_map_rgb(50, 50, 70), 1);

    // nucleo con arco de vida
    al_draw_filled_circle(CX, CY, 20, al_map_rgb(60, 80, 140));
    float vida = j->vidaNucleo / 100.0f;
    ALLEGRO_COLOR colorVida = vida > 0.5f ? al_map_rgb(80, 220, 120) : al_map_rgb(240, 70, 70);
    if (vida > 0) al_draw_arc(CX, CY, 27, -PI_V / 2, 2 * PI_V * vida, colorVida, 4);

    // onda de pulso
    if (motor->EsPulsoActivo()) {
        al_draw_circle(CX, CY, static_cast<float>(motor->GetRDeOndaPulso()), Blanco(), 3);
    }

    // balas
    for (NodoBala* b = motor->GetListaBalas().ObtenerCabeza(); b != NULL; b = b->siguiente) {
        al_draw_filled_circle(CX + static_cast<float>(b->bala.cord_x), CY - static_cast<float>(b->bala.cord_y), 3, ColorFase(b->bala.color));
    }

    // enemigos
    for (NodoEnemigo* n = motor->GetListaEnemigos().Primero(); n != NULL; n = n->siguiente) {
        DibujarEnemigo(n->enemigo);
    }

    // destellos de las bajas
    for (int i = 0; i < MAX_DESTELLOS; i++) {
        if (destellos[i].activo) {
            al_draw_circle(destellos[i].x, destellos[i].y, 25.0f - destellos[i].vida * 1.5f, ColorFase(destellos[i].color), 2);
        }
    }

    // nave: triangulo que apunta hacia afuera
    float nx, ny, px, py, ax, ay, bx, by;
    Polar(100, j->anguloNave, nx, ny);
    Polar(114, j->anguloNave, px, py);
    Polar(92, j->anguloNave + 7, ax, ay);
    Polar(92, j->anguloNave - 7, bx, by);
    al_draw_filled_triangle(px, py, ax, ay, bx, by, ColorFase(j->faseColor));
    al_draw_triangle(px, py, ax, ay, bx, by, Blanco(), 1);

    // datos del turno
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
