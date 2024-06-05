#include <Servo.h>
#include <Keypad.h>
#include <ArduinoMqttClient.h>

#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA)
  #include <WiFi.h>
#elif defined(ARDUINO_PORTENTA_C33)
  #include <WiFiC3.h>
#elif defined(ARDUINO_UNOR4_WIFI)
  #include <WiFiS3.h>
#endif

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "iot.eie.ucr.ac.cr";
int        port     = 1883;
const char topic[]  = "v1/devices/me/telemetry";
Servo servoAcelerador;
Servo servoDireccional;

int pos = 0;
String client_instruccion = String("Avanza");
int instruccion_number = 0;
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] ={
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {2,3,4,5};
byte colPins[COLS] = {8,7,6};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS,COLS);
void setup() {
  //////////////////////////////////////
  //      Inicia conexion MQTT      //
  //////////////////////////////////////
  //Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }

    // attempt to connect to WiFi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
      // failed, retry
      Serial.print(".");
      delay(5000);
    }

    Serial.println("You're connected to the network");
    Serial.println();

    // You can provide a unique client ID, if not set the library uses Arduino-millis()
    // Each client must have a unique client ID
    // mqttClient.setId("clientId");

    // You can provide a username and password for authentication
    // mqttClient.setUsernamePassword("username", "password");

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(broker);

    if (!mqttClient.connect(broker, port)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());

      while (1);
    }

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();

    Serial.print("Subscribing to topic: ");
    Serial.println(topic);
    Serial.println();

    // subscribe to a topic
    mqttClient.subscribe(topic);

    // topics can be unsubscribed using:
    // mqttClient.unsubscribe(topic);

    Serial.print("Waiting for messages on topic: ");
    Serial.println(topic);
    Serial.println();

  //////////////////////////////////////
  //      Finaliza conexion MQTT      //
  //////////////////////////////////////

  //////////////////////////////////////
  // Configuracion Servomotores       //
  //////////////////////////////////////
    servoAcelerador.attach(9);
    servoDireccional.attach(10);

  if (
    client_instruccion.compareTo("Avanza")==0  ||
    client_instruccion.compareTo("Adelante") ==0  ||
    client_instruccion.compareTo("Go")==0         ||
    client_instruccion.compareTo("Forward")==0  
    ){
    instruccion_number = 1;
    }else if (
      client_instruccion.compareTo("Retrocede")==0  ||
      client_instruccion.compareTo("Atras")==0      ||
      client_instruccion.compareTo("Back")==0       ||
      client_instruccion.compareTo("Backward")==0  
      ) 
      {
        instruccion_number = 2;
      }
}

void loop() {
  //////////////////////////////////////
  //       Inicia Mensaje MQTT        //
  //////////////////////////////////////
    int messageSize = mqttClient.parseMessage();
    if (messageSize) {
      // we received a message, print out the topic and contents
      Serial.print("Received a message with topic '");
      Serial.print(mqttClient.messageTopic());
      Serial.print("', length ");
      Serial.print(messageSize);
      Serial.println(" bytes:");

      // use the Stream interface to print the contents
      while (mqttClient.available()) {
        Serial.print((char)mqttClient.read());
      }
      Serial.println();

      Serial.println();
    }
  //////////////////////////////////////
  //       Finaliza Mensaje MQTT        //
  //////////////////////////////////////


  //////////////////////////////////////
  //  Inicia movimiento Servomotoes   //
  //////////////////////////////////////
    for(pos = -180; pos <= 180; pos++) {
      switch(key){
        case '1':
              servoAcelerador.write(pos);
              servoDireccional.write(-pos);
          break;

        case '2':
            servoAcelerador.write(pos);
          break;

        case '3':
              servoAcelerador.write(pos);
              servoDireccional.write(pos);
          break;

        case '4': 
              servoDireccional.write(-pos);
          break;

        case '5':// No hace nada
        break;

        case '6':
              servoDireccional.write(pos);
          break;

        case '7':
              servoAcelerador.write(-pos);
              servoDireccional.write(-pos);
          break;

        case '8':
            servoAcelerador.write(-pos);
          break;

        case '9':
            servoAcelerador.write(-pos);
            servoDireccional.write(pos);
      
      }
    }
  //////////////////////////////////////
  //  Finaliza movimiento Servomotoes //
  //////////////////////////////////////
  

  char key = keypad.getKey();
  if(key != NO_KEY){
    Serial.println(key);
  }
}
