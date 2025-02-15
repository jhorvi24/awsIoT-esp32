#include <DHT.h>
#include <DHT_U.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include "credentials.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>



#define dhtpin 19
#define DHTTYPE DHT11
#define led 2

#define monitoreo_topic "esp32/sensors" //TOPIC MQTT
#define suscribe_topic "esp32/actuators"

float h;
float t;

WiFiClientSecure client;
PubSubClient esp32(client);

DHT dht(dhtpin, DHTTYPE);

void connectAWS(){


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

   // Configure WiFiClientSecure to use the AWS IoT device credentials
  client.setCACert(AWS_CERT_CA);
  client.setCertificate(AWS_CERT_CRT);
  client.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  esp32.setServer(AWS_IoT_Endpoint, 8883);
 
  // Create a message handler
  esp32.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!esp32.connect(THINGSNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!esp32.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  esp32.subscribe(suscribe_topic);
 
  Serial.println("AWS IoT Connected!");


}


void publishMessage()
{

  StaticJsonDocument<200> doc;

  // Añadir timestamp en formato ISO
  char timestamp[20];
  sprintf(timestamp, "%lu", millis());
  
  doc["timestamp"] = timestamp;




  doc["humidity"] = h;
  doc["temperature"] = t;  //Organizar data en un json

  doc["device_id"] = "esp32_1";

  char jsonBuffer[512]; //crear un buffer
  serializeJson(doc, jsonBuffer); // Convertir en un string y almacenar en el buffer
 
  esp32.publish(monitoreo_topic, jsonBuffer);  //Public data al topic mqtt

}


void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);

    if (strcmp(message, "ON") == 0) {
        digitalWrite(led, HIGH);
        Serial.println("LED turned ON");
    } 
    else if (strcmp(message, "OFF") == 0) {
        digitalWrite(led, LOW);
        Serial.println("LED turned OFF");
    }
    else {
        Serial.println("Unknown command received");
    }
}


void reconnect(){

  while (!esp32.connected()){
    Serial.println("Attempting MQTT connection...");
    if (esp32.connect("esp32Client")){
      Serial.println("Connected");

    } else{
      Serial.println("Failed, rc=");
      Serial.print(esp32.state());
      Serial.println(" Try again in 5 seconds");
      delay(5000);
    }

  }
}

void setup() 
{
  // put your setup code here, to run once:

  //pinMode(11, INPUT);
  Serial.begin(9600);
  
  connectAWS();
  Serial.println("WiFi connected");
  Serial.println("Connected to IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connected to ");
  Serial.println(ssid);

  dht.begin();
  pinMode(led, OUTPUT);
  
  //reconnect();


}

void loop() 
{
  // put your main code here, to run repeatedly:
  
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    WiFi.begin(ssid, password);
  }

  Serial.println("WiFi connected");
  Serial.println("Connected to IP address: ");
  Serial.println(WiFi.localIP());

  if (!esp32.connected()){
    reconnect();
  }

  delay(5000);

  h = dht.readHumidity();
  t = dht.readTemperature();

  

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println("%");
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

  publishMessage();
  esp32.loop(); //Mantener conexión con el servidor
  //delay(1000);

  if (t>=33){
    digitalWrite(led, HIGH);    
  }else{
    digitalWrite(led, LOW);
  }

}