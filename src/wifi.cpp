
#include <config.hpp>
#include <wifi.hpp>

bool wifi_setup() {
  delay(10);
  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(wifi_ssid);

  //WiFi.setPhyMode(WIFI_PHY_MODE_11G);
  //WiFi.setOutputPower(10*4.0f);
  WiFi.mode(WIFI_STA);

  WiFi.begin(wifi_ssid, wifi_password);
  boolean success=false;
  for(int i=0; i<(wifi_wait_sec*2) ; i++){
    if(WiFi.status() == WL_CONNECTED){
      success=true;
      break;
    }
    delay(500);
    //Serial.print(".");
  }
  if(!success){
    //Serial.println("Failed to connect");
    //Serial.print("Setting up softAP");
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ap_ip, ap_gw, ap_sn);
    WiFi.softAP(ap_ssid, ap_psk);
    return false;
  }
  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.print("IP address: ");
  //localIP = WiFi.localIP();
  //Serial.println(localIP);
  return true;
}
