import csv
import matplotlib.pyplot as plt
import pandas as pd
import os

# Hardcoded file path (directory containing the CSV files)
directory_path = './DC_Res'  # Replace with your actual folder path

# Process for each file
for filename in os.listdir(directory_path):
    # Search for csv files
    if filename.endswith('.csv'):
        # Creates the rout to each file
        file_path = os.path.join(directory_path, filename)
        
        # Read the data saved
        data = pd.read_csv(file_path, header=None, names=['timestamp', 'state', 'time', 'v', 'i', 'capacity', 'dt', 'vmeas', 'cmeas'])

        # Extract voltage, current, and timestamp data
        voltages = data['v']
        currents = data['i']
        timestamps = data['time']

        # Find the timestamp corresponding to 10 seconds (0.2C pulse)
        # Adjust to your sampling rate and time intervals
        V1, I1 = None, None
        V2, I2 = None, None

        for idx, time in enumerate(timestamps):
            if time >= 11 and V1 is None:  # Find the first value at or after 10 seconds (0.2C)
                V1 = voltages[idx]
                I1 = currents[idx]
            if time >= 12 and V2 is None:  # Find the first value at or after 11 seconds (1C)
                V2 = voltages[idx]
                I2 = currents[idx]
            
            if V1 is not None and V2 is not None:
                break  # Exit the loop once both measurements have been found

        if V1 is not None and V2 is not None and I1 is not None and I2 is not None:
            # Calculate DC internal resistance using the formula
            R_dc = (V1 - V2) / (I2 - I1)

            # Print the calculated DC resistance for this file
            print(f"DC Resistance for {filename}: {R_dc:.3f} Ohms")
        else:
            print(f"Could not calculate DC Resistance for {filename} (missing data)")


        ## Crear el gráfico para el archivo actual
        #plt.figure(figsize=(10, 6))
        #plt.plot(timestamps, currents, label="Current (mA)")  # Primera serie de corriente
        #plt.plot(timestamps, voltages, label="Voltage (mV)")  # Segunda serie de voltaje
        #plt.title(f"DC_Resistance Test - {filename}")
        #plt.xlabel("Time (s)")
        #plt.grid(True)
        #plt.legend()  # Leyenda para el gráfico de voltaje
        #plt.ylim(0, 4200)
        #plt.xticks(rotation=45)
        #plt.tight_layout()

        ## Guardar el gráfico con un nombre único basado en el archivo
        #plot_filename = f"DC_RES_TEST_{filename.split('.')[0]}.png"
        #plt.savefig(plot_filename, dpi=300)
        #plt.show()
