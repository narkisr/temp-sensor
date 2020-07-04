#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DHT.h>

// Deep sleep
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  10

RTC_DATA_ATTR int bootCount = 0;

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
  log_d("reading temp");
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  // a "JSON" payload
  String payload = "{temperature:" + String(t) + ", humidity:" + String(h) +  "}";
  log_d("payload is %s", payload);
  char buffer[payload.length()+1];
  payload.toCharArray(buffer, payload.length()+1);
  bool result = client.publish("temp/reading" , buffer);
  log_d("published reading %s", result);
}

void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i=0 ; i<length ; i++) {
      message += (char)payload[i];
    }
    log_d("Message is %s", message);
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
    log_d("Attempting MQTT connection...");
    if (connect()) {
      log_d("connected");
      client.subscribe("temp/control");
    } else {
      log_d("failed, rc=%d",client.state());
      log_d("try again in 5 seconds");
      delay(5000);
    }
  }
}

// Wifi setup

void setupWifi(){
  log_d("Connecting to ");
  log_d(WIFI_SSID);

  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  delay(150);
  WiFi.setHostname(HOSTNAME);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  log_d("WiFi connected");
  log_d("IP address: %s", WiFi.localIP());
}

void setupMqtt(){
  if(mqttServer.fromString(MQTT_SERVER_IP)) {
    log_d("Setting up MQTT");
    client.setServer(mqttServer, 8083);
    client.setCallback(callback);
    log_d("MQTT setup is done");
  }
}

void closeMqtt(){
  client.disconnect();
  wifiClient.flush();
  while( client.state() != -1 ){
    delay(10);
  }
  wifiClient.stop();
}

void setup() {
  Serial.begin(115200);
  delay(1000); // wait for serial to start
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  setupWifi();
  setupMqtt();
  dht.begin();

  if (!client.connected()) {
    log_d("Reconnecting to MQTT broker");
    reconnect();
  }
  publishTemp();

  log_d("Going to sleep now");
  delay(1000);
  Serial.flush();
  closeMqtt();
  esp_deep_sleep_start();
}

void loop() {
  /*if (!client.connected()) {
    log_d("Reconnecting to MQTT broker");
    reconnect();
  }
  client.loop();*/
}
