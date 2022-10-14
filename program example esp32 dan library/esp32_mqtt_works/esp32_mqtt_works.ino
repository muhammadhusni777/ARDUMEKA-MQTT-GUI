#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>


// Replace the next variables with your SSID/Password combination
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


// LED Pin
const int led1_pin = 2;
const int led2_pin = 4;
const int led3_pin = 5;
const int pwm_pin = 23; 

// button pin
const int button1_pin = 18;
const int button2_pin = 19;

int button1_state;
int button2_state;
int pwm_value;

int analog;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(led3_pin, OUTPUT);

  pinMode(button1_pin, INPUT_PULLUP);
  pinMode(button2_pin, INPUT_PULLUP);

  
  ledcSetup(0, 1000, 8);
  ledcAttachPin(pwm_pin, 0);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  
  if (String(topic) == "button1_send") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("ON");
      digitalWrite(led1_pin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("OFF");
      digitalWrite(led1_pin, LOW);
    }
  }

  if (String(topic) == "button2_send") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("ON");
      digitalWrite(led2_pin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("OFF");
      digitalWrite(led2_pin, LOW);
    }
  }

  if (String(topic) == "button3_send") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("ON");
      digitalWrite(led3_pin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("OFF");
      digitalWrite(led3_pin, LOW);
    }
  }


  if (String(topic) == "analog_send") {
    pwm_value = messageTemp.toInt();
  
  }


}



void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");

      client.subscribe("button1_send");
      client.subscribe("button2_send");
      client.subscribe("button3_send");
      client.subscribe("analog_send");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  ledcWrite(0, pwm_value);


  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    
    //kirim data 
    analog = analogRead(35);   
    char analog_send[8];
    dtostrf(analog, 1, 2, analog_send);
    
    client.publish("potensiometer", analog_send);
    
    button1_state = !digitalRead(button1_pin);
    if (button1_state == 1){
      client.publish("sensor1", "on");
    } else{
      client.publish("sensor1", "off");
    }


    button2_state = !digitalRead(button2_pin);
    if (button2_state == 1){
      client.publish("sensor2", "on");
    } else{
      client.publish("sensor2", "off");
    }



  }
}
