# To install TBDeviceMqttClient
# https://thingsboard.io/docs/reference/python-client-sdk/
#

from tb_device_mqtt import TBDeviceMqttClient, TBPublishInfo
# import serial
import time
import socket

HOST = "localhost" 
PORT_Socket = 1881

broker="iot.eie.ucr.ac.cr"
topic="v1/devices/me/telemetry"
password="YpucCrCnikzUd5W0tQnm"
angle_limit = 5


# Connect to ThingsBoard
client = TBDeviceMqttClient(broker, username=password)
client.connect()
try:
    while True:
        # Conectarse al socket del servidor
    
            # Wraping data
        telemetry = { "ruedas_traseras":50,\
                    "ruedas_delanteras":20}
        
        # Sending telemetry and checking the delivery status (QoS = 1 by default)
        result = client.send_telemetry(telemetry)
        
        # get is a blocking call that awaits delivery status  
        success = result.get() == TBPublishInfo.TB_ERR_SUCCESS
        
except KeyboardInterrupt:
    print("\nInterrupción de Programa debido al usuario.")
# Disconnect from ThingsBoard
client.disconnect()