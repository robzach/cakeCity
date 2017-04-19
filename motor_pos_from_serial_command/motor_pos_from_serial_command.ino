/*
    Driving the linear actuator according to a position or speed command sent via serial monitor

    4-19-17
      - simplified motorMoveSpeed to be a simple event-loop stepper
    
    4-17-18
      - added jog pushbuttons
      - added global position constraints
      - added copy of Garth Zeglin's suggestion for a linear state estimator, but not correctly
      - added serial 'e' and 'r' commands for emit and retratct at fixed speed, and 's' for stop
        (these are meant to imitate the style of robot command we expect the device to receive, though
        that command will be received and interpreted via pulse width signals via the ABB digital out)
        they are not really working right yet; they move the motor but not at a slow speed
      later
      - added motor off command for proper speed control

    4-12-17
      - added - and = serial commands to decrement and increment by one step
      - still haven't finished analogRead smoothing function

    4-10-17
    It was likely an electrical fault in the on-board voltage regulator on the Nano, though
    I also had switched the wiring assignments of motor pin A and B (that itself wasn't
    the trouble).
    Today:
      - added proportional (PWM) movement control
      - adding analogRead smoothing function, which isn't done yet


    4-9-17
    As of this writing, I didn't get it to work properly and there's a smell of burnt electronics
    so I'm going to call it a night since I'm not sure what I did wrong

    Robert Zacharias, rz@rzach.me
    released by the author to the public domain
*/


const int MOTORPINA = 4;
const int MOTORPINB = 2;
const int PWMPIN = 3;
const int POSREADPIN = A0;
const int JOGEXTEND = 10;
const int JOGRETRACT = 8;

const int MAXPOS = 850; // found empirically on this device; approx. 8" of extension
const int MINPOS = 10;  // found empirically; fully retracted
const int PWMMIN = 32;
const int PWMMAX = 64;

int mode = 1; // 1 = motorMovePos, 2 = motorMoveSpeed

char speedCommand = 's';  // 's' for 'stop' by default

int posCommand = 0;
int pos = 0;

void setup() {
  Serial.begin(9600);
  pinMode(MOTORPINA, OUTPUT);
  pinMode(MOTORPINB, OUTPUT);
  pinMode(PWMPIN, OUTPUT);
  pinMode(JOGEXTEND, INPUT_PULLUP);
  pinMode(JOGRETRACT, INPUT_PULLUP);
}

void loop() {
  switch (mode) {
    case 1: // motorMovePos
      motorMovePos();
      break;
    case 2: // motorMoveSpeed
      motorMoveSpeed();
      break;
    default:
      motorMovePos();
      break;
  }

  static unsigned long jogTimer = 0;
  int JOGWAIT = 50;   // lower value for faster jog

  if (digitalRead(JOGEXTEND) == LOW && millis() - jogTimer > JOGWAIT) {
    mode = 1;
    posCommand ++;
    posCommand = constrain(posCommand, MINPOS, MAXPOS);
    Serial.print("jog extend command to "); Serial.println(posCommand);
    jogTimer = millis();
  }
  if (digitalRead(JOGRETRACT) == LOW && millis() - jogTimer > JOGWAIT) {
    mode = 1;
    posCommand --;
    posCommand = constrain(posCommand, MINPOS, MAXPOS);
    Serial.print("jog retract command to "); Serial.println(posCommand);
    jogTimer = millis();
  }
}

void motorMovePos() {
  int pos = analogRead(POSREADPIN);
  //  pos += int(0.5 * analogRead(POSREADPIN)); beginning of exponential decay smoothing?
  int PWMsignal = map(abs(pos - posCommand), 0, 900, PWMMIN, PWMMAX);

  if (posCommand > pos) {
    analogWrite(PWMPIN, PWMsignal);
    digitalWrite(MOTORPINA, HIGH);
    digitalWrite(MOTORPINB, LOW);
  }
  else if (posCommand < pos) {
    analogWrite(PWMPIN, PWMsignal);
    digitalWrite(MOTORPINA, LOW);
    digitalWrite(MOTORPINB, HIGH);
  }
  else {
    analogWrite(PWMPIN, 0);
    digitalWrite(MOTORPINA, LOW);
    digitalWrite(MOTORPINB, LOW);
  }
}

void motorMoveSpeed() {

  int travelWait = 50;  // millisecond wait between steps
  static unsigned long lastMoveTime = 0;
  
  switch (speedCommand) {
    case 'e':
      if (millis() - lastMoveTime > travelWait) {
        digitalWrite(MOTORPINA, HIGH);
        digitalWrite(MOTORPINB, LOW);
        lastMoveTime = millis();
      }
      else {
        digitalWrite(MOTORPINA, LOW);
        digitalWrite(MOTORPINB, LOW);
      }
      break;
    case 'r':
      if (millis() - lastMoveTime > travelWait) {
        digitalWrite(MOTORPINA, LOW);
        digitalWrite(MOTORPINB, HIGH);
        lastMoveTime = millis();
      }
      else {
        digitalWrite(MOTORPINA, LOW);
        digitalWrite(MOTORPINB, LOW);
      }
      break;
    case 's':
    default:
      digitalWrite(MOTORPINA, LOW);
      digitalWrite(MOTORPINB, LOW);
  }
}

void serialEvent() {
  while (Serial.available()) {
    if (Serial.peek() == '-') {
      Serial.read();
      mode = 1;
      posCommand --;
    }
    else if (Serial.peek() == '=') {
      Serial.read();
      mode = 1;
      posCommand ++;
    }
    else if (Serial.peek() == 's') {
      Serial.read();
      Serial.println("stopping");
      mode = 2;
      speedCommand = 's';
    }
    else if (Serial.peek() == 'e') {
      Serial.read();
      Serial.println("emitting");
      mode = 2;
      speedCommand = 'e';
    }
    else if (Serial.peek() == 'r') {
      Serial.read();
      Serial.println("retracting");
      mode = 2;
      speedCommand = 'r';
    }
    else {
      posCommand = Serial.parseInt();
      mode = 1;
      Serial.print("posCommand = ");
      Serial.print(posCommand);
      Serial.print("\tPOSREADPIN value = ");
      Serial.println(analogRead(POSREADPIN));
    }
  }
}


