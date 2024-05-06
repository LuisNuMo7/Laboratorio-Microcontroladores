#include <PCD8544.h>
#include <PID_v1_bc.h>

PCD8544 lcd;

// <--------- PID ------------->

// Definicion de las constantes del PID
// Debido a que la libreria del PID tiene una funcion
// que calcula la salida del PID, solo basta con seleccionar
// los valores de las constantes
double Setpoint = 50; // Valor deseado que queremos alcanzar
double Kp = 2;        // Constante proporcional
double Ki = 5;        // Constante integral
double Kd = 1;        // Constante derivativa

// La variable input en este caso es la temperatura de la planta
// La variable output es la salida del PID
double input, output;
PID myPID(&input, &output, &Setpoint, Kp, Ki, Kd, DIRECT);
// <--------- Fin PID --------->

void LCD_state(){
  int switchState = analogRead(A1);
  
  if (switchState > 700){
    lcd.setPower(1);
  } else{
    lcd.setPower(0);
  }
}

void setup() {
  lcd.begin(); // default resolution is 84x48
  Serial.begin(9600); //Se da inicio a la comunicación con el puerto serial
  pinMode(8, OUTPUT);
  // <----- Inicializacion del PID ------->
  myPID.SetMode(AUTOMATIC);
  // <----- Fin Inicializacion del PID --->
}

void loop() {
  LCD_state();
  lcd.clear();

  // // Write some text
  lcd.setCursor(0, 0);
  int value = analogRead(A0);
  // Calcular el valor de control PID
  myPID.Compute();
  float volt = value*(100.0 / 1022.0);
  lcd.print("Temp: ");
  lcd.print(volt);
  lcd.setCursor(0, 1);
  lcd.print("PID: ");
  lcd.print(output);

  lcd.setCursor(0, 3);
  lcd.print(" SP: ");
  lcd.print(Setpoint);
  //lcd.print("  ");
  delay(1000);
  
  // Mostrar resultados
  Serial.print("Input: ");
  Serial.print(input);
  Serial.print(" - Output: ");
  Serial.println(output);

  delay(1000);
}
