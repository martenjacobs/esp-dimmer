# ESP dimmer

This is a first step towards creating an open source firmware for the ESP8266-based dimmer modules sold by [ex-store.de](https://ex-store.de/2-Kanal-RS232-WiFi-Dimmer-Modul-V4-fuer-Unterputzmontage-230V-3A).

## NOTE:
A MQTT broker, like [Mosquitto](https://mosquitto.org/download/) should be running.

## Usage:
Edit src/config.hpp to match your WIFI, MQTT settings.
You should also set a name for your dimmer:

```c++
#define mqtt_namespace mqtt_client_id "<DIMMER NAME>/"
```



## Example script for home assistant:
```yaml
- platform: mqtt
  name: "Keuken"
  command_topic: "dimmer-1/gate1/set"
  state_topic: "dimmer-1/gate1/state"
  availability_topic: "dimmer-1/online"
  brightness_command_topic: "dimmer-1/dim1/set"
  brightness_state_topic: "dimmer-1/dim1/set"
  brightness_scale: 255
  payload_on: "ON"
  payload_off: "OFF"
  payload_available: "yes"
  payload_not_available: "no"
```
