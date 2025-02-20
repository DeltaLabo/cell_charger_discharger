import tkinter as tk
from tkinter import ttk
from tkinter import Scrollbar, Canvas
import customtkinter
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import serial
import threading
import struct
from datetime import datetime
import csv
import controller
import pyvisa
from functools import partial

################################################################
#                                                              #
#                    DM3068 MULTIMETER DEF                     #
#                                                              #
################################################################

# Initialize pyvisa
rm = pyvisa.ResourceManager()
res = rm.list_resources()
print("Cantidad de instrumentos encontrados:", len(res))
print(res)

# Find and initialize instruments 
for resource in res:
    if "DM3O252401066" in resource:
        print("Multimetro DM3038 encontrada")
        multimeter = rm.open_resource(resource)
    else:
        print("No se encontró el multimetro")

Multimeter = controller.Meter(multimeter)
Multimeter.remote_sense(True)

################################################################
#                                                              #
#                     FUNCTION DEFINITION                      #
#                                                              #
################################################################

def enviar_hex(hex_string):
    data = bytes.fromhex(hex_string)
    ser.write(data)
    print(f"Enviado: {hex_string}")

def recibir_hex():
    data = ser.read()
    hex_data = data.hex()
    print(f"Recibido: {hex_data}")
    return hex_data

################################################################
#                                                              #
#                      BUTTON DEFINITION                       #
#                                                              #
################################################################

def eventBasic():
    try:
        # Crear una nueva ventana
        config_window = customtkinter.CTkToplevel(root)
        config_window.title("Basic Configuration")
        config_window.geometry("400x550")
        config_window.lift()
        config_window.focus_force()
        config_window.grab_set()

        # Opciones del SelectBox
        options = ["Li-Ion", "Ni-MH"]

        # Valores por defecto para cada tipo de batería
        defaults = {
            "Li-Ion": [4200, 1750, 1750, 3500, 400, 1750, 2500, 1750],
            "Ni-MH": [1200, 1000, 1000, 1300, 1600, 1600, 1000, 1000]
        }

        # Etiqueta y SelectBox
        label_selectbox = customtkinter.CTkLabel(config_window, text="Select Version:")
        label_selectbox.grid(row=0, column=0, padx=10, pady=10)

        selectbox = customtkinter.CTkOptionMenu(config_window, values=options)
        selectbox.grid(row=0, column=1, padx=10, pady=10)

        # Etiquetas y entradas de texto
        labels = ["Const. Voltage (mV):", "Const. Current Char (mA):", "Const. Current Disc (mA):",
                  "Capacity (mAh):", "end-of-charge (mA or mV):", "end-of-precharge (mA or v):",
                  "end-of-discharge (mV):", "end-of-postdischarge (mA or v):"]

        entries = []

        for i, label_text in enumerate(labels):
            label = customtkinter.CTkLabel(config_window, text=label_text)
            label.grid(row=i+1, column=0, padx=10, pady=10)

            entry = customtkinter.CTkEntry(config_window)
            entry.grid(row=i+1, column=1, padx=10, pady=10)
            entries.append(entry)

        # Función para actualizar valores al cambiar la selección
        def update_defaults(choice):
            values = defaults[choice]
            for i, entry in enumerate(entries):
                entry.delete(0, "end")  # Borra el valor actual
                entry.insert(0, values[i])  # Inserta el nuevo valor

        # Configurar la función de cambio
        selectbox.configure(command=update_defaults)

        # Establecer valores iniciales
        update_defaults("Li-Ion")  # Aplica los valores por defecto de Li-Ion al inicio

        # Botón para aplicar la configuración
        button_apply = customtkinter.CTkButton(config_window, text="Apply", command=lambda: apply_basic_config(selectbox, entries, labelSended))
        button_apply.grid(row=len(labels)+1, column=0, columnspan=2, padx=10, pady=20)

        # Display del mensaje enviado
        labelSend = customtkinter.CTkLabel(config_window, text="Message Sended: ")
        labelSend.grid(row=len(labels)+2, column=0, padx=10, pady=10)
        labelSended = customtkinter.CTkLabel(config_window, text="")
        labelSended.grid(row=len(labels)+2, column=1, padx=10, pady=10)

    except Exception as e:
        print("ERROR AL ABRIR LA VENTANA DE BASIC CONFIGURATION:", e)

def eventTest():
    try:
        # Crear una nueva ventana
        test_window = customtkinter.CTkToplevel(root)
        test_window.title("Test Configuration")
        test_window.geometry("600x800")
        test_window.lift()
        test_window.focus_force()
        test_window.grab_set() 

        # Entry boxes
        labels = ["Number of cells:", "Number of states:", "Number of repetitions:",
                  "wait time (s):", "end wait time (s):"]
        
        default = [1, 1, 1, 600, 1200]
        
        entries = []

        # Añadir widgets de configuración

        for i, label_text in enumerate(labels):
            label = customtkinter.CTkLabel(test_window, text=label_text)
            label.grid(row=i+1, column=0, padx=10, pady=10)
            entry = customtkinter.CTkEntry(test_window)
            entry.grid(row=i+1, column=1, padx=10, pady=10)
            entries.append(entry)

        for i, e in enumerate(entries):
            e.insert(0, default[i])

        # SelectBox
        labels_select = ["1st state:", "2st state:", "3st state:", "4st state:", "5st state:",
                  "6st state:", "7st state:", "8st state:", "9st state:", "10st state:"]
        
        options = ["Not defined", "Charge", "Precharge", "Discharge", "Postdischarge", "DC Resistamce"]
        
        selectboxes = []

        for i, select_text in enumerate(labels_select):
            label_selectbox = customtkinter.CTkLabel(test_window, text=select_text)
            label_selectbox.grid(row=i+6, column=0, padx=10, pady=10)
            selectbox = customtkinter.CTkOptionMenu(test_window, values=options)
            selectbox.grid(row=i+6, column=1, padx=10, pady=10)
            selectboxes.append(selectbox)
        
        # Botón para aplicar la configuración
        button_apply = customtkinter.CTkButton(test_window, text="Apply", command=lambda: apply_test_config(selectboxes, entries, labelSended))
        button_apply.grid(row=17, column=0, columnspan=2, padx=10, pady=20)

        # Display del mensaje enviado
        labelSend = customtkinter.CTkLabel(test_window, text="Message Sended: ")
        labelSend.grid(row=18, column=0, padx=10, pady=10)
        labelSended = customtkinter.CTkLabel(test_window, text="")
        labelSended.grid(row=18, column=1, padx=10, pady=10)

    except:
        print("ERROR")

def eventConverter():
    try:
        # Crear una nueva ventana
        config_window = customtkinter.CTkToplevel(root)
        config_window.title("Converter Configuration")
        config_window.geometry("400x550")
        config_window.lift()
        config_window.focus_force()
        config_window.grab_set() 

        # Añadir widgets de configuración
        # Entry boxes
        labels = ["CVKp (xE-6):", "CVKi (xE-6):", "CVKd (xE-3):", 
                  "CCKp Charge (xE-6):", "CCKi Discharge (xE-6):"
                  "CCKp Charge (xE-6):", "CCKi Discharge (xE-6):"]
        
        default = [1800, 500, 20, 
                   3000, 50,
                   6000, 1000]
        
        entries = []

        for i, label_text in enumerate(labels):
            label = customtkinter.CTkLabel(config_window, text=label_text)
            label.grid(row=i+1, column=0, padx=10, pady=10)
            entry = customtkinter.CTkEntry(config_window)
            entry.grid(row=i+1, column=1, padx=10, pady=10)
            entries.append(entry)

        for i, e in enumerate(entries):
            e.insert(0, default[i])

        # Botón para aplicar la configuración
        button_apply = customtkinter.CTkButton(config_window, text="Apply", command=lambda: apply_converter_config(entries, labelSended))
        button_apply.grid(row=len(labels)+1, column=0, columnspan=2, padx=10, pady=20)

        # Display del mensaje enviado
        labelSend = customtkinter.CTkLabel(config_window, text="Message Sended: ")
        labelSend.grid(row=len(labels)+2, column=0, padx=10, pady=10)
        labelSended = customtkinter.CTkLabel(config_window, text="")
        labelSended.grid(row=len(labels)+2, column=1, padx=10, pady=10)

    except:
        print("ERROR AL ABRIR LA VENTANA DE BASI CONFIGURATION")

def eventNextS():
    try:
        enviar_hex(nextS)
        recibido = recibir_hex()
    except:
        print("ERROR")

def eventNextC():
    try:
        enviar_hex(nextC)
        recibido = recibir_hex()
    except:
        print("ERROR")

def eventStart():
    try:
        enviar_hex(start)
        recibido = recibir_hex()
    except:
        print("ERROR")

def eventReset():
    try:
        enviar_hex(reset)
        recibido = recibir_hex()
        clear_data()
        clear_graphs()
        upgrade_labels()

    except:
        print("ERROR")

def showCalibration():
    global calibrating
    calibrating = 1
    selectFrame.grid_forget()
    MainFrameNormal.grid_forget()
    MainFrameCalibration.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

def showNormal():
    global calibrating
    calibrating = 0
    selectFrame.grid_forget()
    MainFrameCalibration.grid_forget()
    MainFrameNormal.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

def eventBack():
    global calibrating
    calibrating = 0
    eventReset()
    MainFrameCalibration.grid_forget()
    MainFrameNormal.grid_forget()
    selectFrame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")


################################################################
#                                                              #
#                       GLOBAL VARIABLES                       #
#                                                              #
################################################################

# DATA CLUSTER
cell_count = 0
repetition_counter = 0
state = 0
elapsed_time = [0]
voltage = [0]
current = [0]
capacity = [0]
duty_cicle = [0]
#MULTIMETER DATA
voltageM = [0]
currentM =[0]
# WORK VARIABLES
current_state = 0
calibrating = 0
# MESSAGES
start = "DD0F0500000577"
nextS = "09"
nextC = "07"
basic_conf = "DD5A031101740ED606D606AC0D9001D606C409D6061E0677"
test_conf = "DD5A051102020205000A0003070000000000000000350077"
reset = "03"
# SAVE PATH
csv_file_path = 'Results/data_log_{state}.csv'

################################################################
#                                                              #
#                       DATA FUNCTIONS                         #
#                                                              #
################################################################

def read_serial_data():
    if ser.in_waiting >= 17:  # Check if there's enough data for a full packet
        packet = ser.read_all()
        print("Datos recividos:")
        print(packet)
        process_serial_data(packet)
    root.after(10, read_serial_data)

def clear_data():
    global cell_count, repetition_counter, state, voltageM, currentM
    cell_count = 0
    repetition_counter = 0
    state = 0
    elapsed_time.clear()
    voltage.clear()
    current.clear()
    capacity.clear()
    duty_cicle.clear()
    voltageM.clear()
    currentM.clear()

def process_serial_data(packet):
    global cell_count, repetition_counter, state, current_state, csv_file_path, calibrating, voltageM, currentM
    header = packet[:1]
    tail = packet[16:17]
    if (header == b'\xdd'):
        # Data into lists 1 byte
        data = struct.unpack('3B', packet[1:4])
        cell_count = data[0]
        repetition_counter = data[1]
        state = data[2]
        if current_state != state:
            current_state = state
            now = datetime.now()
            timestamp = now.strftime("%Y-%m-%d_%H-%M-%S")
            csv_file_path = f"Results/data_log_{state}_{timestamp}.csv"
            clear_data()
            clear_graphs()

        # Data into list 2 bytes
        data = struct.unpack('6H', packet[4:16])
        elapsed_time.append(data[0])
        voltage.append(data[1])
        current.append(data[2])
        capacity.append(data[3])
        duty_cicle.append(data[5])

        # Guardar datos del multimetro
        if (calibrating):
            try:
                vmeas,cmeas = Multimeter.measure_all()
                voltageM.append(round(vmeas*1000, 3))
                currentM.append(round(abs(cmeas*1000), 3))

            except:
                print("WTF")
            
            # Preparar los datos para guardar en CSV
            csv_data = [
                state,
                data[0],
                data[1],
                data[2],
                vmeas,
                cmeas
            ]
        else:
            # Preparar los datos para guardar en CSV
            csv_data = [
                state,
                data[0],
                data[1],
                data[2],
                0,
                0
            ]
        
        # Guardar datos en CSV
        update_graphs()
        upgrade_labels()
        save_to_csv(csv_data)


    elif (header == "01"):
        print("No error")

    else:
        print("ERROR")

def save_to_csv(data):
    # Obtener la fecha y hora actuales
    now = datetime.now()
    timestamp = now.strftime("%Y-%m-%d %H:%M:%S")
    
    # Abrir el archivo CSV en modo anexado
    with open(csv_file_path, 'a', newline='') as file:
        writer = csv.writer(file)
        
        # Escribir los datos en el archivo
        writer.writerow([timestamp] + data)

def str_to_hexStr(text):
    try:
        # Se convierte en int
        hex_str = int(text)
        # Se convierte en hex
        hex_str = hex(hex_str)[2:].upper()

        # Se verifica que sea un numero par de dígitos
        if len(hex_str) % 2 != 0:
            hex_str = '0' + hex_str

        # Se le dan vuelta a los bytes
        bytes_reversed = [hex_str[i:i+2] for i in range(0, len(hex_str), 2)][::-1]
        hex_str_reversed = ''.join(bytes_reversed)
        
        return hex_str_reversed
    
    except ValueError:
        return "Invalid input"
    
def calculate_checksum(command, length, message):
    result = 0
    result += int(command)
    result += int(length, 16)
    for i in range(0, len(message), 2):
        hex_pair = message[i:i+2]  # Extrae el par de caracteres hexadecimales
        byte_value = int(hex_pair, 16)  # Convierte de hex a int
        result += byte_value

    return str_to_hexStr(result & 0xFFFF)
    
def apply_basic_config(selectbox, entries, labelSended):
    send_values = [e.get() for e in entries]

    # default = "DD5A 03 11 01 740E D606 D606 AC0D 9001 D606 C409 D606 1E06 77"
    #            DD5A 03 11 01 6810 D606 D606 AC0D 9001 D606 C409 D606 1406 77

    header = "DD"
    operation = "5A"

    command = "03"
    length = "11"

    message = ""
    # message = header + operation + command + lenght

    version = selectbox.get()
    if (version == "Li-Ion"):
        message = message + str_to_hexStr("1")
    else:
        message = message + str_to_hexStr("2")

    for value in send_values:
        message = message + str_to_hexStr(value)

    message = header + operation + command + length + message + calculate_checksum(command, length, message) + "77"    

    # Se envia el mensaje
    enviar_hex(message)
    recibido = recibir_hex()

    # Display del mensaje enviado
    print(message)
    labelSended.configure(text=message)
    return

def apply_test_config(selectbox, entries, labelSended):
    send_values = [e.get() for e in entries]

    # default = "DD 5A 05 11 02 02 02 0500 0A00 03 07 00 00 00 00 00 00 00 00 3500 77"
    #           "DD 5A 05 11 02 02 02 0500 0A00 03 07 00 00 00 00 00 00 00 00 3500 77"
    #            DD 5A 05 11 01 01 01 5802 B004 03 00 00 00 00 00 00 00 00 00 2A01 77

    header = "DD"
    operation = "5A"

    command = "05"
    length = "11"

    message = ""
    # message = header + operation + command + lenght

    # Se leen los valores de los entries
    for i, value in enumerate(send_values):
        if (i<3):
            print(value)
            message = message + str_to_hexStr(value)
        else:
            print(value)
            lengthBytes = len(str_to_hexStr(value))
            if (lengthBytes < 4):
                message = message + str_to_hexStr(value) + "00"
            else:
                message = message + str_to_hexStr(value)
        
    for box in selectbox:
        state = box.get()
        print(state)
        if (state == "Not defined"):
            state = "0"
        elif (state == "Charge"):
            state = "3"
        elif (state == "Preharge"):
            state = "5"
        elif (state == "Discharge"):
            state = "7"
        elif (state == "Postdischarge"):
            state = "9"
        elif (state == "DC Resistance"):
            state = "11"
        message = message + str_to_hexStr(state)

    check = calculate_checksum(command, length, message)
    if (len(check) < 4):
        check = check + "00"

    message = header + operation + command + length + message + check + "77"    

    # Se envia el mensaje
    enviar_hex(message)
    recibido = recibir_hex()

    # Display del mensaje enviado
    print(message)
    labelSended.configure(text=message)

def apply_converter_config(entries, labelSended):
    send_values = [e.get() for e in entries]

    # default = "DD A507 0007 00 77"
    #           "DD 5A 07 0A 0807 F401 1400 B80B 3200 7017 A502 77"

    header = "DD"
    operation = "5A"

    command = "07"
    length = "0A"

    message = ""
    # message = header + operation + command + lenght

    # Se leen los valores de los entries
    for i, value in enumerate(send_values):
        print(value)
        lengthBytes = len(str_to_hexStr(value))
        if (lengthBytes < 4):
            message = message + str_to_hexStr(value) + "00"
        else:
            message = message + str_to_hexStr(value)

    check = calculate_checksum(command, length, message)
    if (len(check) < 4):
        check = check + "00"

    message = header + operation + command + length + message + check + "77"    

    # Se envia el mensaje
    enviar_hex(message)
    recibido = recibir_hex()

    # Display del mensaje enviado
    print(message)
    labelSended.configure(text=message)
    return


################################################################
#                                                              #
#              FUNCIONES QUE ACTUALIZAN GRÁFICAS               #
#                                                              #
################################################################

# Función para actualizar la gráficas gráficas
def update_graphs():
    global calibrating, voltageM, currentM

    if (calibrating):
        axGraph.clear()
        axGraph.plot(range(len(elapsed_time)), voltage, label="Voltage_IPTC (mV)")
        axGraph.plot(range(len(elapsed_time)), voltageM, label="Voltage_Tester (mV))")
        axGraph.set_xlabel('Time (s)', fontsize=12)
        axGraph.set_title("Voltage Measurements", fontsize=12)
        axGraph.set_ylim(0, None)
        axGraph.set_xlim(0, None)
        axGraph.legend(loc="upper right", fontsize=7)
        axGraph.grid(True)
        canvasVar.draw()

        axGraphReal.clear()
        axGraphReal.plot(range(len(elapsed_time)), current, label="Current_IPTC (mA)")
        axGraphReal.plot(range(len(elapsed_time)), currentM, label="Current_Tester (mA))")
        axGraphReal.set_xlabel('Time (s)', fontsize=12)
        axGraphReal.set_title("Current Measurements", fontsize=12)
        axGraphReal.set_ylim(0, None)
        axGraphReal.set_xlim(0, None)
        axGraphReal.legend(loc="upper right", fontsize=7)
        axGraphReal.grid(True)
        canvasVarReal.draw()

    else:
        axGraphNormal.clear()
        axGraphNormal.plot(range(len(elapsed_time)), current, label="Current (mA)")
        axGraphNormal.plot(range(len(elapsed_time)), voltage, label="Voltage (mV)")
        axGraphNormal.plot(range(len(elapsed_time)), capacity, label="Capacity (mAh)")
        axGraphNormal.set_xlabel('Time (s)', fontsize=12)
        axGraphNormal.set_title("Measurements", fontsize=12)
        axGraphNormal.set_ylim(0, None)
        axGraphNormal.set_xlim(0, None)
        axGraphNormal.legend(loc="upper right", fontsize=7)
        axGraphNormal.grid(True)
        canvasVarNormal.draw()

def clear_graphs():

    axGraph.clear()
    axGraph.set_xlabel('Time (s)', fontsize=12)
    axGraph.set_title("Voltage Measurements", fontsize=12)
    axGraph.set_ylim(0, None)
    axGraph.set_xlim(0, None)
    canvasVar.draw()

    axGraphReal.clear()
    axGraphReal.set_xlabel('Time (s)', fontsize=12)
    axGraphReal.set_title("Current Measurements", fontsize=12)
    axGraphReal.set_xlim(0, None)
    axGraphReal.set_ylim(0, None)
    canvasVarReal.draw()

    axGraphNormal.clear()
    axGraphNormal.set_xlabel('Time (s)', fontsize=12)
    axGraphNormal.set_title("Measurements", fontsize=12)
    axGraphNormal.set_xlim(0, None)
    axGraphNormal.set_ylim(0, None)
    canvasVarNormal.draw()

def upgrade_labels():
    cell_display.configure(state="normal")
    cell_display.delete(0, "end")
    cell_display.insert(0, str(cell_count)) 
    cell_display.configure(state="readonly")

    rep_display.configure(state="normal")
    rep_display.delete(0, "end")
    rep_display.insert(0, str(repetition_counter)) 
    rep_display.configure(state="readonly")

    state_display.configure(state="normal")
    state_display.delete(0, "end")
    state_display.insert(0, str(state)) 
    state_display.configure(state="readonly")

    time_display.configure(state="normal")
    time_display.delete(0, "end")
    time_display.insert(0, str(elapsed_time[-1])) 
    time_display.configure(state="readonly")

    voltage_display.configure(state="normal")
    voltage_display.delete(0, "end")
    voltage_display.insert(0, str(voltage[-1])) 
    voltage_display.configure(state="readonly")

    current_display.configure(state="normal")
    current_display.delete(0, "end")
    current_display.insert(0, str(current[-1])) 
    current_display.configure(state="readonly")

    cap_display.configure(state="normal")
    cap_display.delete(0, "end")
    cap_display.insert(0, str(capacity[-1])) 
    cap_display.configure(state="readonly")

    dc_display.configure(state="normal")
    dc_display.delete(0, "end")
    dc_display.insert(0, str(duty_cicle[-1])) 
    dc_display.configure(state="readonly")

################################################################
#                                                              #
#           SE CREA LA ESTRUCTURA DE LA INTERFAZ               #
#                                                              #
################################################################

# Inicializar ventana
root = customtkinter.CTk()
root.title("Calibration System")
root.geometry("1250x700")

# Initial mode selector frame
selectFrame = customtkinter.CTkFrame(root)
selectFrame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

# Selection buttons
buttonNormal = customtkinter.CTkButton(text="Normal Mode", command=showNormal, master=selectFrame, height=50)
buttonNormal.grid(row = 0, column = 0, padx = 2, pady = 2, sticky="nsew")

buttonCalibration = customtkinter.CTkButton(text="Calibration Mode", command=showCalibration, master=selectFrame, height=50)
buttonCalibration.grid(row = 0, column = 1, padx = 2, pady = 2, sticky="nsew")

# Main frame for calibration
MainFrameCalibration = customtkinter.CTkScrollableFrame(root, label_text="Calibration")
MainFrameNormal = customtkinter.CTkScrollableFrame(root, label_text="Operation")

# No se inicializa para que no se muestre al inicio
MainFrameCalibration.grid_rowconfigure(0, weight=10)
MainFrameCalibration.grid_rowconfigure(1, weight=1)
MainFrameCalibration.grid_columnconfigure(0, weight=1)
MainFrameCalibration.grid_columnconfigure(1, weight=1)

# No se inicializa para que no se muestre al inicio
MainFrameNormal.grid_rowconfigure(0, weight=10)
MainFrameNormal.grid_rowconfigure(1, weight=1)
MainFrameNormal.grid_columnconfigure(0, weight=1)
MainFrameNormal.grid_columnconfigure(1, weight=1)

# Graph frame calibration
GraphFrameCalibration = customtkinter.CTkFrame(master=MainFrameCalibration)
GraphFrameCalibration.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")
GraphFrameCalibration.grid_rowconfigure(0, weight=1)
GraphFrameCalibration.grid_columnconfigure(0, weight=1)
GraphFrameCalibration.grid_columnconfigure(1, weight=1)

# Graph frame Normal
GraphFrameNormal = customtkinter.CTkFrame(master=MainFrameNormal)
GraphFrameNormal.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")
GraphFrameNormal.grid_rowconfigure(0, weight=1)
GraphFrameNormal.grid_columnconfigure(0, weight=1)
GraphFrameNormal.grid_columnconfigure(1, weight=1)

# Button frame Calibration
ButtonFrameCalibration = customtkinter.CTkFrame(master=MainFrameCalibration)
ButtonFrameCalibration.grid(row=1, column=0, columnspan=3, padx=10, pady=10, sticky="nsew")

# Button frame Noramal
ButtonFrameNormal = customtkinter.CTkFrame(master=MainFrameNormal)
ButtonFrameNormal.grid(row=1, column=0, columnspan=3, padx=10, pady=10, sticky="nsew")

# RESIZE
root.grid_rowconfigure(0, weight=1)
root.grid_columnconfigure(0, weight=1)

################################################################
#                                                              #
#     SE CREAN LAS GRÁFICAS Y SE MANDAN A ACTUALIZAR           #
#                                                              #
################################################################

########################## GRAPHS #############################
# Voltaje variable monitoring graph
varGraph = Figure(figsize=(6, 4), dpi=100)
axGraph = varGraph.add_subplot(111)
axGraph.set_xlabel('Time (s)')
canvasVar = FigureCanvasTkAgg(varGraph, master=GraphFrameCalibration)
canvasVar.get_tk_widget().grid(row = 0, column = 0, padx = 2, pady = 2)

# Current variable monitoring graph
varGraphReal = Figure(figsize=(6, 4), dpi=100)
axGraphReal = varGraphReal.add_subplot(111)
axGraphReal.set_xlabel('Time (s)')
canvasVarReal = FigureCanvasTkAgg(varGraphReal, master=GraphFrameCalibration)
canvasVarReal.get_tk_widget().grid(row = 0, column = 1, padx = 2, pady = 2)

# Normal variable monitoring graph
varGraphNoraml = Figure(figsize=(9, 4), dpi=100)
axGraphNormal = varGraphNoraml.add_subplot(111)
axGraphNormal.set_xlabel('Time (s)')
canvasVarNormal = FigureCanvasTkAgg(varGraphNoraml, master=GraphFrameNormal)
canvasVarNormal.get_tk_widget().grid(row = 0, column = 0, padx = 2, pady = 2)

# Configurar la comunicación serial
ser = serial.Serial('COM4', 57600, timeout=1)  # Ajusta el puerto y la velocidad según sea necesario

# Botones Calibration
buttonBasic = customtkinter.CTkButton(text="BASIC CONFIG", command=eventBasic, master=ButtonFrameCalibration, height=50)
buttonBasic.grid(row = 0, column = 0, padx = 2, pady = 2, sticky="nsew")

buttonTest = customtkinter.CTkButton(text="TEST CONFIG", command=eventTest, master=ButtonFrameCalibration, height=50)
buttonTest.grid(row = 0, column = 1, padx = 2, pady = 2, sticky="nsew")

buttonConverter = customtkinter.CTkButton(text="CONVERTER CONFG", command=eventConverter, master=ButtonFrameCalibration, height=50)
buttonConverter.grid(row = 0, column = 2, padx = 2, pady = 2, sticky="nsew")

buttonStart = customtkinter.CTkButton(text="START", command=eventStart, master=ButtonFrameCalibration, height=50)
buttonStart.grid(row = 0, column = 3, padx = 2, pady = 2, sticky="nsew")

buttonNextS = customtkinter.CTkButton(text="NEXT STATE", command=eventNextS, master=ButtonFrameCalibration, height=50)
buttonNextS.grid(row = 0, column = 4, padx = 2, pady = 2, sticky="nsew")

buttonNextC = customtkinter.CTkButton(text="NEXT CELL", command=eventNextC, master=ButtonFrameCalibration, height=50)
buttonNextC.grid(row = 0, column = 5, padx = 2, pady = 2, sticky="nsew")

buttonReset = customtkinter.CTkButton(text="RESET", command=eventReset, master=ButtonFrameCalibration, height=50)
buttonReset.grid(row = 0, column = 6, padx = 2, pady = 2, sticky="nsew")

buttonBack = customtkinter.CTkButton(text="BACK", command=eventBack, master=ButtonFrameCalibration, height=50, fg_color="#E8664A", text_color="black", font=("Arial", 12, "bold"))
buttonBack.grid(row = 0, column = 7, padx = 2, pady = 2, sticky="nsew")

# Botones Normal
buttonBasic = customtkinter.CTkButton(text="BASIC CONFIG", command=eventBasic, master=ButtonFrameNormal, height=50)
buttonBasic.grid(row = 0, column = 0, padx = 2, pady = 2, sticky="nsew")

buttonTest = customtkinter.CTkButton(text="TEST CONFIG", command=eventTest, master=ButtonFrameNormal, height=50)
buttonTest.grid(row = 0, column = 1, padx = 2, pady = 2, sticky="nsew")

buttonConverter = customtkinter.CTkButton(text="CONVERTER CONFG", command=eventConverter, master=ButtonFrameNormal, height=50)
buttonConverter.grid(row = 0, column = 2, padx = 2, pady = 2, sticky="nsew")

buttonStart = customtkinter.CTkButton(text="START", command=eventStart, master=ButtonFrameNormal, height=50)
buttonStart.grid(row = 0, column = 3, padx = 2, pady = 2, sticky="nsew")

buttonNextS = customtkinter.CTkButton(text="NEXT STATE", command=eventNextS, master=ButtonFrameNormal, height=50)
buttonNextS.grid(row = 0, column = 4, padx = 2, pady = 2, sticky="nsew")

buttonNextC = customtkinter.CTkButton(text="NEXT CELL", command=eventNextC, master=ButtonFrameNormal, height=50)
buttonNextC.grid(row = 0, column = 5, padx = 2, pady = 2, sticky="nsew")

buttonReset = customtkinter.CTkButton(text="RESET", command=eventReset, master=ButtonFrameNormal, height=50)
buttonReset.grid(row = 0, column = 6, padx = 2, pady = 2, sticky="nsew")

buttonBack = customtkinter.CTkButton(text="BACK", command=eventBack, master=ButtonFrameNormal, height=50, fg_color="#E8664A", text_color="black", font=("Arial", 12, "bold"))
buttonBack.grid(row = 0, column = 7, padx = 2, pady = 2, sticky="nsew")

# Labels frame Calibration
dataFrameCalibration = customtkinter.CTkFrame(master=MainFrameCalibration)
dataFrameCalibration.grid(row=0, column=2, padx=10, pady=10)

# Labels frame Normal
dataFrameNormal = customtkinter.CTkFrame(master=MainFrameNormal)
dataFrameNormal.grid(row=0, column=1, padx=10, pady=10)

# Labels Calibration
cell_label = customtkinter.CTkLabel(dataFrameCalibration, text="Cell counter: ")
cell_label.grid(row = 1, column = 0, padx = 2, pady = 0, sticky="w")
cell_display = customtkinter.CTkEntry(dataFrameCalibration, justify="left")
cell_display.grid(row=2, column=0, padx=10, pady=2)

rep_label = customtkinter.CTkLabel(dataFrameCalibration, text="Repetition counter: ")
rep_label.grid(row = 3, column = 0, padx = 2, pady = 0, sticky="w")
rep_display = customtkinter.CTkEntry(dataFrameCalibration, justify="left")
rep_display.grid(row=4, column=0, padx=10, pady=2)

state_label = customtkinter.CTkLabel(dataFrameCalibration, text="State: ")
state_label.grid(row = 5, column = 0, padx = 2, pady = 0, sticky="w")
state_display = customtkinter.CTkEntry(dataFrameCalibration, justify="left")
state_display.grid(row=6, column=0, padx=10, pady=2)

time_label = customtkinter.CTkLabel(dataFrameCalibration, text="Elapsed time(s): ")
time_label.grid(row = 7, column = 0, padx = 2, pady = 0, sticky="w")
time_display = customtkinter.CTkEntry(dataFrameCalibration, justify="left")
time_display.grid(row=8, column=0, padx=10, pady=2)

voltage_label = customtkinter.CTkLabel(dataFrameCalibration, text="Voltage: ")
voltage_label.grid(row = 9, column = 0, padx = 2, pady = 0, sticky="w")
voltage_display = customtkinter.CTkEntry(dataFrameCalibration, justify="left")
voltage_display.grid(row=10, column=0, padx=10, pady=2)

current_label = customtkinter.CTkLabel(dataFrameCalibration, text="Current: ")
current_label.grid(row = 11, column = 0, padx = 2, pady = 0, sticky="w")
current_display = customtkinter.CTkEntry(dataFrameCalibration, justify="left")
current_display.grid(row=12, column=0, padx=10, pady=2)

cap_label = customtkinter.CTkLabel(dataFrameCalibration, text="Capacity: ")
cap_label.grid(row = 13, column = 0, padx = 2, pady = 0, sticky="w")
cap_display = customtkinter.CTkEntry(dataFrameCalibration, justify="left")
cap_display.grid(row=14, column=0, padx=10, pady=2)

dc_label = customtkinter.CTkLabel(dataFrameCalibration, text="Duty cicle: ")
dc_label.grid(row = 15, column = 0, padx = 2, pady = 0, sticky="w")
dc_display = customtkinter.CTkEntry(dataFrameCalibration, justify="left")
dc_display.grid(row=16, column=0, padx=10, pady=2)

# Labels Normal
cell_label = customtkinter.CTkLabel(dataFrameNormal, text="Cell counter: ")
cell_label.grid(row = 1, column = 0, padx = 2, pady = 0, sticky="w")
cell_display = customtkinter.CTkEntry(dataFrameNormal, justify="left")
cell_display.grid(row=2, column=0, padx=10, pady=2)

rep_label = customtkinter.CTkLabel(dataFrameNormal, text="Repetition counter: ")
rep_label.grid(row = 3, column = 0, padx = 2, pady = 0, sticky="w")
rep_display = customtkinter.CTkEntry(dataFrameNormal, justify="left")
rep_display.grid(row=4, column=0, padx=10, pady=2)

state_label = customtkinter.CTkLabel(dataFrameNormal, text="State: ")
state_label.grid(row = 5, column = 0, padx = 2, pady = 0, sticky="w")
state_display = customtkinter.CTkEntry(dataFrameNormal, justify="left")
state_display.grid(row=6, column=0, padx=10, pady=2)

time_label = customtkinter.CTkLabel(dataFrameNormal, text="Elapsed time(s): ")
time_label.grid(row = 7, column = 0, padx = 2, pady = 0, sticky="w")
time_display = customtkinter.CTkEntry(dataFrameNormal, justify="left")
time_display.grid(row=8, column=0, padx=10, pady=2)

voltage_label = customtkinter.CTkLabel(dataFrameNormal, text="Voltage: ")
voltage_label.grid(row = 9, column = 0, padx = 2, pady = 0, sticky="w")
voltage_display = customtkinter.CTkEntry(dataFrameNormal, justify="left")
voltage_display.grid(row=10, column=0, padx=10, pady=2)

current_label = customtkinter.CTkLabel(dataFrameNormal, text="Current: ")
current_label.grid(row = 11, column = 0, padx = 2, pady = 0, sticky="w")
current_display = customtkinter.CTkEntry(dataFrameNormal, justify="left")
current_display.grid(row=12, column=0, padx=10, pady=2)

cap_label = customtkinter.CTkLabel(dataFrameNormal, text="Capacity: ")
cap_label.grid(row = 13, column = 0, padx = 2, pady = 0, sticky="w")
cap_display = customtkinter.CTkEntry(dataFrameNormal, justify="left")
cap_display.grid(row=14, column=0, padx=10, pady=2)

dc_label = customtkinter.CTkLabel(dataFrameNormal, text="Duty cicle: ")
dc_label.grid(row = 15, column = 0, padx = 2, pady = 0, sticky="w")
dc_display = customtkinter.CTkEntry(dataFrameNormal, justify="left")
dc_display.grid(row=16, column=0, padx=10, pady=2)

# Se mandan a crear y a limpiar las gráficas
update_graphs()
clear_graphs()

# Se inicializan los valores de los labels
upgrade_labels()

################################################################
#                                                              #
#           SE INICIALIZA LA INTERFAZ GRÁFICA                  #
#                                                              #
################################################################

# Crear y empezar el hilo para leer datos seriales
read_serial_data()

# Ocultar los MainFrames al inicio
MainFrameCalibration.grid_forget()
MainFrameNormal.grid_forget()
root.mainloop()