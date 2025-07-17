const int encoderPin = 8;    //Pin to be connected to the encoder
const int ledPin = 6;
unsigned long lastDetectionTime = 0;
const unsigned long inactivityTimeout = 300000;    //value should be in milliseconds, 300000 = 5min
bool wasActive = false;     //Flag to track the  encoder state, true = time since last state change < inactivityTimeout, false = > inactivityTimeout

void setup() {
  Serial.begin(9600);
  pinMode(encoderPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);  
  attachInterrupt(digitalPinToInterrupt(encoderPin), encoderTriggered, CHANGE);    //Launch the interruption every time the encoder state changes
  lastDetectionTime = millis();    //Starts the timer
}

void loop() {
  if(millis() - lastDetectionTime > inactivityTimeout){    //Inactivity Verification
    if(wasActive){  //Prevents light from turning on when first powering on the system, as well as repeated commands and prints      
      Serial.println("Inactivity detected, check filament.");
      digitalWrite(ledPin, HIGH); 
      wasActive = false;    //Reset the flag
    }
  }
  delay(10);
}
