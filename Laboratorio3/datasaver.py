#!/usr/bin/python3
import serial

# from graficos import print_chart
path = "./Tensiones.csv"
ser = serial.Serial(
    port = '/tmp/ttyS1',\
#    port = 'COM3',\
    baudrate = 9600,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,
    timeout=0\
    )
#ser=serial.Serial('/tmp/ttyS1', 9600)
f = open('Tensiones.csv', 'w+')
f.write("Setpoint,PID_Output,Planta_Output\n")
print("connected to: "+ ser.portstr)

try:
    while True:
        for c in ser.read():
            c=chr(c)
            print(c, end= "")
            f.write(c)
except KeyboardInterrupt:
    print("\nInterrupción de Programa debido al usuario.")

finally:
    ser.close()
    f.close()
    


