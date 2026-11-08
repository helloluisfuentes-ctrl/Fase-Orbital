CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -O2 -I./FaseOrbital/FaseOrbital
LDFLAGS ?= -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec -lm

SRC_DIR = FaseOrbital/FaseOrbital
SRCS = $(SRC_DIR)/BitacoraEventos.cpp \
       $(SRC_DIR)/Bot.cpp \
       $(SRC_DIR)/ColaSpawn.cpp \
       $(SRC_DIR)/ControladorLotes.cpp \
       $(SRC_DIR)/EnemigosLogica.cpp \
       $(SRC_DIR)/HistorialPartidas.cpp \
       $(SRC_DIR)/ListaBalas.cpp \
       $(SRC_DIR)/ListaEnemigos.cpp \
       $(SRC_DIR)/Main.cpp \
       $(SRC_DIR)/Motor.cpp \
       $(SRC_DIR)/Oleada.cpp \
       $(SRC_DIR)/Partida.cpp \
       $(SRC_DIR)/Pruebas.cpp \
       $(SRC_DIR)/Ranking.cpp \
       $(SRC_DIR)/ReporteHistorico.cpp \
       $(SRC_DIR)/RondaJugadores.cpp \
       $(SRC_DIR)/SesionJuego.cpp \
       $(SRC_DIR)/VistaAllegro.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = faseorbital

.PHONY: all clean asan

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para compilar con AddressSanitizer (ASan)
asan: CXXFLAGS += -fsanitize=address -g
asan: LDFLAGS += -fsanitize=address
asan: clean $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
