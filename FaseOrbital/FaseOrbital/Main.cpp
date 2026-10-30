
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include "ListaBalas.h"
#include "Globales.h"

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

// Condición para eliminar balas que salgan de la pantalla (radio > 320 u)
bool BalaFueraDeRango(Bala b) {
    double distSq = b.cord_x * b.cord_x + b.cord_y * b.cord_y;
    return distSq > (320.0 * 320.0);
}

int main() {
    //Inicialización de Allegro
    if (!al_init()) return -1;
    if (!al_init_primitives_addon()) return -1;
    if (!al_install_keyboard()) return -1;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0); // 30 ticks por segundo
    ALLEGRO_DISPLAY* display = al_create_display(PANTALLA_ANCHO, PANTALLA_ALTO);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    // 2. Estado de la Nave y Simulación
    double anguloNave = 270.0; // Inicia a 270° según el PDF
    Color colorNave = CIAN;
    int tick = 0;
    ListaBalas listaBalas; // ¡Tu estructura en acción!

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
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) corriendo = false;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT)  teclas[0] = false;
            if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) teclas[1] = false;
        }
        else if (evento.type == ALLEGRO_EVENT_TIMER) {
            tick++;

            // Giro de la nave (máximo 6° por tick)
            if (teclas[0]) anguloNave -= 6.0; // Sentido horario
            if (teclas[1]) anguloNave += 6.0; // Sentido antihorario

            if (anguloNave >= 360.0) anguloNave -= 360.0;
            if (anguloNave < 0.0)   anguloNave += 360.0;

            // Disparo automático cada 4 ticks (Sección 3.1)
            if (tick % 4 == 0) {
                double rad = anguloNave * (PI / 180.0);
                Bala nuevaBala;
                nuevaBala.cord_x = 100.0 * std::cos(rad);
                nuevaBala.cord_y = 100.0 * std::sin(rad);
                // Velocidad 9 u/tick en dirección radial hacia afuera
                nuevaBala.speed_x = 9.0 * std::cos(rad);
                nuevaBala.speed_y = 9.0 * std::sin(rad);
                nuevaBala.color = colorNave;
                nuevaBala.duenio = 1;

                NodoBala* nodo = new NodoBala;
                nodo->bala = nuevaBala;
                nodo->siguiente = NULL;
                listaBalas.AgregarInicio(nodo);
            }

            // Mover balas
            NodoBala* auxBala = listaBalas.ObtenerCabeza();
            while (auxBala != NULL) {
                auxBala->bala.cord_x += auxBala->bala.speed_x;
                auxBala->bala.cord_y += auxBala->bala.speed_y;
                auxBala = auxBala->siguiente;
            }

            // Eliminar balas que superen r = 320 con tu método de doble puntero
            listaBalas.EliminarSi(BalaFueraDeRango);

            redibujar = true;
        }

        // 4. Dibujado (solo cuando la cola de eventos está vacía)
        if (redibujar && al_is_event_queue_empty(queue)) {
            redibujar = false;
            al_clear_to_color(al_map_rgb(10, 10, 15)); // Fondo oscuro espacial

            // A) Dibujar el Núcleo (Radio 20)
            al_draw_filled_circle(CENTRO_X, CENTRO_Y, 20.0f, al_map_rgb(200, 200, 255));
            al_draw_circle(CENTRO_X, CENTRO_Y, 20.0f, al_map_rgb(255, 255, 255), 2.0f);

            // B) Dibujar el Anillo de Órbita (Radio 100)
            al_draw_circle(CENTRO_X, CENTRO_Y, 100.0f, al_map_rgb(60, 60, 80), 1.5f);

            // C) Dibujar las Balas activas desde ListaBalas
            NodoBala* bActual = listaBalas.ObtenerCabeza();
            while (bActual != NULL) {
                float bx = CENTRO_X + (float)bActual->bala.cord_x;
                float by = CENTRO_Y - (float)bActual->bala.cord_y; // Invertimos Y para pantalla
                al_draw_filled_circle(bx, by, 3.5f, ObtenerColorAllegro(bActual->bala.color));
                bActual = bActual->siguiente;
            }

            // D) Dibujar la Nave (Radio 100 en la órbita)
            double radNave = anguloNave * (PI / 180.0);
            float naveX = CENTRO_X + (float)(100.0 * std::cos(radNave));
            float naveY = CENTRO_Y - (float)(100.0 * std::sin(radNave));

            al_draw_filled_circle(naveX, naveY, 8.0f, ObtenerColorAllegro(colorNave));
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