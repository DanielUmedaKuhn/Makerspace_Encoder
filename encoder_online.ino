#include <WiFi.h>
#include "esp_wpa2.h"
#include <PubSubClient.h>

//Wifi Credentials
const char* ssid = "Network_SSID_here";     //choose one of the univerisity networks, I find HAWHof the most reliable one  
const char* eap_identity = "your_username_here";    //Username for the network
const char* eap_password = "your_password_here";  //Password for the network

//MQTT Configs
const char* mqtt_server = "broker.hivemq.com";
const char* status_topic = "nano-esp32/makerspace/status";   //Name of the topic to be subscribed in
const char* client_id = "arduino-nano-esp32-inactivity-tracker";

//Code variables
const int encoderPin = 8;     //Pin to be connected to the encoder
const int ledPin = 6;         
unsigned long lastDetectionTime = 0;
const unsigned long inactivityTimeout = 300000;   //value should be in milliseconds, 300000 = 5min
bool wasActive = false; //Flag to track the  encoder state, true = time since last state change < inactivityTimeout, false = > inactivityTimeout

//Client Objects
WiFiClient espClient;
PubSubClient client(espClient);

//Interruption
void IRAM_ATTR encoderTriggered() {
  lastDetectionTime = millis();
  digitalWrite(ledPin, LOW);   
  wasActive = true;
}

//Connection to Enterprise Wifi (the chosen university Wifi in this case)
void setup_wifi_enterprise() {
  Serial.println("Configuring Wi-Fi to WPA2-Enterprise...");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_enable();
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)eap_identity, strlen(eap_identity));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)eap_identity, strlen(eap_identity));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)eap_password, strlen(eap_password));
  WiFi.begin(ssid);
  Serial.print("Attempting to connect to network: ");
  Serial.println(ssid);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (++attempts > 30) {
      Serial.println("\nFailed to connect.");
      return;
    }
  }
  Serial.println("\nWiFi connected successfully.");
}

//MQTT Reconnection Function
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting to connect to MQTT Broker...");
    if (client.connect(client_id)) {
      Serial.println("Connected.");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" attempting again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(encoderPin, INPUT_PULLUP); 
  digitalWrite(ledPin, LOW); 
  attachInterrupt(digitalPinToInterrupt(encoderPin), encoderTriggered, CHANGE);  //Launch the interruption every time the encoder state changes

  //Wif and MQTT connection
  setup_wifi_enterprise();
  client.setServer(mqtt_server, 1883);
  lastDetectionTime = millis(); //Starts the timer
  Serial.println("Setup concluded. Monitoring Activity...");
}

void loop() {
  //Maintain connection with Wifi and the MQTT Broker
  if (WiFi.status() != WL_CONNECTED) return;
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

//Inactivity Verification
  if (millis() - lastDetectionTime > inactivityTimeout) {    //Maximum waiting threshold exceeded
    if (wasActive) {      //Prevents light from turning on when first powering on the system, as well as repeated publication      
      Serial.println("Inactivity detected! Turning LED on and publishing to MQTT.");
      digitalWrite(ledPin, HIGH);
      client.publish(status_topic, "Inactivity detected: LED on.");    //Message to be published to MQTT topic

      wasActive = false; //Reset the flag
    }
  }
}
