#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DHT.h>

// Sensor
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Networking
#define MQTT_VERSION MQTT_VERSION_3_1
IPAddress mqttServer;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void publishTemp(){
  Serial.println("reading temp");
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  // a "JSON" payload
  String payload = "{temperature:" + String(t) + ", humidity:" + String(h) +  "}";
  char buffer[payload.length()+1];
  payload.toCharArray(buffer, payload.length()+1);
  client.publish("temp/reading",buffer);
}

void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i=0;i<length;i++) {
      message += (char)payload[i];
    }
    Serial.println(message);
    if(message == "readTemp") {
    	publishTemp();
    }
}


bool connect() {
   return client.connect(HOSTNAME, MQTT_USER, MQTT_PASSWORD);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (connect()) {
      Serial.println("connected");
      client.subscribe("temp/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Wifi setup

void setupWifi(){
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  delay(150);
  WiFi.setHostname(HOSTNAME);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMqtt(){
  if(mqttServer.fromString(MQTT_SERVER_IP)) {
    Serial.println("Setting up MQTT");
    client.setServer(mqttServer, 8083);
    client.setCallback(callback);
    Serial.println("MQTT setup is done");
  }
}

void setup() {
  Serial.begin(115200);
  setupWifi();
  setupMqtt();
  dht.begin();
  Serial.println("Setup done!!!!");
}

void loop() {
    if (!client.connected()) {
    Serial.println("Reconnecting to MQTT broker");
    reconnect();
  }
  client.loop();
}
