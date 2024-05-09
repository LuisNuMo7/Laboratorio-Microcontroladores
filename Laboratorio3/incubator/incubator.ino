#include <PCD8544.h>
#include <PID_v1_bc.h>

//Referencias de los headers
// https://github.com/drf5n/Arduino-PID-Library/blob/PID_v1_bc/examples/PID_simulated_heater/PID_simulated_heater.ino

// Parametros de simulacion
#define DELAY_TIME 1

// <----------- Pantalla ----------->
PCD8544 lcd;
#define SWITCH_PIN A1      // Pin al que está conectado el switch
#define LCD_POWER_PIN 3    // Pin al que está conectado el pin de alimentación de la pantalla LCD
// <----------- Luces LED ---------->
#define BLUE_LED_PIN 8  // Pin al que está conectado el LED azul
#define GREEN_LED_PIN 9 // Pin al que está conectado el LED verde
#define RED_LED_PIN 10   // Pin al que está conectado el LED rojo

// <--------- PID ------------->
// Definicion de las constantes del PID
// Debido a que la libreria del PID tiene una funcion
// que calcula la salida del PID, solo basta con seleccionar
// los valores de las constantes
double Kp = 1;        // Constante proporcional
double Ki = 0.3;        // Constante integral
double Kd = 0.3;        // Constante derivativa

float potRead;

// La variable input en este caso es la temperatura de la planta
// La variable output es la salida del PID
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, P_ON_E, DIRECT);
// <--------- Fin PID --------->

// <--------- Funcion para simular el proceso de la incubadora ----->
float simPlanta(float Q){
  float h = 5;
  float Cps = 0.89;
  float area = 1e-4;
  float masa = 10;
  float Tamb = 25;
  static float T = Tamb;
  static uint32_t last = 0;
  uint32_t interval = 100;

  if (millis () - last >= interval){
    last += interval;
    T = T + Q * interval / 1000 / masa / Cps - (T - Tamb) * area * h;
  }
  return T;
}
// <--------- Fin funcion para simular el proceso de la incubadora ----->

void setup() {
  Serial.begin(9600); //Se da inicio a la comunicación con el puerto serial
  
 //<------------- Configuracion Pantalla ---------------->
  pinMode(SWITCH_PIN, INPUT_PULLUP); // Configura el pin del switch como entrada con pull-up
  pinMode(LCD_POWER_PIN, OUTPUT);    // Configura el pin de alimentación de la pantalla LCD como salida
  digitalWrite(LCD_POWER_PIN, LOW);  // Apaga inicialmente la pantalla LCD

  lcd.begin(); // default resolution is 84x48

  // Configuracion de los LED
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  // <----- Inicializacion del PID ------->
  myPID.SetOutputLimits(-100,100); // Limites PID
  myPID.SetMode(AUTOMATIC);
  // <----- Fin Inicializacion del PID --->
}

void loop() {
  // <------------ Encender la pantalla --------->
  // Lee el estado del switch
  int switchState = analogRead(SWITCH_PIN);

  // Si el switch está presionado (el estado es LOW porque se utiliza pull-up)
  // Enciende la pantalla LCD
  if (switchState > 700) {
    lcd.setPower(true); // Enciende la pantalla LCD
  } else {
    lcd.setPower(false); // Apaga la pantalla LCD
  }
  // <-------------------------------------------------------->

  // Calcular el valor de control PID
  Input = simPlanta(Output);
  potRead = analogRead(A0);
  Setpoint =  map(potRead, 0, 1023, 20, 80);

  myPID.Compute();

  // Encender los LEDs correspondientes según el rango de temperatura
  if (Input < 30) {
    digitalWrite(GREEN_LED_PIN, LOW);  // Apagar el LED verde
    digitalWrite(BLUE_LED_PIN, HIGH); // Encender el LED azul
    digitalWrite(RED_LED_PIN, LOW);    // Apagar el LED rojo
  }
    else if (Input >= 30 && Input <= 42) {
    digitalWrite(GREEN_LED_PIN, HIGH); // Encender el LED verde
    digitalWrite(BLUE_LED_PIN, LOW);   // Apagar el LED azul
    digitalWrite(RED_LED_PIN, LOW);    // Apagar el LED rojo
  } else if (Input > 30){
    digitalWrite(GREEN_LED_PIN, LOW);  // Apagar el LED verde
    digitalWrite(BLUE_LED_PIN, LOW);   // Apagar el LED azul
    digitalWrite(RED_LED_PIN, HIGH);   // Encender el LED rojo
  } 
  else {
    digitalWrite(GREEN_LED_PIN, LOW);  // Apagar el LED verde
    digitalWrite(BLUE_LED_PIN, LOW);   // Apagar el LED azul
    digitalWrite(RED_LED_PIN, LOW);   // Encender el LED rojo
  }
  
  lcd.setCursor(0, 1);  
  lcd.print("Temp: ");
  lcd.print(Input);
  
  lcd.setCursor(0, 2);
  lcd.print("PID: ");
  lcd.print(Output);
;
  lcd.setCursor(0, 3);
  lcd.print("SP: ");
  lcd.print(Setpoint);
  
  // Mostrar resultados para pasar a python
  Serial.print(Setpoint);
  Serial.print(",");
  Serial.print(Output);
  Serial.print(",");
  Serial.println(Input);
}