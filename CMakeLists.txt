cmake_minimum_required(VERSION 3.20)
project(FaseOrbital LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Archivos fuente del motor, estructuras de datos, logica y vistas
set(SOURCES
    FaseOrbital/FaseOrbital/BitacoraEventos.cpp
    FaseOrbital/FaseOrbital/Bot.cpp
    FaseOrbital/FaseOrbital/ColaSpawn.cpp
    FaseOrbital/FaseOrbital/ControladorLotes.cpp
    FaseOrbital/FaseOrbital/EnemigosLogica.cpp
    FaseOrbital/FaseOrbital/HistorialPartidas.cpp
    FaseOrbital/FaseOrbital/ListaBalas.cpp
    FaseOrbital/FaseOrbital/ListaEnemigos.cpp
    FaseOrbital/FaseOrbital/Main.cpp
    FaseOrbital/FaseOrbital/Motor.cpp
    FaseOrbital/FaseOrbital/Oleada.cpp
    FaseOrbital/FaseOrbital/Partida.cpp
    FaseOrbital/FaseOrbital/Pruebas.cpp
    FaseOrbital/FaseOrbital/Ranking.cpp
    FaseOrbital/FaseOrbital/ReporteHistorico.cpp
    FaseOrbital/FaseOrbital/RondaJugadores.cpp
    FaseOrbital/FaseOrbital/SesionJuego.cpp
    FaseOrbital/FaseOrbital/VistaAllegro.cpp
)

set(HEADERS
    FaseOrbital/FaseOrbital/BitacoraEventos.h
    FaseOrbital/FaseOrbital/Bot.h
    FaseOrbital/FaseOrbital/ColaSpawn.h
    FaseOrbital/FaseOrbital/ControladorLotes.h
    FaseOrbital/FaseOrbital/EnemigosLogica.h
    FaseOrbital/FaseOrbital/Generador.h
    FaseOrbital/FaseOrbital/Globales.h
    FaseOrbital/FaseOrbital/HistorialPartidas.h
    FaseOrbital/FaseOrbital/ListaBalas.h
    FaseOrbital/FaseOrbital/ListaEnemigos.h
    FaseOrbital/FaseOrbital/Motor.h
    FaseOrbital/FaseOrbital/Oleada.h
    FaseOrbital/FaseOrbital/Partida.h
    FaseOrbital/FaseOrbital/Pruebas.h
    FaseOrbital/FaseOrbital/Ranking.h
    FaseOrbital/FaseOrbital/ReporteHistorico.h
    FaseOrbital/FaseOrbital/RondaJugadores.h
    FaseOrbital/FaseOrbital/SesionJuego.h
    FaseOrbital/FaseOrbital/VistaAllegro.h
)

add_executable(faseorbital ${SOURCES} ${HEADERS})

target_include_directories(faseorbital PRIVATE
    FaseOrbital/FaseOrbital
)

# Busqueda de dependencias de Allegro 5
find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(ALLEGRO allegro-5 allegro_primitives-5 allegro_font-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    if (ALLEGRO_FOUND)
        target_include_directories(faseorbital PRIVATE ${ALLEGRO_INCLUDE_DIRS})
        target_link_libraries(faseorbital PRIVATE ${ALLEGRO_LIBRARIES})
    endif()
endif()

# En caso de compilacion en Linux sin PkgConfig o manual
if (UNIX AND NOT ALLEGRO_FOUND)
    target_link_libraries(faseorbital PRIVATE
        allegro
        allegro_primitives
        allegro_font
        allegro_ttf
        allegro_audio
        allegro_acodec
    )
endif()
