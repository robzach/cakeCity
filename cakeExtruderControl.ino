/*
 * Simple motor control based on incoming pulse width signal
 * 
 * Motor has dedicated H-bridge driven by pins 5 and 6,
 * and receives a 5V signal on pin 8. A position-driven
 * potentiometer on the motor is read on A0.
 * 
 * 3-24-17
 * 
 * Robert Zacharias
 * released to the public domain by the author
 */


const int MINPULSE = 10; // shortest received pulse in microseconds
const int PULSERANGE = 1024; // number of steps between min and max pulse signals
const int MOTORCONTROL1 = 5; // if driven positive, will extend linear actuator
const int MOTORCONTROL2 = 6; // if driven positive, will retract linear actuator
const int EXTENDINGLED = 9;
const int RETRACTINGLED = 10;
const int ATPOSLED = 13;
const int PULSERECEIVELED = 12;
const int MOTORPOSITION = A0;
const int SIGNALPIN = 8;

long timer;

int posCommand;

void setup() {
  pinMode(MOTORCONTROL1, OUTPUT);
  pinMode(MOTORCONTROL2, OUTPUT);
  pinMode(EXTENDINGLED, OUTPUT);
  pinMode(RETRACTINGLED, OUTPUT);
  pinMode(ATPOSLED, OUTPUT);
  pinMode(PULSERECEIVELED, OUTPUT);
 
  pinMode(MOTORPOSITION, INPUT);
  pinMode(SIGNALPIN, INPUT);

  digitalWrite(MOTORCONTROL1, LOW);
  digitalWrite(MOTORCONTROL2, LOW);
  digitalWrite(EXTENDINGLED, LOW);
  digitalWrite(RETRACTINGLED, LOW);
  digitalWrite(ATPOSLED, LOW);
}

void loop() {
  int rawPosCommand = pulseIn(SIGNALPIN, HIGH);
  if (rawPosCommand) timer = millis(); // half second nonblocking blink if signal received
  if ((millis() - timer) < 500) digitalWrite(PULSERECEIVELED,HIGH);
  else digitalWrite(PULSERECEIVELED, LOW);
  posCommand = int((rawPosCommand - MINPULSE) * (1024.0 / PULSERANGE));
  driveMotor(posCommand);
}

void driveMotor(int command) {
  int pos = analogRead(MOTORPOSITION);
  if (pos > command) {
    digitalWrite(MOTORCONTROL1, HIGH);
    digitalWrite(MOTORCONTROL2, LOW);
    digitalWrite(ATPOSLED, LOW);
    digitalWrite(EXTENDINGLED, HIGH);
  }
  else if (pos < command) {
    digitalWrite(MOTORCONTROL1, LOW);
    digitalWrite(MOTORCONTROL2, HIGH);
    digitalWrite(ATPOSLED, LOW);
    digitalWrite(RETRACTINGLED, HIGH);
  }
  else {
    digitalWrite(MOTORCONTROL1, LOW);
    digitalWrite(MOTORCONTROL2, LOW);
    digitalWrite(EXTENDINGLED, LOW);
    digitalWrite(RETRACTINGLED, LOW);
    digitalWrite(ATPOSLED, HIGH);
  }
}

void pulseReceive(){
  digitalWrite(PULSERECEIVELED, HIGH);
  delay(500);
  digitalWrite(PULSERECEIVELED, LOW);
}

