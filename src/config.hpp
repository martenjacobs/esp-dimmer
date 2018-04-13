#include <constants.hpp>

#define wifi_ssid "<SSID>"
#define wifi_password "<KEY>"
#define mqtt_server "<HOST>"
#define mqtt_port <PORT>
#define mqtt_user "<MQTT_USER>"
#define mqtt_password "<MQTT_PASSWORD>"
#define mqtt_client_id "<UNIQUE_CLIENT_ID>"

#define mqtt_namespace mqtt_client_id "/"
#define mqtt_pub_topic_ip mqtt_namespace "ip-address"
#define mqtt_pub_topic_log mqtt_namespace "log"
#define mqtt_pub_topic_state mqtt_namespace "state"
#define mqtt_pub_topic_online mqtt_namespace "online"
#define mqtt_pub_topic_gpio4 mqtt_namespace "gpio4"
#define mqtt_pub_topic_gpio5 mqtt_namespace "gpio5"
#define mqtt_pub_topic_gpio12 mqtt_namespace "gpio12"
#define mqtt_pub_topic_gpio14 mqtt_namespace "gpio14"
#define mqtt_pub_topic_gate1 mqtt_namespace "gate1/state"
#define mqtt_pub_topic_gate2 mqtt_namespace "gate2/state"
#define mqtt_sub_topic_gate1 mqtt_namespace "gate1/set"
#define mqtt_sub_topic_gate2 mqtt_namespace "gate2/set"



#define ENABLE_OTA 1
#define ENABLE_MQTT 1

#define ENABLE_GATE_1 1
#define ENABLE_GATE_2 2

#define SETUP_GPIO4_INPUT CONTROL_PULSE_BUTTON
#define SETUP_GPIO4_CONTROL GATE_BOTH
#define SETUP_GPIO5_INPUT CONTROL_PULSE_BUTTON
//#define SETUP_GPIO5_CONTROL GATE_BOTH
//#define SETUP_GPI12_INPUT CONTROL_PULSE_BUTTON
//#define SETUP_GPI12_CONTROL GATE_BOTH
//#define SETUP_GPI14_INPUT CONTROL_PULSE_BUTTON
//#define SETUP_GPI14_CONTROL GATE_BOTH
