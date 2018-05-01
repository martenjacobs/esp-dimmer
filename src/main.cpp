#include <main.hpp>
#include <PubSubClient.h>
#include <boardiface.hpp>
#include <wifi.hpp>
#if ENABLE_OTA==1
#include <ota.hpp>
#endif

//TODO: clean up this mess

IPAddress localIP;
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long last_pub = 0;
unsigned long now = 0;
unsigned long now_mqtt = 0;
unsigned long lastmqtt_retry = -1;


void publish_gate1(){
  mqtt_publish(mqtt_pub_topic_gate1, get_gate1()==1?"ON":"OFF", true);
  char cstr[16];
  mqtt_publish(mqtt_pub_topic_dim1, itoa(get_dim1(), cstr, 10), true);
}
void publish_gate2(){
  mqtt_publish(mqtt_pub_topic_gate2, get_gate2()==1?"ON":"OFF", true);
  char cstr[16];
  mqtt_publish(mqtt_pub_topic_dim2, itoa(get_dim2(), cstr, 10), true);
}
void publish_gates(){
  publish_gate1();
  publish_gate2();
}

void set_gate(int id, int on){
  switch(id){
    case 1:
      set_gate1(on);
      #if ENABLE_DIMMER
      restore_dim_level(id);
      #endif
      #if ENABLE_MQTT
      publish_gate1();
      #endif
      break;
    case 2:
      set_gate2(on);
      #if ENABLE_DIMMER
      restore_dim_level(id);
      #endif
      #if ENABLE_MQTT
      publish_gate2();
      #endif
      break;
  }
}
void set_dim(int id, uint8_t value){
  switch(id){
    case 1:
      set_dimm1_tbl(value);
      #if ENABLE_MQTT
      publish_gate1();
      #endif
      break;
    case 2:
      set_dimm2_tbl(value);
      #if ENABLE_MQTT
      publish_gate2();
      #endif
      break;
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length){
	char payload_str[length+1];
	for (unsigned int i = 0; i < length; i++) {
		payload_str[i] = (char) payload[i];
	}
  payload_str[length]=0;

  if(strcmp(topic, mqtt_sub_topic_gate1) == 0){
    set_gate(1, strcmp(payload_str, "ON")==0?1:0);
    return;
  }
  if(strcmp(topic, mqtt_sub_topic_gate2) == 0){
    set_gate(2, strcmp(payload_str, "ON")==0?1:0);
    return;
  }
  if(strcmp(topic, mqtt_sub_topic_dim1) == 0){
    set_dim_level(1, payload_str);
    return;
  }
  if(strcmp(topic, mqtt_sub_topic_dim2) == 0){
    set_dim_level(2, payload_str);
    return;
  }
  if(strcmp(topic, mqtt_sub_topic_state) == 0){
    get_values();
    publish_status();
    return;
  }
  if(strcmp(topic, mqtt_sub_topic_eeprom_read) == 0){
    read_eeprom();
    get_values();
    publish_status();
    return;
  }
  if(strcmp(topic, mqtt_sub_topic_eeprom_write) == 0){
    write_eeprom();
    return;
  }
}
int mqtt_publish (const char* topic, const char* payload, bool retained){
#if ENABLE_MQTT
if (client.connected()){
  return client.publish(topic,payload,retained);
  }
else{
  return false;
}
#endif
return false;
}

void restore_dim_level(uint8_t gate){
  set_dim_level(gate, get_dim(gate));
}

void set_dim_level(uint8_t gate, char* value){
  uint8_t val;
  sscanf(value, "%d", &val);
  set_dim_level(gate, val);
}

void set_dim_level(uint8_t gate, uint8_t value){
  set_dim(gate, value);
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    now_mqtt = millis();
    if (now_mqtt - lastmqtt_retry > 5000 or lastmqtt_retry == -1){
      lastmqtt_retry = millis();
      if (client.connect(mqtt_client_id, mqtt_user, mqtt_password,
                         mqtt_pub_topic_online, 0, true, "no")) {
        mqtt_publish(mqtt_pub_topic_online, "yes", true);
        client.subscribe(mqtt_sub_topic_gate1);
        client.subscribe(mqtt_sub_topic_gate2);
        client.subscribe(mqtt_sub_topic_dim1);
        client.subscribe(mqtt_sub_topic_dim2);
        client.subscribe(mqtt_sub_topic_state);
        client.subscribe(mqtt_sub_topic_eeprom_read);
        client.subscribe(mqtt_sub_topic_eeprom_write);
        client.setCallback(mqtt_callback);
        //Serial.println("connected");
      } else {
        //Serial.print("failed, rc=");
        //Serial.print(client.state());
        //Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        // delay(5000);
      }
    }
    else{
      // If not connected to mqtt broker, handle gpio and OTA
      gpio_loop();
      OTA_LOOP_IF_ENABLED;
    }
    delay(50);
  }
}


void mqtt_debug_log(String msg){
  mqtt_publish(mqtt_pub_topic_log, msg.c_str(), false);
}


int get_gate(int gate){
  switch(gate){
    case GATE_1:
      return get_gate1();
    case GATE_2:
      return get_gate2();
  }
}

void update_gates(int gate, int action){
  if(gate==GATE_BOTH){
    if(action==SWITCH_TOGGLE){
      int sw = (get_gate1()==1 || get_gate2()==1) ? 0 : 1;
      set_gate(1, sw);
      set_gate(2, sw);
    }else{
      set_gate(1, action);
      set_gate(2, action);
    }
  }else{
    if(action==SWITCH_TOGGLE){
      set_gate(gate, 1-get_gate(gate));
    }else{
      set_gate(gate, action);
    }
  }
}

int check_pulse_button(int id, int &last, const char *topic){
  int curr=digitalRead(id);
  if(curr!=last){
    delay(10);
    if(digitalRead(id)==curr){
      last=curr;
      if(curr){
        mqtt_publish(topic, "HIGH", false);
      }else{
        mqtt_publish(topic, "LOW", false);
        return SWITCH_TOGGLE;
      }
    }
  }
  return SWITCH_NO_CHANGE;
}
int check_toggle_button(int id, int &last, const char *topic){
  int curr=digitalRead(id);
  if(curr!=last){
    delay(10);
    if(digitalRead(id)==curr){
      last=curr;
      if(curr){
        mqtt_publish(topic, "HIGH", false);
      }else{
        mqtt_publish(topic, "LOW", false);
      }
      return SWITCH_TOGGLE;
    }
  }
  return SWITCH_NO_CHANGE;
}

#ifdef SETUP_GPIO4_INPUT
int last_gpio4 = -1;
#endif
#ifdef SETUP_GPIO5_INPUT
int last_gpio5 = -1;
#endif
#ifdef SETUP_GPIO12_INPUT
int last_gpio12 = -1;
#endif
#ifdef SETUP_GPIO14_INPUT
int last_gpio14 = -1;
#endif

int gpio_change = SWITCH_NO_CHANGE;
void gpio_loop(){
  #ifdef SETUP_GPIO4_INPUT
    #if SETUP_GPIO4_INPUT == CONTROL_PULSE_BUTTON
      gpio_change=check_pulse_button(GPIO4, last_gpio4, mqtt_pub_topic_gpio4);
    #elif SETUP_GPIO4_INPUT == CONTROL_TOGGLE_BUTTON
      gpio_change=check_toggle_button(GPIO4, last_gpio4, mqtt_pub_topic_gpio4);
    #endif
    #ifdef SETUP_GPIO4_CONTROL
      if(gpio_change!=SWITCH_NO_CHANGE)
        update_gates(SETUP_GPIO4_CONTROL, gpio_change);
    #endif
  #endif
  #ifdef SETUP_GPIO5_INPUT
    #if SETUP_GPIO5_INPUT == CONTROL_PULSE_BUTTON
      gpio_change=check_pulse_button(GPIO5, last_gpio5, mqtt_pub_topic_gpio5);
    #elif SETUP_GPIO5_INPUT == CONTROL_TOGGLE_BUTTON
      gpio_change=check_toggle_button(GPIO5, last_gpio5, mqtt_pub_topic_gpio5);
    #endif
    #ifdef SETUP_GPIO5_CONTROL
      if(gpio_change!=SWITCH_NO_CHANGE)
        update_gates(SETUP_GPIO5_CONTROL, gpio_change);
    #endif
  #endif
  #ifdef SETUP_GPIO12_INPUT
    #if SETUP_GPIO12_INPUT == CONTROL_PULSE_BUTTON
      gpio_change=check_pulse_button(GPIO12, last_gpio12, mqtt_pub_topic_gpio12);
    #elif SETUP_GPIO12_INPUT == CONTROL_TOGGLE_BUTTON
      gpio_change=check_toggle_button(GPIO12, last_gpio12, mqtt_pub_topic_gpio12);
    #endif
    #ifdef SETUP_GPIO12_CONTROL
      if(gpio_change!=SWITCH_NO_CHANGE)
        update_gates(SETUP_GPIO12_CONTROL, gpio_change);
    #endif
  #endif
  #ifdef SETUP_GPIO14_INPUT
    #if SETUP_GPIO14_INPUT == CONTROL_PULSE_BUTTON
      gpio_change=check_pulse_button(GPIO14, last_gpio14, mqtt_pub_topic_gpio14);
    #elif SETUP_GPIO14_INPUT == CONTROL_TOGGLE_BUTTON
      gpio_change=check_toggle_button(GPIO14, last_gpio14, mqtt_pub_topic_gpio14);
    #endif
    #ifdef SETUP_GPIO14_CONTROL
      if(gpio_change!=SWITCH_NO_CHANGE)
        update_gates(SETUP_GPIO14_CONTROL, gpio_change);
    #endif
  #endif
}

void publish_status(){

  mqtt_publish(mqtt_pub_topic_ip, localIP.toString().c_str(), true);
  publish_gates();

  Dimmer d = get_state();
  char cstr[16];

  mqtt_publish(mqtt_pub_topic_state "/version_major", itoa(d.version_major, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/version_minor", itoa(d.version_minor, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate1_on", itoa(d.gate1_on, cstr, 10), false);
  //mqtt_publish(mqtt_pub_topic_state "/gate1_bright_proz", itoa(d.gate1_bright_proz, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate1_bright_tbl", itoa(d.gate1_bright_tbl, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate1_dimm", itoa(d.gate1_dimm, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate1_impuls_start", itoa(d.gate1_impuls_start, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate1_impuls_len", itoa(d.gate1_impuls_len, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate2_on", itoa(d.gate2_on, cstr, 10), false);
  //mqtt_publish(mqtt_pub_topic_state "/gate2_bright_proz", itoa(d.gate2_bright_proz, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate2_bright_tbl", itoa(d.gate2_bright_tbl, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate2_dimm", itoa(d.gate2_dimm, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate2_impuls_start", itoa(d.gate2_impuls_start, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate2_impuls_len", itoa(d.gate2_impuls_len, cstr, 10), false);
  mqtt_publish(mqtt_pub_topic_state "/gate_lock", itoa(d.gate_lock, cstr, 10), false);
}

void mqtt_loop(){
  if (!client.connected()) {
    mqtt_reconnect();
    publish_status();
  }
  client.loop();
}

void mqtt_setup(){
  client.setServer(mqtt_server, mqtt_port);
}

void setup() {
  //Serial.begin(115200);
  if(!wifi_setup()){
    //TODO: Something if OTA is not enabled
    ota_setup_and_wait();
  }
  OTA_SETUP_IF_ENABLED;
  for(int i=0 ; i < 50 ; i++){
    OTA_LOOP_IF_ENABLED;
    delay(100);
  }
  localIP=WiFi.localIP();
  mqtt_setup();
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  board_setup();
  get_values();
  last_pub=millis();
}

void loop() {
  OTA_LOOP_IF_ENABLED;
  board_loop();
  now=millis();
  mqtt_loop();
  gpio_loop();
  if(now-last_pub>INFO_PUBLISH_INTERVAL){
    last_pub=millis();
    publish_status();
  }
}
