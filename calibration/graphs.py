import csv
import matplotlib.pyplot as plt
import pandas as pd

# Leer los datos del CSV (sin encabezados)
data = pd.read_csv('data_log_7_2025-02-18_17-53-08.csv', header=None, names=['timestamp', 'state', 'time', 'v', 'i', 'vmeas', 'cmeas'])

# Eliminar la primera fila de datos (no tomar en cuenta el primer dato)
data = data.iloc[1:].reset_index(drop=True)

# Graficar los datos
voltagesM = data['vmeas']*1000
currentsM = data['cmeas']*1000
voltages = data['v']
currents = data['i']
timestamps = data['time']

# Crear una figura con dos subgráficas
fig, ax = plt.subplots(4, 1, figsize=(10, 8))

# Gráfico de voltajes
ax[0].plot(timestamps, voltagesM, label="Voltaje RIGOL (mV)")  # Primera serie de voltaje
ax[0].plot(timestamps, voltages, label="Voltaje IPTC (mV)")  # Segunda serie de voltaje
ax[0].set_title("Voltaje a través del tiempo")
ax[0].set_xlabel("Tiempo (s)")
ax[0].set_ylabel("Voltaje (mV)")
ax[0].grid(True)
ax[0].legend()  # Leyenda para el gráfico de voltaje
ax[0].set_ylim(0, 4200) 

# Gráfico de corrientes
ax[1].plot(timestamps, currentsM, label="Corriente RIGOL (mA)")  # Primera serie de corriente
ax[1].plot(timestamps, currents, label="Corriente IPTC (mA)")  # Segunda serie de corriente
ax[1].set_title("Corriente a través del tiempo")
ax[1].set_xlabel("Tiempo (s)")
ax[1].set_ylabel("Corriente (mA)")
ax[1].grid(True)
ax[1].legend()  # Leyenda para el gráfico de corriente
ax[1].set_ylim(0, 3500) 

# Gráfico de delta V
ax[2].plot(timestamps, voltagesM-voltages, label="Offset en voltajes (mV)")  # Primera serie de corriente
ax[2].set_title("Offset a través del tiempo")
ax[2].set_xlabel("Tiempo (s)")
ax[2].set_ylabel("Voltage (mV)")
ax[2].grid(True)
ax[2].legend()  # Leyenda para el gráfico de corriente
ax[2].set_ylim(80, 120) 

# Gráfico de delta I
ax[3].plot(timestamps, abs(currentsM-currents), label="Offset en corrientes (mA)")  # Primera serie de corriente
ax[3].set_title("Offset a través del tiempo")
ax[3].set_xlabel("Tiempo (s)")
ax[3].set_ylabel("Corriente (mA)")
ax[3].grid(True)
ax[3].legend()  # Leyenda para el gráfico de corriente
ax[3].set_ylim(50, 100) 

# Gráfico de delta I
ax[4].plot(timestamps, abs(currentsM-currents), label="Ofsset currents (mA)")  # Primera serie de corriente
ax[4].plot(timestamps, voltagesM-voltages, label="Offset voltages (mV)")  # Segunda serie de corriente
ax[4].set_xlabel("Tiempo (s)")
ax[4].set_ylabel("Corriente (mA)")
ax[4].grid(True)
ax[4].legend()  # Leyenda para el gráfico de corriente
ax[4].set_ylim(50, 100) 

# Ajustar el formato de las fechas en el eje x para que se vea bien
plt.xticks(rotation=45)

# Ajustar el diseño y dejar espacio en la parte superior para los títulos
plt.tight_layout()

# Ajustar el margen superior para dar más espacio a los títulos
fig.subplots_adjust(top=0.95)  # Ajusta este valor si es necesario

# Guardar la imagen en formato PNG
plt.savefig('Voltage_Calibration_Disc.png', dpi=300)  # Ajusta el nombre y el DPI si es necesario

# Mostrar la gráfica
plt.show()
