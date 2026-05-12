import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter

# --- Paso 1: Definir los datos extraídos de image_3.png (Imagen 1) ---

frecuencias = [10, 50, 100, 200, 500, 800, 1000, 1500, 2000, 5000, 
               8000, 10000, 50000, 100000, 200000, 400000, 500000, 
               800000, 1000000, 1500000, 2000000]

# Ganancia de bucle abierto (Av)
av = [23.69, 43.69, 45.54, 46.15, 46.46, 46.77, 47.69, 47.08, 47.08, 47.08, 
      47.08, 47.08, 46.15, 45.23, 42.77, 35.38, 31.70, 23.08, 18.77, 11.38, 7.69]

# Ganancia de bucle cerrado (Avf)
avf = [2.96, 7.80, 8.79, 9.09, 9.38, 9.48, 9.48, 9.38, 9.38, 9.38, 
       9.38, 9.38, 9.38, 9.48, 9.38, 9.28, 8.94, 8.69, 8.49, 7.63, 6.50]

# --- Paso 2: Configurar la figura y el diseño basándose en image_4.png (Imagen 2) ---

# Crear la figura y los ejes (2 gráficos lado a lado)
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

# Configurar el color de fondo de la figura (fuera de los gráficos)
fig.patch.set_facecolor('#ecf0f1') # Color gris claro similar a la referencia

# Definir fuentes comunes (SIN pad ni labelpad aquí)
label_font = {'fontsize': 11, 'fontweight': 'bold'}
title_font = {'fontsize': 12, 'fontweight': 'bold'}

# Marcas específicas para el eje X logarítmico como en la referencia
x_ticks = [1, 100, 10000, 1000000, 10000000]
x_formatter = ScalarFormatter(useOffset=False)
x_formatter.set_scientific(False) # Evitar notación científica en el eje X

# --- Paso 3: Gráfico 1: Ganancia a Lazo Abierto ---

# Trazar datos (usar semilogx para eje X logarítmico)
ax1.semilogx(frecuencias, av, color='black', linewidth=1.5)

# Configuración del eje y título (pasando pad y labelpad como argumentos separados)
ax1.set_title('Variación de la Ganancia a Lazo Abierto', fontdict=title_font, pad=15)
ax1.set_xlabel('Frecuencia (Hz.)', fontdict=label_font, labelpad=10)
ax1.set_ylabel('Av', fontdict=label_font, labelpad=10)

# Ajustar límites del eje Y para tus datos.
ax1.set_ylim(0, 60) 
ax1.set_xlim(1, 1e7) # Mismo rango de frecuencia de la referencia

# Configurar ticks y formato del eje X
ax1.set_xticks(x_ticks)
ax1.xaxis.set_major_formatter(x_formatter)
ax1.tick_params(axis='both', which='major', labelsize=10, width=1, length=4)

# Cuadrícula sutil
ax1.grid(True, which='major', axis='x', linestyle='-', linewidth=0.7, color='#7f8c8d')
ax1.grid(True, which='minor', axis='x', linestyle='--', linewidth=0.5, color='#bdc3c7', alpha=0.5)
ax1.set_axisbelow(True) # Cuadrícula detrás de las líneas

# --- Paso 4: Gráfico 2: Ganancia a Lazo Cerrado ---

# Trazar datos
ax2.semilogx(frecuencias, avf, color='black', linewidth=1.5)

# Configuración del eje y título (pasando pad y labelpad como argumentos separados)
ax2.set_title('Variación de la Ganancia a Lazo Cerrado', fontdict=title_font, pad=15)
ax2.set_xlabel('Frecuencia (Hz.)', fontdict=label_font, labelpad=10)
ax2.set_ylabel('Avf', fontdict=label_font, labelpad=10)

# Ajustar límites del eje Y basándose en la imagen de referencia
ax2.set_ylim(0, 14) 
ax2.set_xlim(1, 1e7)

# Mismas configuraciones de ticks y formato del eje X
ax2.set_xticks(x_ticks)
ax2.xaxis.set_major_formatter(x_formatter)
ax2.tick_params(axis='both', which='major', labelsize=10, width=1, length=4)

# Mismas cuadrículas sutiles
ax2.grid(True, which='major', axis='x', linestyle='-', linewidth=0.7, color='#7f8c8d')
ax2.grid(True, which='minor', axis='x', linestyle='--', linewidth=0.5, color='#bdc3c7', alpha=0.5)
ax2.set_axisbelow(True)

# --- Paso 5: Añadir anotaciones de texto como en la referencia ---

# Cuadro de texto para ax1
bbox_props = dict(boxstyle="square,pad=0.5", facecolor="#c8e6c9", edgecolor="none") # Color verde claro

text_ax1 = "Mayor Ganancia\nmenor AB." # AB = Ancho de Banda
ax1.text(0.05, -0.22, text_ax1, transform=ax1.transAxes, fontsize=10, fontweight='bold', 
         bbox=bbox_props, va='top', ha='left')

# Cuadro de texto para ax2
text_ax2 = "Menor Ganancia\nMayor AB."
ax2.text(0.05, -0.22, text_ax2, transform=ax2.transAxes, fontsize=10, fontweight='bold', 
         bbox=bbox_props, va='top', ha='left')

# --- Paso 6: Mostrar el gráfico ---

# Ajustar el diseño para que nada se superponga
plt.tight_layout(pad=3.0)

# Mostrar el gráfico
plt.show()