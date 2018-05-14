#ifndef __MAIN_INCLUDED__
#define __MAIN_INCLUDED__

#include <config.hpp>
#include <Arduino.h>

#if ENABLE_GATE_1==1
void publish_gate1();
#endif
#if ENABLE_GATE_2==1
void publish_gate2();
#endif

inline void publish_gates();

void set_gate(int id, int on);

#if ENABLE_MQTT
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void mqtt_reconnect();
inline void mqtt_debug_log(String msg);
int mqtt_publish (const char* topic, const char* payload, bool retained);
#endif

#if ENABLE_DIMMER
void set_dim_level(uint8_t gate, char* value);
void set_dim_level(uint8_t gate, uint8_t value);
void restore_dim_level(uint8_t gate);
#endif

void set_enabled(bool enabled);
void gpio_loop();
void publish_status();

void mqtt_loop();

#endif
