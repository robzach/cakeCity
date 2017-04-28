/*
   Simple motor control based on incoming pulse width signal

   Motor has dedicated H-bridge driven by pins 5 and 6,
   and receives a 5V control signal on pin 8. The input signal
   is either extend, retract, or stop, and the motor will keep
   going until it gets a different instruction.

   3-24-17 initial version
   3-26-17
       added explicit gaps between control pulses
       restructured input to use interrupt rather than loop function
       added nonblocking pulse receive indicator light
       added proportional motor control
       broke LED functions out
   3-27-17
       rewrote to accept extend, retract, or stop commands rather than position
       added jog pushbuttons
       added stop button, read by interrupt process for greater immediacy
       TO POSSIBLY DO:
           pot feedback governs motor speed through feedback
           (but jog buttons run at non-feedback speed)
           multiple extend-speed commands to be read (fast, med, slow?)
    5-25-17
       changed expected pulse lengths to 10, 20, 30 milliseconds
       changed motor pin assignments to current wiring
       added PWM pin, though not using for speed control yet

   Robert Zacharias
   released to the public domain by the author
*/

// timing variables
const int STOPPULSE = 10000; // two millisecond (2000 uS) pulse is stop command
const int EXTENDPULSE = 4000; // four millisecond (4000 uS) pulse is extend command
const int RETRACTPULSE = 20000; // six millisecond (6000 uS) pulse is retract command
const int PULSEFUZZ = 200; // microseconds of permissible slop in pulse signal
const int INDICATORWAIT = 500; // number of milliseconds LED should stay lit after pulse received

// output pins
const int MOTORCONTROL1 = 4; // if driven positive, will extend linear actuator
const int MOTORCONTROL2 = 6; // if driven positive, will retract linear actuator
const int PWMPIN = 3;
const int EXTENDINGLED = 9;
const int RETRACTINGLED = 10;
const int ATPOSLED = 13;
const int PULSERECEIVELED = 12;
const int ESTOPLED = 8;

// input pins
const int SIGNALPIN = 2; // on an Uno, pins 2 and 3 can have interrupts
const int EXTENDBUTTON = 4; // pushbutton to jog
const int RETRACTBUTTON = 7; // pushbutton to jog
const int ESTOPBUTTON = 3; // the other interrupt;

long timer;
char mode = 's';
int rawPosCommand;

void setup() {

  // output pins
  pinMode(MOTORCONTROL1, OUTPUT);
  pinMode(MOTORCONTROL2, OUTPUT);
  pinMode(PWMPIN, OUTPUT);
  pinMode(EXTENDINGLED, OUTPUT);
  pinMode(RETRACTINGLED, OUTPUT);
  pinMode(ATPOSLED, OUTPUT);
  pinMode(PULSERECEIVELED, OUTPUT);
  pinMode(ESTOPLED, OUTPUT);

  digitalWrite(PWMPIN, HIGH);

  // input pins
  pinMode(SIGNALPIN, INPUT);
  pinMode(EXTENDBUTTON, INPUT_PULLUP);
  pinMode(RETRACTBUTTON, INPUT_PULLUP);
  pinMode(ESTOPBUTTON, INPUT_PULLUP);

  digitalWrite(MOTORCONTROL1, LOW);
  digitalWrite(MOTORCONTROL2, LOW);
  statusLEDs('x'); // turn off indicator LEDs to start

  attachInterrupt(digitalPinToInterrupt(SIGNALPIN), readPulse, RISING);
  attachInterrupt(digitalPinToInterrupt(ESTOPBUTTON), estop, LOW);
}

void loop() {
  driveMotor(mode);
  if (digitalRead(EXTENDBUTTON == LOW)) mode = 'e'; // jog extend
  if (digitalRead(RETRACTBUTTON == LOW)) mode = 'r'; // jog retract
  digitalWrite(PULSERECEIVELED, ((millis() - timer) < INDICATORWAIT));
}

// the interrupt service routine to detect stop button
void estop() {
  // shut off motor and blink eStop light forever (blocking anything else until restart)
  digitalWrite(MOTORCONTROL1, LOW);
  digitalWrite(MOTORCONTROL2, LOW);
  statusLEDs('x');
  while (true) {
    digitalWrite(ESTOPLED, HIGH);
    delay(200);
    digitalWrite(ESTOPLED, LOW);
    delay(200);
  }
}

// the interrupt service routine to detect incoming pulse
void readPulse() {
  rawPosCommand = pulseIn(SIGNALPIN, HIGH);
  if (abs(rawPosCommand - STOPPULSE) < PULSEFUZZ) mode = 's';
  else if (abs(rawPosCommand - EXTENDPULSE) < PULSEFUZZ) mode = 'e';
  else if (abs(rawPosCommand - RETRACTPULSE) < PULSEFUZZ) mode = 'r';
  timer = millis();
}

void driveMotor(int command) {
  switch (command) {
    case 'e':
      digitalWrite(MOTORCONTROL1, HIGH);
      digitalWrite(MOTORCONTROL2, LOW);
      statusLEDs('e');
      break;
    case 'r':
      digitalWrite(MOTORCONTROL1, LOW);
      digitalWrite(MOTORCONTROL2, HIGH);
      statusLEDs('e');
      break;
    case 's':
      digitalWrite(MOTORCONTROL1, LOW);
      digitalWrite(MOTORCONTROL2, LOW);
      statusLEDs('k');
      break;
  }
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
      digitalWrite(ATPOSLED, LOW);
      break;
  }
}
