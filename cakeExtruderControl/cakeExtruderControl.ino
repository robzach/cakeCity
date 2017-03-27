/*
   Simple motor control based on incoming pulse width signal

   Motor has dedicated H-bridge driven by pins 5 and 6,
   and receives a 5V signal on pin 8. A position-driven
   potentiometer on the motor is read on A0.

   Control system should send pulses using transmitting a value of mx+b like so:
      m = gap between pulses, PULSESTEP below
      b = minimum pulse width, MINPULSE below
      x = actual datum, should range from 0 to 1023
      at m = 5 and b = 10, min pulse sent should be 10us and max should be 5125us

   3-24-17 initial version
   3-26-17
       added explicit gaps between control pulses
       restructured input to use interrupt rather than loop function
       added nonblocking pulse receive indicator light
       added proportional motor control
       broke LED functions out

   Robert Zacharias
   released to the public domain by the author
*/


const int MINPULSE = 10; // shortest received pulse in microseconds
const int PULSESTEP = 5; // microseconds of gap between expected pulse widths
const int MOTORCONTROL1 = 5; // if driven positive, will extend linear actuator
const int MOTORCONTROL2 = 6; // if driven positive, will retract linear actuator
const int EXTENDINGLED = 9;
const int RETRACTINGLED = 10;
const int ATPOSLED = 13;
const int PULSERECEIVELED = 12;
const int INDICATORWAIT = 500; // number of milliseconds LED should stay lit after pulse received
const int MOTORPOSITION = A0;
const int SIGNALPIN = 2; // on an Uno, pins 2 and 3 can have interrupts
const int PWMMIN = 40; // smallest PWM signal to transmit; greater than 0 to eliminate deadband
const int PWMMAX = 255; // largest PWM signal to transmit

long timer;

int rawPosCommand;
int posCommand;

void setup() {

  attachInterrupt(digitalPinToInterrupt(SIGNALPIN), readPulse, RISING);

  pinMode(MOTORCONTROL1, OUTPUT);
  pinMode(MOTORCONTROL2, OUTPUT);
  pinMode(EXTENDINGLED, OUTPUT);
  pinMode(RETRACTINGLED, OUTPUT);
  pinMode(ATPOSLED, OUTPUT);
  pinMode(PULSERECEIVELED, OUTPUT);

  pinMode(MOTORPOSITION, INPUT);

  digitalWrite(MOTORCONTROL1, LOW);
  digitalWrite(MOTORCONTROL2, LOW);
  statusLEDs('x'); // turn off indicator LEDs to start
}

void loop() {
  driveMotor(posCommand);
  digitalWrite(PULSERECEIVELED, ((millis() - timer) < INDICATORWAIT));
}

// the interrupt service routine
void readPulse() {
  rawPosCommand = pulseIn(SIGNALPIN, HIGH);
  posCommand = int((rawPosCommand - MINPULSE) / PULSESTEP);
  timer = millis();
}

void driveMotor(int command) {
  int pos = analogRead(MOTORPOSITION);

  // need to extend
  if (pos < command) {
    analogWrite(MOTORCONTROL1, driveSpeed(pos, command));
    digitalWrite(MOTORCONTROL2, LOW);
    statusLEDs('e');
  }

  // need to retract
  else if (pos > command) {
    digitalWrite(MOTORCONTROL1, LOW);
    analogWrite(MOTORCONTROL2, driveSpeed(pos, command));
    statusLEDs('r');
  }

  // at target
  else {
    digitalWrite(MOTORCONTROL1, LOW);
    digitalWrite(MOTORCONTROL2, LOW);
    statusLEDs('k');
  }
}

int driveSpeed(int inPos, int inCommand) {
  int outspeed = map(abs(inPos - inCommand), 0, 1023, PWMMIN, PWMMAX);
  return outspeed;
}

void statusLEDs(char in) {
  switch (in) {
    case 'e':
      digitalWrite(EXTENDINGLED, HIGH);
      digitalWrite(RETRACTINGLED, LOW);
      digitalWrite(ATPOSLED, LOW);
      break;
    case 'r':
      digitalWrite(EXTENDINGLED, LOW);
      digitalWrite(RETRACTINGLED, HIGH);
      digitalWrite(ATPOSLED, LOW);
      break;
    case 'k':
      digitalWrite(EXTENDINGLED, LOW);
      digitalWrite(RETRACTINGLED, LOW);
      digitalWrite(ATPOSLED, HIGH);
      break;
    case 'x':
    default:
      digitalWrite(EXTENDINGLED, LOW);
      digitalWrite(RETRACTINGLED, LOW);
      digitalWrite(ATPOSLED, HIGH);
      break;
  }
}
