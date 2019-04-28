/*
 * ESP8266 Wireless temperature sensor
 * 
 * © Janne Kemppainen, 2019
 */
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "network_name";
const char* password = "password";
const char* mqtt_server = "192.168.1.168";
const char* client_id = "ESP-01-balcony";
const char* mqtt_topic = "home/balcony/temperature";
const long deep_sleep_seconds = 360;
const long deep_sleep_time = 1000000 * deep_sleep_seconds;

WiFiClient espClient;
PubSubClient client(espClient);
char msg[10];  // message buffer

#define ONE_WIRE_PIN 2

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature DS18B20(&oneWire);

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

void mqtt_connect() {
  Serial.print("Connecting MQTT...");
  client.setServer(mqtt_server, 1883);  
  if (client.connect(client_id)) {
    Serial.println("connected");
    client.loop();
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println("Going to sleep..");
    ESP.deepSleep(1000000*60);
  }
}

float get_temperature() {
  Serial.println("Read temperature");
  DS18B20.begin();
  DS18B20.requestTemperatures();
  return DS18B20.getTempCByIndex(0);
}

void send_temperature(float temperature) {
  dtostrf(temperature, 3, 2, msg);
  Serial.print("Sending temperature ");
  Serial.print(msg);
  Serial.print(" degC");
  Serial.print(" to topic ");
  Serial.println(mqtt_topic);
  client.publish("home/balcony/temperature", msg);
  Serial.println("Published");
}

void setup() {
  Serial.begin(74880);
  setup_wifi();
  mqtt_connect();
  float temperature = get_temperature();
  send_temperature(temperature);
  client.disconnect();
  delay(10);
  Serial.print("Going to sleep for ");
  Serial.print(deep_sleep_seconds);
  Serial.print(" seconds..");
  ESP.deepSleep(deep_sleep_time);
}

void loop() {
  // not used 
}