# Proyecto PSP-UAV (Persistent Surveillance Problem with UAVs)

Este proyecto implementa una metaheurística **Tabu Search** para resolver el problema de **vigilancia persistente con drones (UAVs)** sobre una grilla.  
Cada dron debe cubrir celdas con distintas urgencias, partiendo desde alguna base, buscando minimizar la urgencia acumulada total del mapa durante una ventana de tiempo \(T\).

---

## Estructura del proyecto

```
.
├── .venv/                   # Entorno virtual (Python)
├── instancias/              # Archivos de entrada con las grillas del problema
│   └── PSP-UAV_01_a.txt
├── output/                  # Archivos exportados (mapa.csv, trayectorias.csv, etc.)
├── main.cpp                 # Código principal en C++
├── Makefile                 # Script de compilación y ejecución
├── plot_resultado.py        # Script para visualizar trayectorias
└── README.md
```

---

## Compilación y ejecución

Asegúrate de estar en el directorio raíz del proyecto.

### Compilar el ejecutable

```bash
make
```

Esto compila todos los archivos `.cpp` del proyecto y genera el ejecutable:

```
PSP-UAV
```

---

### Ejecutar un ejemplo rápido

```bash
make run
```

Este comando compila (si es necesario) y ejecuta un ejemplo pequeño por defecto:

```bash
./PSP-UAV instancias/PSP-UAV_01_a.txt 3 100 1000 50
```

donde:
- `3` → número de drones (`k`)
- `100` → ticks o duración de la misión (`T`)
- `1000` → iteraciones máximas del Tabu Search (`iterMax`)
- `50` → tamaño de la lista tabú (`tabuTenencia`)

Durante la ejecución se mostrará:
- La información de la instancia y los parámetros.
- La mejor asignación de drones a bases.
- El costo (urgencia acumulada total).
- Las trayectorias y movimientos de cada dron.

Los resultados se exportan automáticamente a la carpeta `output/`:
```
output/mapa.csv
output/trayectorias.csv
```

---

### Limpiar archivos generados

```bash
make clean
```

Elimina los binarios y archivos `.o` generados durante la compilación.

---

## Visualización de trayectorias (Python)

El script `plot_resultado.py` genera una animación con las trayectorias de los drones exportadas en los archivos CSV.

### 1. Crear entorno virtual (una sola vez)

```bash
python3 -m venv .venv
source .venv/bin/activate  # En Windows: .venv\Scripts\activate
```

### 2. Instalar dependencias necesarias

```bash
pip install matplotlib pandas pillow
```

*(Se usa Pillow como alternativa a ffmpeg para exportar animaciones.)*

### 3. Ejecutar el script de visualización

```bash
python plot_resultado.py
```

Esto generará un video o animación del tipo:

```
trayectorias_animadas.mp4
```

donde se observa el movimiento de cada dron sobre la grilla durante los \(T\) ticks de tiempo.

---

## Resumen rápido de comandos

| Acción                     | Comando                      |
|-----------------------------|------------------------------|
| Compilar el programa        | `make`                      |
| Ejecutar ejemplo de prueba  | `make run`                  |
| Limpiar binarios y objetos  | `make clean`                |
| Activar entorno virtual     | `source .venv/bin/activate` |
| Instalar dependencias Python| `pip install matplotlib pandas pillow` |
| Ejecutar visualización      | `python plot_resultado.py`  |
