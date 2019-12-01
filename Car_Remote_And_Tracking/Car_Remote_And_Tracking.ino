
#include <AFMotor.h>
#include <SoftwareSerial.h>

#define MOTOR_LEFT 4
#define MOTOR_RIGHT 1
#define SENSOR_SENSITIVITY 100  // below this value the sensor has not touched the line (Between 35 - 50) 
#define SENSOR_LEFT 19  // left sensor A0
#define SENSOR_RIGHT 14   // right sensor A5
#define SENSOR_LED A2

// Define motor ports
AF_DCMotor motorRight(1); 
AF_DCMotor motorLeft(4); 

SoftwareSerial mySerial(A3, A4); // RX, TX

enum directions { forward = 1, backward, left, right, stopped };

const int buzzer = 12 ;   // Define o Pino 13 como pino do Buzzer.
const int BTState = 2;    // Define o Pino 2 como o pino de comunicação do Bluetooth.

// Variáveis Úteis
int i = 0;
int j = 0;
int state_rec;
int vSpeed = 200;   // Define velocidade padrão 0 < x < 255.
char state;

boolean lineTracking = false;
int sensorValueRight = 0; // variable to store the value coming from the sensor
int sensorValueLeft = 0;

void moveMotor(int moveDirection){
  motorRight.setSpeed(vSpeed); 
  motorLeft.setSpeed(vSpeed); 
  switch(moveDirection)
  {
    case forward:
          motorLeft.run(FORWARD); 
          motorRight.run(BACKWARD); 
      break;
    case backward:
          motorLeft.run(BACKWARD); 
          motorRight.run(FORWARD);  
      break;
    case right:
          motorLeft.run(FORWARD); 
          motorRight.run(RELEASE);
      break;
    case left:
          motorLeft.run(RELEASE); 
          motorRight.run(BACKWARD);
      break;
    case stopped:
          motorRight.run(RELEASE); 
          motorLeft.run(RELEASE); 
      break;         
  }
}

void moveMotorBySensor(int moveDirection){
 if(mySerial.read() == 'u'){
    lineTracking = false;
    return;
  }
  motorRight.setSpeed(200); 
  motorLeft.setSpeed(200); 
  switch(moveDirection)
  {
    case forward:
          motorRight.setSpeed(120); 
          motorLeft.setSpeed(120); 
          motorLeft.run(BACKWARD); 
          motorRight.run(FORWARD); 
          delay(25);
          motorRight.run(RELEASE); 
          motorLeft.run(RELEASE); 
          delay(30); 
          break;
    case left:
          motorLeft.run(FORWARD); 
          motorRight.run(FORWARD);
          delay(50);
          motorLeft.run(FORWARD); 
          motorRight.run(RELEASE);
          delay(20);
          motorRight.run(RELEASE); 
          motorLeft.run(RELEASE); 
          delay(50);
      break;
    case right:
          motorLeft.run(BACKWARD); 
          motorRight.run(BACKWARD);
          delay(40);
          motorLeft.run(RELEASE); 
          motorRight.run(BACKWARD);
          delay(30);
          motorRight.run(RELEASE); 
          motorLeft.run(RELEASE); 
          delay(50);
      break;
    case stopped:
          motorRight.run(RELEASE); 
          motorLeft.run(RELEASE); 
          delay(100);
      break;                
  }
}

int moveDirection(){
  sensorValueRight = analogRead (SENSOR_LEFT);
  sensorValueLeft = analogRead (SENSOR_RIGHT);

 /* Serial.println("Right:");
  Serial.println(sensorValueRight, DEC);

  Serial.println("Left:");
  Serial.println(sensorValueLeft, DEC); */

  if(sensorValueRight < SENSOR_SENSITIVITY && sensorValueLeft < SENSOR_SENSITIVITY){  // not touch line
     return forward; 
  }else if(sensorValueRight > SENSOR_SENSITIVITY && sensorValueLeft > SENSOR_SENSITIVITY){  // both sensor touch line (must stop)
     return stopped; 
  }else if(sensorValueRight < SENSOR_SENSITIVITY){  // Turn right (left sensor triggered)
     return right; 
  } 

  // Turn left
  return left; 
}


void setup() {
  // Inicializa as portas como entrada e saída.
  pinMode(buzzer, OUTPUT);
  pinMode(BTState, INPUT);
  pinMode(SENSOR_LED, OUTPUT);

  // Define speed for both motor dc
  motorRight.setSpeed(vSpeed); 
  motorLeft.setSpeed(vSpeed); 

  // Inicializa a comunicação serial em 9600 bits.
  Serial.begin(9600);
  mySerial.begin(38400);
}

void loop() {
  // Para o caso quando a conexão com Bluetooth é perdida ou desconectada.
  if (digitalRead(BTState) == LOW && lineTracking == false) {
    state_rec = 'S';
  }

  // Salva os valores da variável 'state'
  if (mySerial.available() > 0) {
    state_rec = mySerial.read();
    state = state_rec;
    //   Serial.println(vSpeed);
  }

  // Altera a velocidade de acordo com valores especificados.
  if (state == '0') {
    vSpeed = 0;
  }
  else if (state == '4') {
    vSpeed = 100;
  }
  else if (state == '6') {
    vSpeed = 155;
  }
  else if (state == '7') {
    vSpeed = 180;
  }
  else if (state == '8') {
    vSpeed = 200;
  }
  else if (state == '9') {
    vSpeed = 230;
  }
  else if (state == 'q') {
    vSpeed = 255;
  }

  if (state != 'S') {
    Serial.print(state);
  }

  if(state == 'U' || state == 'u') { // Adaptado para acionar tracking
    lineTracking = !lineTracking;
  }

  if(lineTracking){
    digitalWrite(SENSOR_LED, HIGH); // TRACKING SENSOR ON
    moveMotorBySensor(moveDirection());
  } else {
    digitalWrite(SENSOR_LED, LOW); // TRACKING SENSOR OFF
    // Se o estado recebido for igual a 'F', o carro se movimenta para frente.
    if (state == 'F') {
      moveMotor(1);
    }
  
    else if (state == 'I') {  // Se o estado recebido for igual a 'I', o carro se movimenta para Frente Esquerda.
      moveMotor(3);
    }
  
    else if (state == 'G') {   // Se o estado recebido for igual a 'G', o carro se movimenta para Frente Direita.
      moveMotor(2);
    }
  
    else if (state == 'B') { // Se o estado recebido for igual a 'B', o carro se movimenta para trás.
      moveMotor(2);
    }
  
    else if (state == 'H') {  // Se o estado recebido for igual a 'H', o carro se movimenta para Trás Esquerda.
  
    }
    
    else if (state == 'J') {  // Se o estado recebido for igual a 'J', o carro se movimenta para Trás Direita.
  
    }
  
    else if (state == 'L') {   // Se o estado recebido for igual a 'L', o carro se movimenta para esquerda.
       moveMotor(4);
    }
    else if (state == 'R') {   // Se o estado recebido for igual a 'R', o carro se movimenta para direita.
       moveMotor(3);
    }
    else if (state == 'S') {   // Se o estado recebido for igual a 'S', o carro permanece parado.
       moveMotor(5);
    }
  
    else if (state == 'V') { // Se o estado recebido for iguala 'V', aciona a buzina.
      if (j == 0) {
        tone(buzzer, 1000);
        j = 1;
      }
      else if (j == 1) {
        noTone(buzzer);
        j = 0;
      }
      state = 'n';
    }

  }

}
