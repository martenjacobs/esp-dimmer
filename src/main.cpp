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




void publish_gate1(){
  client.publish(mqtt_pub_topic_gate1, get_gate1()==1?"ON":"OFF", true);
}
void publish_gate2(){
  client.publish(mqtt_pub_topic_gate2, get_gate2()==1?"ON":"OFF", true);
}
void publish_gates(){
  publish_gate1();
  publish_gate2();
}

void set_gate(int id, int on){
  switch(id){
    case 1:
      set_gate1(on);
      #if ENABLE_MQTT
      publish_gate1();
      #endif
      break;
    case 2:
      set_gate2(on);
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
}
void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password,
                       mqtt_pub_topic_online, 0, true, "no")) {
      client.publish(mqtt_pub_topic_online, "yes", true);
      client.subscribe(mqtt_sub_topic_gate1);
      client.subscribe(mqtt_sub_topic_gate2);
      client.setCallback(mqtt_callback);
      //Serial.println("connected");
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void mqtt_debug_log(char *msg){
  client.publish(mqtt_pub_topic_log, msg, false);

}

unsigned long last_pub = 0;
unsigned long now = 0;

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
  int curr=digitalRead(4);
  if(curr!=last){
    delay(10);
    if(digitalRead(4)==curr){
      last=curr;
      if(curr){
        #if ENABLE_MQTT
        client.publish(topic, "HIGH", false);
        #endif
      }else{
        #if ENABLE_MQTT
        client.publish(topic, "LOW", false);
        #endif
        return SWITCH_TOGGLE;
      }
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
  client.publish(mqtt_pub_topic_ip, localIP.toString().c_str(), true);
  publish_gates();
}

void mqtt_loop(){
  if (!client.connected()) {
    mqtt_reconnect();
    publish_status();
  }
  client.loop();
}


void setup() {
  //Serial.begin(115200);
  if(!setup_wifi())
    ota_wait();
  localIP=WiFi.localIP();
  setup_ota();
  for(int i=0 ; i < 50 ; i++){
    ota_loop();
    delay(100);
  }
  client.setServer(mqtt_server, mqtt_port);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  board_setup();
  get_values();
  last_pub=millis();
}

void loop() {
  ota_loop();
  board_loop();
  now=millis();
  mqtt_loop();
  gpio_loop();
  if(now-last_pub>300000){
    last_pub=millis();
    publish_status();
  }
}
