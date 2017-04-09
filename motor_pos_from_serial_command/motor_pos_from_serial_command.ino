/* 
 *  Attempting to drive the linear actuator to a position command sent via serial monitor
 *  using bang-bang feedback, without any proportional control
 *  
 *  As of this writing, I didn't get it to work properly and there's a smell of burnt electronics
 *  so I'm going to call it a night since I'm not sure what I did wrong
 *  
 *  4-9-17
 */


const int MOTORPINA = 2;
const int MOTORPINB = 4;
const int PWMPIN = 3;
const int POSREADPIN = A0;

int posCommand = 0;

void setup() {
  Serial.begin(9600);
  pinMode(MOTORPINA, OUTPUT);
  pinMode(MOTORPINB, OUTPUT);
  pinMode(PWMPIN, OUTPUT);

  // just leave the PWM all the way up
  analogWrite(PWMPIN, 255);
}

void loop() {
//  Serial.println(posCommand);
//  delay(500);

motorMove();

}

void motorMove(){
//  int pos = map(analogRead(POSREADPIN),0,847,0,1023);
  int pos = analogRead(POSREADPIN);
  if(posCommand > pos){
    digitalWrite(MOTORPINA, HIGH);
    digitalWrite(MOTORPINB, LOW);
  }
  else if (posCommand < pos){
    digitalWrite(MOTORPINA, LOW);
    digitalWrite(MOTORPINB, HIGH);
  }
  else{
    digitalWrite(MOTORPINA, LOW);
    digitalWrite(MOTORPINB, LOW);
  }
}

void serialEvent() {
  while (Serial.available()) {
    posCommand = Serial.parseInt();
    Serial.print("posCommand = ");
    Serial.print(posCommand);
    Serial.print("\tPOSREADPIN value = ");
    Serial.println(analogRead(POSREADPIN));
  }
}


