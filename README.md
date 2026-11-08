# Fase Orbital — Proyecto #1A
**Curso:** Estructuras de Datos IC-2001
**Profesor:** Víctor Garro Abarca  
**Semestre:** II Semestre 2026  
**Tecnológico de Costa Rica (TEC)**  

### Integrantes:
- Luis Carlos Fuentes Calvo
- Marco Esteban Perez Ramos
- Elian Esteban Montero Valverde

---

## Descripción general
Este proyecto es un juego arcade llamado **Fase Orbital**, hecho en C++ con la biblioteca gráfica Allegro 5.

El juego consiste en defender un núcleo central desde una nave en órbita disparando a enemigos de tres colores (Cian, Magenta y Ámbar). 

---

## Requisitos
- **Windows 10 o Windows 11**
- **Visual Studio 2022 o 2026 Community** (con soporte para C++20).
- La biblioteca **Allegro 5 (5.2.x)** y sus dependencias ya vienen incluidas en la solución mediante paquetes NuGet.

---

## Instrucciones de compilación

### Opción 1: Desde Visual Studio (Recomendada)
1. Abrir la solución `FaseOrbital.slnx` (o `FaseOrbital.sln`) que está dentro de la carpeta `FaseOrbital/`.
2. En la barra superior de Visual Studio, asegurarse de que esté seleccionado **Debug** y plataforma **x64**.
3. Compilar el proyecto con `Ctrl + Shift + B` (o en el menú *Compilar -> Compilar solución*).
4. El ejecutable se compila en `FaseOrbital/x64/Debug/FaseOrbital.exe`.

### Opción 2: Por consola con MSBuild
Desde una terminal de Visual Studio (Developer PowerShell):
```powershell
msbuild "FaseOrbital\FaseOrbital.slnx" /p:Configuration=Debug /p:Platform=x64
```

---

## Instrucciones de ejecución

El programa se puede correr en modo gráfico interactivo o en diferentes modos por consola según los argumentos que se le pasen:

### 1. Juego con interfaz gráfica (Allegro 5)
Abre la ventana con menús, registro de jugadores (1 a 6 jugadores humanos o bots), sonido y controles:
```powershell
.\FaseOrbital\x64\Debug\FaseOrbital.exe
```


**Controles del juego:**
- **Flechas izquierda / derecha o mouse:** Girar la nave alrededor del núcleo.
- **Teclas 1, 2, 3:** Cambiar de color (1 = Cian, 2 = Magenta, 3 = Ámbar).
- **Espacio:** Disparar el pulso cuando la carga llegue al 100%.
- **Tecla P o Escape:** Pausar la partida.

---

### 2. Banco de pruebas unitarias (14/14)
Corre las 14 pruebas de las estructuras de datos (listas enlazadas, colas, copias profundas, lista circular, Merge Sort, etc.) y reporta los resultados en consola:
```powershell
.\FaseOrbital\x64\Debug\FaseOrbital.exe --pruebas
```

---

### 3. Modo por lotes con bots
Ejecuta una partida automática sin ventana gráfica a velocidad de procesador, imprime la línea de tiempo por oleada y guarda los resultados en `historial.csv`:
```powershell
.\FaseOrbital\x64\Debug\FaseOrbital.exe --lotes --semilla 42 --oleadas 5 --jugadores BOT1,BOT2,BOT3,BOT4
```

---

### 4. Modo por lotes con archivo de script
Ejecuta la partida leyendo los movimientos y colores de los jugadores desde un archivo de texto (`partida.txt`):
```powershell
.\FaseOrbital\x64\Debug\FaseOrbital.exe --lotes --semilla 7 --script partida.txt --jugadores Ana,Luis
```

---

### 5. Consultar estadísticas históricas
Muestra en consola las estadísticas de los jugadores guardados en el archivo CSV (partidas jugadas, victorias, promedio, mediana calculada con Merge Sort, desviación estándar e histograma):
```powershell
.\FaseOrbital\x64\Debug\FaseOrbital.exe --estadisticas historial.csv
```

---

### 6. Consultar ranking (Top 10)
Muestra la lista de los 10 mejores puntajes históricos aplicando los criterios de desempate oficiales (puntaje, precisión de color y orden alfabético):
```powershell
.\FaseOrbital\x64\Debug\FaseOrbital.exe --ranking historial.csv
```

---

## Archivos adicionales
- `recursos/`: Carpeta con las fuentes TTF y los efectos de sonido WAV que usa el juego.
- `partida.txt`: Archivo de texto con acciones de ejemplo por tick para probar el modo por lotes.
- `historial.csv`: Archivo donde se guardan las partidas terminadas y de donde se leen las estadísticas.
