
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include "Motor.h"
#include "Globales.h"
#include "Oleada.h"
#include "BitacoraEventos.h"
#include "RondaJugadores.h"

const int PANTALLA_ANCHO = 800;
const int PANTALLA_ALTO = 600;
const float CENTRO_X = 400.0f;
const float CENTRO_Y = 300.0f;
const float PI = 3.14159265359f;

// Función para obtener el color de Allegro según nuestro enum Color
ALLEGRO_COLOR ObtenerColorAllegro(Color c) {
    switch (c) {
        case CIAN:    return al_map_rgb(0, 255, 255);
        case MAGENTA: return al_map_rgb(255, 0, 255);
        case AMBAR:   return al_map_rgb(255, 191, 0);
        default:      return al_map_rgb(255, 255, 255);
    }
}

int main() {
    //Inicialización de Allegro
    if (!al_init()) return -1;
    if (!al_init_primitives_addon()) return -1;
    if (!al_install_keyboard()) return -1;
    if (!al_install_mouse()) return -1;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0); // 30 ticks por segundo
    ALLEGRO_DISPLAY* display = al_create_display(PANTALLA_ANCHO, PANTALLA_ALTO);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    // inicializacion de jugador y estructuras
    Jugador jugadorPrueba;
    jugadorPrueba.alias = "Prueba";
    jugadorPrueba.esBot = false;
    jugadorPrueba.vidaNucleo = 100;
    jugadorPrueba.puntaje = 0;
    jugadorPrueba.combo = 0;
    jugadorPrueba.cargaPulso = 0;
    jugadorPrueba.faseColor = CIAN;
    jugadorPrueba.anguloNave = 270.0;

    uint32_t semillaPartida = 42; // semilla de prueba
    int numeroOleada = 1;
    ColaSpawn colaOleada;
    GenerarOleada(semillaPartida, numeroOleada, colaOleada);
    BitacoraEventos bitacora;
    Motor motor;
    motor.IniciarTurno(&jugadorPrueba, numeroOleada, colaOleada, &bitacora);

    // 2. Estado de la Nave y Simulación
    double anguloNave = 270.0; // Inicia a 270° según el PDF
    Color colorNave = CIAN;
    bool pedirPulso = false;

    bool teclas[2] = { false, false }; // [0] = Izquierda, [1] = Derecha
    bool corriendo = true;
    bool redibujar = true;

    al_start_timer(timer);

    // 3. Bucle Principal del Juego
    while (corriendo) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(queue, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            corriendo = false;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT)  teclas[0] = true;
            if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) teclas[1] = true;
            if (evento.keyboard.keycode == ALLEGRO_KEY_1) colorNave = CIAN;
            if (evento.keyboard.keycode == ALLEGRO_KEY_2) colorNave = MAGENTA;
            if (evento.keyboard.keycode == ALLEGRO_KEY_3) colorNave = AMBAR;
            if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE)  pedirPulso = true;
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) corriendo = false;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT)  teclas[0] = false;
            if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) teclas[1] = false;
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            // apuntado con el mouse
            float dx = (float)evento.mouse.x - CENTRO_X;
            float dy = CENTRO_Y - (float)evento.mouse.y; // invertido porque Y en pantalla crece hacia abajo
            double rad = atan2(dy, dx);
            double grados = rad * (180.0 / PI);
            if (grados < 0.0) grados += 360.0;
            anguloNave = grados;
        }
        else if (evento.type == ALLEGRO_EVENT_TIMER) {
            // Giro de la nave (máximo 6° por tick)
            if (teclas[0]) anguloNave -= 6.0; // Sentido horario
            if (teclas[1]) anguloNave += 6.0; // Sentido antihorario

            while (anguloNave >= 360.0) anguloNave -= 360.0;
            while (anguloNave < 0.0)   anguloNave += 360.0;

            // accion de tick para el motor
            AccionTick accion;
            accion.anguloObjetivo = anguloNave;
            accion.colorObjetivo = colorNave;
            accion.activarPulso = pedirPulso;
            pedirPulso = false; // se consume en este tick

            // implementacion del motor
            EstadoMotor estado = motor.ActualizarTick(accion);
            if (estado == ESTADO_FIN_OLEADA) {
                numeroOleada++;
                colaOleada = ColaSpawn();
                GenerarOleada(semillaPartida, numeroOleada, colaOleada);
                motor.IniciarTurno(&jugadorPrueba, numeroOleada, colaOleada, &bitacora);
            }

            else if (estado == ESTADO_JUGADOR_ELIMINADO) {
                corriendo = false;
            }

            redibujar = true;
        }

        // 4. Dibujado (solo cuando la cola de eventos está vacía)
        if (redibujar && al_is_event_queue_empty(queue)) {
            redibujar = false;
            al_clear_to_color(al_map_rgb(10, 10, 15)); // Fondo oscuro espacial

            // A) Dibujar el Núcleo (Radio 20)
            float vidaRelativa = (float)jugadorPrueba.vidaNucleo / 100.0f;
            if (vidaRelativa < 0.0f) vidaRelativa = 0.0f;
            al_draw_filled_circle(CENTRO_X, CENTRO_Y, 20.0f, al_map_rgb(50, 50, (int)(255*vidaRelativa)));
            al_draw_circle(CENTRO_X, CENTRO_Y, 20.0f, al_map_rgb(255, 255, 255), 2.0f);

            // B) Dibujar el Anillo de Órbita (Radio 100)
            al_draw_circle(CENTRO_X, CENTRO_Y, 100.0f, al_map_rgb(60, 60, 80), 1.5f);

            // C) Dibujar Onda de Pulso (si está activa en el motor)
            if (motor.EsPulsoActivo()) {
                float rPulso = (float)motor.GetRDeOndaPulso();
                al_draw_circle(CENTRO_X, CENTRO_Y, rPulso, al_map_rgb(255, 255, 255), 3.0f);
            }

            // D) Dibujar las Balas activas desde ListaBalas del Motor
            NodoBala* bActual = motor.GetListaBalas().ObtenerCabeza();
            while (bActual != NULL) {
                float bx = CENTRO_X + (float)bActual->bala.cord_x;
                float by = CENTRO_Y - (float)bActual->bala.cord_y; // Invertimos Y para pantalla
                al_draw_filled_circle(bx, by, 3.5f, ObtenerColorAllegro(bActual->bala.color));
                bActual = bActual->siguiente;
            }

            // E) Dibujar Enemigos activos (desde ListaEnemigos del Motor)
            NodoEnemigo* eActual = motor.GetListaEnemigos().Primero();
            while (eActual != nullptr) {
                const Enemigo& e = eActual->enemigo;
                double rad = e.theta * (PI / 180.0);
                float ex = CENTRO_X + (float)(e.r * std::cos(rad));
                float ey = CENTRO_Y - (float)(e.r * std::sin(rad));
                // Círculo del enemigo con el color de su fase
                al_draw_filled_circle(ex, ey, (float)e.radio, ObtenerColorAllegro(e.color));
                al_draw_circle(ex, ey, (float)e.radio + 1.0f, al_map_rgb(255, 255, 255), 1.0f);
                // Si es Tanque, le dibujamos un aro extra para distinguirlo
                if (e.tipo == TANQUE) {
                    al_draw_circle(ex, ey, (float)e.radio + 3.0f, al_map_rgb(255, 100, 100), 1.5f);
                }
                eActual = eActual->siguiente;
            }

            // F) Dibujar la Nave (Radio 100 en la órbita)
            double radNave = jugadorPrueba.anguloNave * (PI / 180.0);
            float naveX = CENTRO_X + (float)(100.0 * std::cos(radNave));
            float naveY = CENTRO_Y - (float)(100.0 * std::sin(radNave));

            al_draw_filled_circle(naveX, naveY, 8.0f, ObtenerColorAllegro(jugadorPrueba.faseColor));
            al_draw_circle(naveX, naveY, 9.0f, al_map_rgb(255, 255, 255), 1.5f);

            al_flip_display();
        }
    }

    // 5. Limpieza de recursos
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(queue);

    return 0;
}