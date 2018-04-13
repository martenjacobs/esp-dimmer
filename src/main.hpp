#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <boardiface.hpp>
#include <config.hpp>

IPAddress localIP;

WiFiClient espClient;
PubSubClient client(espClient);

#if ENABLE_OTA==1
void setup_ota();
#endif
void setup_wifi();

#if ENABLE_GATE_1==1
void publish_gate1();
#endif
#if ENABLE_GATE_2==1
void publish_gate2();
#endif

void publish_gates();

void set_gate(int id, int on);

#if ENABLE_MQTT==1
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void mqtt_reconnect();
void mqtt_debug_log(char *msg);
#endif


void gpio_loop();
void publish_status();

void mqtt_loop();
