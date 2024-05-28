from flask import Flask, render_template, request, redirect, url_for, jsonify
#from flask_ngrok import 
from smbus2 import SMBus, i2c_msg
import struct
import traceback
import pandas as pd
import numpy as np
import time, datetime
import json, os
import matplotlib.pyplot as plt

global readTemp
readTemp = 1

app = Flask(__name__, template_folder='template')
SLAVE_ADDR = 0x0A
bus = SMBus(1)  # Abre el bus I2C
@app.route('/')
def index():
    global readTemp
    action = readAction()
    print(readTemp)
    if (readTemp == 1):
        writeTemperatureToFile(action)
    if (readTemp == 0):
        writeActionToFile(action)
    return render_template('index.html')

def writeAction(pwr):
    try:       
        data = struct.pack('<f', pwr)  # Empaqueta el número como un flotante
        # Crea un objeto de mensaje para escribir 4 bytes en SLAVE_ADDR
        msg = i2c_msg.write(SLAVE_ADDR, data)
        bus.i2c_rdwr(msg)  # Realiza la escritura
        print(f'Mensaje enviado al Arduino: {pwr}')
    except Exception as e:
        print(f'Error al enviar el mensaje al Arduino: {str(e)}') 

@app.route('/run-bomb', methods=['POST'])
def bomb():

    writeAction(1) 
    print('Se encendio la bomba')
    return redirect(url_for('index'))
    #return render_template('index.html')
    
@app.route('/turn-light', methods=['POST'])
def light():

    writeAction(2) 
    print('Se prendio la luz')
    return redirect(url_for('index'))
    #return render_template('index.html')
    
@app.route('/run-vent', methods=['POST'])
def ventilador():
    writeAction(3) 
    print('Se encendio el ventilador')
    return redirect(url_for('index'))
    #return render_template('index.html')

@app.route('/get_action')
def get_action():
    print('Enviando acciones')
    data = pd.read_json('action_data.json')
    df_bomba = data[(data['action']=='Bomba')]
    last_bomba = df_bomba['Date'].iloc[-1] if not df_bomba.empty else None
    df_luz = data[(data['action']=='Luz')]
    last_luz = df_luz['Date'].iloc[-1] if not df_luz.empty else None
    df_vent = data[(data['action']=='Ventilador')]
    last_vent = df_vent['Date'].iloc[-1] if not df_vent.empty else None
    data_js = {
        'bomba': str(last_bomba.strftime('%Y-%m-%d %H:%M:%S')),
        'luz': str(last_luz.strftime('%Y-%m-%d %H:%M:%S')),
        'ventilador': str(last_vent.strftime('%Y-%m-%d %H:%M:%S'))
    }
    response = jsonify(data_js)
    
    return response


def readAction():
    global readTemp
    try:
        
        tipeEventReq = int(bus.read_byte(SLAVE_ADDR))
        
        print("llego ", tipeEventReq)    
        if tipeEventReq == 1:
            action = 'Bomba'
            readTemp = 0
            return action
        elif tipeEventReq == 2:
            readTemp = 0
            action = 'Luz'
            return action
        elif tipeEventReq == 3:
            readTemp = 0
            action = 'Ventilador'
            return action
        else:
            msg = i2c_msg.read(SLAVE_ADDR, 4)
            bus.i2c_rdwr(msg)    
            data = list(msg)
            ba = bytearray(data[0:4])
            temp = struct.unpack('<f', ba)[0]
            print('Received temp: {}'.format(round(temp,2)))
            readTemp = 1
            return temp
    except:
        traceback.print_exc()
        return None

def plot_temp(time_end):
    data = pd.read_json('temperature_data.json')
    data['Date'] = pd.to_datetime(data['Date'], format='%Y-%m-%d %H:%M:%S')
    data = data.sort_values(by='Date', ascending=True).tail(20)
    data['Time'] = data['Date'].dt.time.apply(lambda x: x.strftime('%Y-%m-%d %H:%M:%S'))
    data = data[(data['Date'] <= time_end)]
    plt.plot(data['Time'], data['temperature'])
    plt.xlabel('Time')
    plt.ylabel('Temperatura °C')
    plt.xticks(rotation=35)
    plt.title(f'Last update at: {time_end}')
    plt.savefig('./static/graph_temp.jpg')
    plt.close()
     
#@app.route('/get_graph')
#def get_graph():
#    print('Graficando')
#    timestamp = datetime.datetime.now()
#    date = timestamp.strftime('%Y-%m-%d %H:%M:%S')
#    plot_temp(date)
#    return redirect(url_for('index'))

@app.route('/get_graph2')
def get_graph2():
    print('Graficando')
    timestamp = datetime.datetime.now()
    date = timestamp.strftime('%Y-%m-%d %H:%M:%S')
    plot_temp(date)
    return 'Se actualizo la grafica'

def writeTemperatureToFile(temperature):
    
    timestamp = datetime.datetime.now()
    date = timestamp.strftime('%Y-%m-%d %H:%M:%S')
    # Obtener el tiempo actual en segundos desde el epoch
    data = {"Date": date, "temperature": temperature}
    file_path = "temperature_data.json"
    file_path = os.path.abspath(file_path)
    # Verificar si el archivo existe
    if not os.path.exists(file_path):
        # Si no existe, crear el archivo vacío
        print("creando archivo")
        with open(file_path, "w") as file:
            file.write("[]")  # Escribir una lista vacía

    try:
        # Escribir el objeto JSON en el archivo
        with open(file_path, "r+") as file:
            # Cargar datos existentes
            existing_data = json.load(file)
            # Agregar nuevos datos al final de la lista
            existing_data.append(data)
            # Regresar al inicio del archivo para escribir los nuevos datos
            file.seek(0)
            # Escribir los datos actualizados
            json.dump(existing_data, file)
            # Truncar el archivo si es necesario
            file.truncate()
    except Exception as e:
        print(f"Error al escribir en el archive: {e}")
        
def writeActionToFile(action):
    print("mandando datos a archivo")
    timestamp = datetime.datetime.now()
    date = timestamp.strftime('%Y-%m-%d %H:%M:%S')
    # Obtener el tiempo actual en segundos desde el epoch
    data = {"Date": date, "action": action}
    file_path = "action_data.json"
    file_path = os.path.abspath(file_path)
    # Verificar si el archivo existe
    if not os.path.exists(file_path):
        # Si no existe, crear el archivo vacío
        print("creando archivo")
        with open(file_path, "w") as file:
            file.write("[]")  # Escribir una lista vacía

    try:
        # Escribir el objeto JSON en el archivo
        with open(file_path, "r+") as file:
            # Cargar datos existentes
            existing_data = json.load(file)
            # Agregar nuevos datos al final de la lista
            existing_data.append(data)
            # Regresar al inicio del archivo para escribir los nuevos datos
            file.seek(0)
            # Escribir los datos actualizados
            json.dump(existing_data, file)
            # Truncar el archivo si es necesario
            file.truncate()
    except Exception as e:
        print(f"Error al escribir en el archivoooo: {e}")

        
        
if __name__ == '__main__':
    app.run()