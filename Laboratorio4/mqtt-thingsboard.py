# To install TBDeviceMqttClient
# https://thingsboard.io/docs/reference/python-client-sdk/
#

from tb_device_mqtt import TBDeviceMqttClient, TBPublishInfo
# import serial
import time
import socket

HOST = "localhost" 
PORT_Socket = 1716

broker="iot.eie.ucr.ac.cr"
topic="v1/devices/me/telemetry"
password="YpucCrCnikzUd5W0tQnm"
angle_limit = 5
## Serial data preparation
# ser = serial.Serial(
#     port = '/tmp/ttyS1',\
# #    port = 'COM3',\
#     baudrate = 9600,\
#     parity=serial.PARITY_NONE,\
#     stopbits=serial.STOPBITS_ONE,\
#     bytesize=serial.EIGHTBITS,
#     timeout=0\
#     )

# Connect to ThingsBoard
client = TBDeviceMqttClient(broker, username=password)
client.connect()
try:
    while True:
        #Preparing data to send
        h = ""
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Conectarse al socket del servidor
            s.connect((HOST, PORT_Socket))
            # Recibir el mensaje
            data = s.recv(1024)
            # Imprimir el mensaje recibido
            # print("Mensaje recibido:", data.decode("utf-8"))
            lines = data.decode("utf-8").split("\r\n")
        try:
            for line in lines:
                eje_x, eje_y, eje_z, temperatura, bateria, comunicacion = line.split(",")
                eje_x, eje_y, eje_z, temperatura, bateria , comunicacion= float(eje_x), float(eje_y), float(eje_z), float(temperatura), float(bateria), bool(comunicacion)
                ang_deformacion = (eje_x >= angle_limit or eje_y >= angle_limit or eje_z >= angle_limit)
                # Wraping data
                telemetry = { "Eje X":eje_x,\
                            "Eje Y":eje_y,\
                            "Eje Z":eje_z,\
                            "Temperatura":temperatura,\
                            "Bateria":bateria,\
                            "Comunicacion": comunicacion,\
                            "Deformacion angular":ang_deformacion}
            
                # Sending telemetry and checking the delivery status (QoS = 1 by default)
                result = client.send_telemetry(telemetry)
            
                # get is a blocking call that awaits delivery status  
                success = result.get() == TBPublishInfo.TB_ERR_SUCCESS
            
                # print(result.get())
                print(h)
                # time.sleep(5)
        except:
            print("Line empty")        
except KeyboardInterrupt:
    print("\nInterrupción de Programa debido al usuario.")
# Disconnect from ThingsBoard
client.disconnect()