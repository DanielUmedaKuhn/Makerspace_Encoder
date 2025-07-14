const int encoderPin = 8; 
const int ledPin = 6; 
unsigned long lastDetectionTime = 0;
const unsigned long inactivityTimeout = 300000;    //value should be in milliseconds, 300000 = 5min
bool wasActive = false;    //Tracks the  encoder state, true = time since last state change < inactivityTimeout, false = > inactivityTimeout

void setup() {
  Serial.begin(9600);
  pinMode(encoderPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); 
  attachInterrupt(digitalPinToInterrupt(encoderPin), encoderTriggered, CHANGE);  //Launch the interruption every time the encoder state changes
  lastDetectionTime = millis();
}

void loop() {
  if(millis() - lastDetectionTime > inactivityTimeout){    //Maximum waiting threshold exceeded
    if(wasActive){     //Prevents light from turning on when first powering on the system, as well as overwriting when the light is already on
      Serial.println("No rotation detected");
      digitalWrite(ledPin, HIGH);         //Light goes on
      wasActive = false;    //reset
    }
  }
  delay(10);
}

//Interruption
void encoderTriggered(){        
  lastDetectionTime = millis();  //Reset timer
  digitalWrite(ledPin, LOW);    //Light goes off
  wasActive = true;
}
