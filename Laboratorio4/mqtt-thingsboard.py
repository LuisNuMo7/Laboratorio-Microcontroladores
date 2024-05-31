from tb_device_mqtt import TBDeviceMqttClient, TBPublishInfo
import serial

broker="iot.eie.ucr.ac.cr"
topic="v1/devices/me/telemetry"
password="YpucCrCnikzUd5W0tQnm"

# ## Serial data preparation
# ser = serial.Serial(
#     port = '/tmp/ttyS1',\
# #    port = 'COM3',\
#     baudrate = 9600,\
#     parity=serial.PARITY_NONE,\
#     stopbits=serial.STOPBITS_ONE,\
#     bytesize=serial.EIGHTBITS,
#     timeout=0\
#     )
# try:
#     while True:
#         for c in ser.read():
#             c=chr(c)
#             print(c, end= "")
# except KeyboardInterrupt:
#     print("\nInterrupción de Programa debido al usuario.")




telemetry = { "valuex":1,"valuey":2,"valuez":3, "enabled": False}

client = TBDeviceMqttClient(broker, username=password)
# Connect to ThingsBoard
client.connect()
# Sending telemetry without checking the delivery status
client.send_telemetry(telemetry) 
# Sending telemetry and checking the delivery status (QoS = 1 by default)
result = client.send_telemetry(telemetry)
# get is a blocking call that awaits delivery status  
success = result.get() == TBPublishInfo.TB_ERR_SUCCESS
# Disconnect from ThingsBoard
client.disconnect()

