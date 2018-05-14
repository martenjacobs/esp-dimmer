#ifndef __CONFIG_INCLUDED__
#define __CONFIG_INCLUDED__

#include <constants.hpp>

// WiFi connection information
#define wifi_ssid "<SSID>"
#define wifi_password "<KEY>"
#define wifi_wait_sec 10

// Information used to start up an AP when connecting to WiFi fails,
// so you can push firmware through OTA
#define ap_ssid "esp-dimmer-1"
#define ap_psk ""
#define ap_ip IPAddress(192, 168, 43, 1)
#define ap_gw IPAddress(192, 168, 43, 1)
#define ap_sn IPAddress(255, 255, 255, 0)

// The connection information for the MQTT server
#define mqtt_server "<HOST>"
#define mqtt_port <PORT>
#define mqtt_user "<MQTT_USER>"
#define mqtt_password "<MQTT_PASSWORD>"
#define mqtt_client_id "<UNIQUE_CLIENT_ID>"

// You probably don't need to change anything here, but you can
#define mqtt_namespace mqtt_client_id "/"
#define mqtt_pub_topic_ip mqtt_namespace "ip-address"
#define mqtt_pub_topic_log mqtt_namespace "log"
#define mqtt_pub_topic_state mqtt_namespace "state"
#define mqtt_sub_topic_state mqtt_namespace "state/request"
#define mqtt_pub_topic_online mqtt_namespace "online"
#define mqtt_pub_topic_gpio4 mqtt_namespace "gpio4"
#define mqtt_pub_topic_gpio5 mqtt_namespace "gpio5"
#define mqtt_pub_topic_gpio12 mqtt_namespace "gpio12"
#define mqtt_pub_topic_gpio14 mqtt_namespace "gpio14"
#define mqtt_pub_topic_gate1 mqtt_namespace "gate1/state"
#define mqtt_pub_topic_gate2 mqtt_namespace "gate2/state"
#define mqtt_sub_topic_gate1 mqtt_namespace "gate1/set"
#define mqtt_sub_topic_gate2 mqtt_namespace "gate2/set"
#define mqtt_pub_topic_dim1 mqtt_namespace "dim1/state"
#define mqtt_pub_topic_dim2 mqtt_namespace "dim2/state"
#define mqtt_sub_topic_dim1 mqtt_namespace "dim1/set"
#define mqtt_sub_topic_dim2 mqtt_namespace "dim2/set"
#define mqtt_sub_topic_chlock mqtt_namespace "chlock/set"
#define mqtt_sub_topic_eeprom_read mqtt_namespace "eeprom/read"
#define mqtt_sub_topic_eeprom_write mqtt_namespace "eeprom/write"

// The interval at which the module publishes a full state report to MQTT
#define INFO_PUBLISH_INTERVAL 300000

// Enables updating the firmware over WiFi (you probably want this)
#define ENABLE_OTA 1

// Enables the device to connect to an MQTT broker
#define ENABLE_MQTT 1

// Enables the gates
#define ENABLE_GATE_1 1
#define ENABLE_GATE_2 1

// Enable dimming (otherwise the device is a simple on/off switch)
#define ENABLE_DIMMER 1

// Enable channel locking. Channel locking is used for blinds, and makes sure
// the channels can't be switched on simultaneously
#define CHANNEL_LOCK 0

// Writes every state change to EEPROM so it persists across restarts. If
// disabled, the start-up state is set by publishing a message to the
// mqtt_sub_topic_eeprom_write topic
#define PERSIST_STATE 1

// Set up how the GPIOs are wired up
#define SETUP_GPIO4_INPUT CONTROL_TOGGLE_BUTTON
#define SETUP_GPIO4_CONTROL GATE_1
#define SETUP_GPIO5_INPUT CONTROL_TOGGLE_BUTTON
#define SETUP_GPIO5_CONTROL GATE_2

#endif
