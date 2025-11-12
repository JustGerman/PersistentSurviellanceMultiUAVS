import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import numpy as np
import time

# === 1. Leer archivos CSV ===
mapa = pd.read_csv("output/resultado_final_mapa.csv")
tray = pd.read_csv("output/resultado_final_trayectorias.csv")

# === 2. Configurar figura ===
filas = mapa['fila'].max() + 1
cols = mapa['columna'].max() + 1

fig, ax = plt.subplots(figsize=(8, 8))
ax.set_xlim(-0.5, cols - 0.5)
ax.set_ylim(filas - 0.5, -0.5)
ax.set_aspect('equal')
ax.set_title("PSP-UAV: Tick 0", fontsize=14, weight='bold')
ax.set_xlabel("Columna")
ax.set_ylabel("Fila")
ax.grid(True, color='lightgray', linestyle='-', linewidth=0.6, alpha=0.7)

# === 3. Dibujar mapa base ===
for _, row in mapa.iterrows():
    i, j, tipo, urg = int(row.fila), int(row.columna), row.tipo, int(row.urgencia)
    if tipo == 'O':
        ax.scatter(j, i, c='black', marker='s', s=180)
    elif tipo == 'B':
        ax.scatter(j, i, c='gold', marker='*', s=280, edgecolors='black')
    elif urg > 0:
        ax.scatter(j, i, c='red', marker='o', s=urg*8, alpha=0.2)

# === 4. Colores por dron ===
def generar_colores_distintos(n, s=0.8, v=0.9):
    hsv = [(i / n, s, v) for i in range(n)]
    return [mcolors.hsv_to_rgb(c) for c in hsv]

drones = sorted(tray['dron'].unique())
n_drones = len(drones)
colores = generar_colores_distintos(n_drones)

lineas = []
puntos = []
for i, dron_id in enumerate(drones):
    linea, = ax.plot([], [], '-', lw=1.5, color=colores[i], label=f"Dron {dron_id}")
    punto, = ax.plot([], [], 'o', color=colores[i], markersize=6)
    lineas.append(linea)
    puntos.append(punto)

ax.legend(loc='upper right', fontsize=8, ncol=2)

# === 5. Mostrar tick por tick ===
ticks = sorted(tray['tick'].unique())

for frame, t in enumerate(ticks):
    subset = tray[tray['tick'] <= t]
    ax.set_title(f"PSP-UAV: Tick {t}/{ticks[-1]}", fontsize=14, weight='bold')

    for i, dron_id in enumerate(drones):
        sub_dron = subset[subset['dron'] == dron_id]
        lineas[i].set_data(sub_dron['columna'], sub_dron['fila'])
        pos_actual = sub_dron[sub_dron['tick'] == t]
        if not pos_actual.empty:
            puntos[i].set_data(pos_actual['columna'], pos_actual['fila'])

    plt.draw()
    plt.pause(0.1)  # tiempo entre ticks (0.5 segundos)
    # plt.waitforbuttonpress()  # <- usa esto si prefieres avanzar con una tecla

plt.show()
