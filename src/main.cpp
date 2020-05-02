#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define MQTT_VERSION MQTT_VERSION_3_1

IPAddress mqttServer(MQTT_SERVER);

WiFiClient wifiClient;

PubSubClient client(wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i=0;i<length;i++) {
       Serial.print((char)payload[i]);
    }
    Serial.println();
    client.publish("temp/reading","1");
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
  Serial.println("Setting up MQTT");
  client.setServer(mqttServer, 8083);
  client.setCallback(callback);
  Serial.println("MQTT setup is done");
}


void setup() {
  Serial.begin(115200);
  setupWifi();
  delay(5000);
  setupMqtt();
  Serial.println("Setup done");
}

void loop() {
  if (!client.connected()) {
    Serial.println("Reconnecting to MQTT broker");
    reconnect();
  }
  client.loop();
}
