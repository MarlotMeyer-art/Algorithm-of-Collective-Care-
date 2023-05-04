#include <ArduinoJson.h>
int TOUCH_PIN[] =    {T3, T0, T9, T8, T7, T6, T5, T4}; 
// pinouts touch pins: 15,  4, 32, 33, 27, 14, 12, 13

#define TOUCH_HISTORY_LEN 6 // history length 
#define TOUCH_HYSTERESIS 2 // this is the sensitivity

#include <NoiselessTouchESP32.h>

// change the last number called in each instance to change the sensitivity of the touch sensor
// the higher the number the less sensitive, the lower the more sensitive 
// has to be multiple of two 

NoiselessTouchESP32 touchsensor1(TOUCH_PIN[0], TOUCH_HISTORY_LEN, 6);
NoiselessTouchESP32 touchsensor2(TOUCH_PIN[1], TOUCH_HISTORY_LEN, 6);
NoiselessTouchESP32 touchsensor3(TOUCH_PIN[2], TOUCH_HISTORY_LEN, 6);
NoiselessTouchESP32 touchsensor4(TOUCH_PIN[3], TOUCH_HISTORY_LEN, 6);
NoiselessTouchESP32 touchsensor5(TOUCH_PIN[4], TOUCH_HISTORY_LEN, 6);
NoiselessTouchESP32 touchsensor6(TOUCH_PIN[5], TOUCH_HISTORY_LEN, 2);
NoiselessTouchESP32 touchsensor7(TOUCH_PIN[6], TOUCH_HISTORY_LEN, 2);
NoiselessTouchESP32 touchsensor8(TOUCH_PIN[7], TOUCH_HISTORY_LEN, 2);

NoiselessTouchESP32 touchsensor[8] = {touchsensor1,touchsensor2,touchsensor3,touchsensor4,touchsensor5,touchsensor6,touchsensor7,touchsensor8}; //T2? =0

#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiUdp.h>

const char * udpAddress = "192.168.22.114"; /*  "192.168.1.55";*/

const int udpPort = 7000;

boolean connected = false;

WiFiManager wm;
WiFiUDP udp;


void setup() {
  Serial.begin(115200);
 Serial.println("connecting to WIFI...");
  if (wm.autoConnect("MarlotsWeb", "12341234")) {
    Serial.println("connected!");
  }
  else {
    Serial.println("Configportal running...");
  }
  Serial.println(" done");

  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
}

int counter[8];
int touch[8];
int raw_mean_value[8];
int highest[8] = {0,0,0,0,0,0,0,0};
int lowest[8] = {255,255,255,255,255,255,255,255};



void loop() {

DynamicJsonDocument doc(8192);

JsonArray countValues = doc.createNestedArray("touch counts");


  for (int i=0; i<8; i++){

  touch[i] = touchsensor[i].touching();
  raw_mean_value[i] = touchsensor[i].value_from_history();
  highest[i] = _max(highest[i], raw_mean_value[i]);
  lowest[i] = _min(lowest[i], raw_mean_value[i]);
  if (touch[i] == 1) {
    
    counter[i]++;
  }


int value = counter[i]/100; // roughly every second its touched will count 1
countValues.add(value);
  }
  
 serializeJson(doc, Serial);
 Serial.println();
 
  udp.beginPacket(udpAddress,udpPort);
  serializeJson(doc, udp);
  Serial.println();
  udp.println();
  udp.endPacket();
delay(50);
}
