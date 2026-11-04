#pragma once
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "SesionJuego.h"
#include "ReporteHistorico.h"

// Version simple de la interfaz: lee el estado del motor, lo dibuja y le manda acciones.

enum Pantalla {
    PANTALLA_MENU,
    PANTALLA_REGISTRO,
    PANTALLA_TURNO,
    PANTALLA_JUEGO,
    PANTALLA_PAUSA,
    PANTALLA_RESUMEN,
    PANTALLA_FIN,
    PANTALLA_ESTADISTICAS,
    PANTALLA_RANKING
};

// circulo que crece donde murio un enemigo
struct Destello {
    float x, y;
    int vida;
    Color color;
    bool activo;
};

const int MAX_DESTELLOS = 40;

class VistaAllegro {
private:
    ALLEGRO_DISPLAY* display;
    ALLEGRO_TIMER* timer;
    ALLEGRO_EVENT_QUEUE* cola;
    ALLEGRO_FONT* fuente;
    ALLEGRO_FONT* fuenteGrande;
    ALLEGRO_SAMPLE* sonidoBaja;
    ALLEGRO_SAMPLE* sonidoError;
    ALLEGRO_SAMPLE* sonidoDano;
    ALLEGRO_SAMPLE* sonidoPulso;
    bool hayAudio;

    SesionJuego sesion;
    Pantalla pantalla;
    bool corriendo;

    // registro
    std::string alias;
    bool aliasEsBot;
    std::string mensaje;

    // controles
    bool izquierda, derecha;
    bool usarMouse;
    double anguloMouse;
    Color fase;
    bool pulso;

    // efectos
    NodoEvento* ultimoEvento;
    Destello destellos[MAX_DESTELLOS];

    // estadisticas
    HistorialPartidas* historial;
    Ranking* ranking;
    std::string nombres[MAX_ALIAS_HISTORIAL];
    int totalNombres;
    int seleccionado;

    bool Cargar(ALLEGRO_FONT*& f, const char* ruta, int tam);
    bool Cargar(ALLEGRO_SAMPLE*& s, const char* ruta);
    void Liberar();

    void Tecla(int tecla);
    void Caracter(int tecla, int c);
    void IrA(Pantalla p);
    void ComenzarTurno();
    void SiguienteTurno();
    void TickJuego();
    void LeerEventos();
    void Sonar(ALLEGRO_SAMPLE* s);
    void CargarHistorial();
    void LiberarHistorial();

    void Dibujar();
    void DibujarJuego();
    void DibujarEnemigo(const Enemigo& e);
    void Escribir(float x, float y, const std::string& s, int alineacion = 0);
    void Titulo(float y, const std::string& s);
    void Polar(double r, double grados, float& x, float& y) const;

public:
    VistaAllegro();
    ~VistaAllegro();
    VistaAllegro(const VistaAllegro&) = delete;
    VistaAllegro& operator=(const VistaAllegro&) = delete;

    bool Inicializar();
    void Ejecutar();
};
