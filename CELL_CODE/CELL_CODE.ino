#include <ArduinoJson.h>
#include <NoiselessTouchESP32.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiUdp.h>

int TOUCH_PIN[] =    {T0, T3, T4, T5, T6, T7, T8, T9}; 
const int pinOuts[] = {26, 16, 17, 18, 19, 21, 22, 23};


#define CELL_NUMBER 1
#define TOUCH_HISTORY_LEN 6 // history length 
#define TOUCH_HYSTERESIS 2 // this is the sensitivity


NoiselessTouchESP32 touchsensor1(TOUCH_PIN[0], TOUCH_HISTORY_LEN, 2);
NoiselessTouchESP32 touchsensor2(TOUCH_PIN[1], TOUCH_HISTORY_LEN, 2);
NoiselessTouchESP32 touchsensor3(TOUCH_PIN[2], TOUCH_HISTORY_LEN, 2);
NoiselessTouchESP32 touchsensor4(TOUCH_PIN[3], TOUCH_HISTORY_LEN, 2);
NoiselessTouchESP32 touchsensor5(TOUCH_PIN[4], TOUCH_HISTORY_LEN, 2);
NoiselessTouchESP32 touchsensor6(TOUCH_PIN[5], TOUCH_HISTORY_LEN, 2);
NoiselessTouchESP32 touchsensor7(TOUCH_PIN[6], TOUCH_HISTORY_LEN, 2);
NoiselessTouchESP32 touchsensor8(TOUCH_PIN[7], TOUCH_HISTORY_LEN, 2);

NoiselessTouchESP32 touchsensor[8] = {touchsensor1,touchsensor2,touchsensor3,touchsensor4,touchsensor5,touchsensor6,touchsensor7,touchsensor8}; //T2? =0

uint8_t count = 0;

const char * udpAddress = "192.168.1.246"; /*  "192.168.1.55";*/


const int udpPort = 10000 + CELL_NUMBER;

boolean connected = false;

WiFiManager wm;
WiFiUDP udp;


char packetBuffer[255]; //buffer to hold incoming packet


void wifiSetup() {
  Serial.println("connecting to WIFI...");
  if (wm.autoConnect("MarlotsWeb", "12341234")) {
    Serial.println("connected!");
  }
  else {
    Serial.println("Configportal running...");
  }
  Serial.println(" done");

  Serial.println("");
  Serial.print("IP address:    ");
  Serial.println(WiFi.localIP());
}

void setup() {
 
 Serial.begin(115200);
 Serial.print("HI I AM : ");
 Serial.print(CELL_NUMBER);

 wifiSetup();


  for (count=0;count<8;count++) {  
  pinMode(pinOuts[count], OUTPUT); 
  digitalWrite(pinOuts[count], HIGH);
  delay(2000);
digitalWrite(pinOuts[count], LOW);
delay(100);
  }

 
  udp.begin(udpPort);
 Serial.print("IDP BEGIN : ");
 Serial.println(udpPort);


}

int HeatState[8];
int counter[] = {0,0,0,0,0,0,0,0};

int raw_mean_value[8];
int highest[8] = {0,0,0,0,0,0,0,0};
int lowest[8] = {255,255,255,255,255,255,255,255};

int touch[8];
int change[8];
int touch_detected[8];
 
void loop() {
  
updateTouchArray();
getStates();

 
}



void updateTouchArray() {
  
DynamicJsonDocument doc(8192);
JsonArray countValues = doc.createNestedArray(CELL_NUMBER);

 for (int i=0; i<8; i++){
    
  touch_detected[i] = touchsensor[i].touched();
  change[i] = touchsensor[i].changed();
  touch[i] = touchsensor[i].touching();

  
  raw_mean_value[i] = touchsensor[i].value_from_history();
  highest[i] = _max(highest[i], raw_mean_value[i]);
  lowest[i] = _min(lowest[i], raw_mean_value[i]);

if (touch_detected[i] == 1) {
  counter[i] = 0;
  Serial.println(counter[i]);
} 

if (touch[i] == 1) {
  counter[i]++;
  Serial.println(counter[i]);

}

else if (touch[i] == 0) {
  counter[i] = 0; 
  Serial.println(counter[i]);
}
  
int value = counter[i]; // roughly every second its touched will count 1
countValues.add(value);

}

 serializeJson(doc, Serial);
 Serial.println();
 Serial.println();

 udp.beginPacket(udpAddress,udpPort);
  serializeJson(doc, udp);
  udp.println();
  udp.endPacket();
  
delay(50);
}

void getStates() {
  
  int packetSize = udp.parsePacket();
  if (packetSize) {}
  int len = udp.read(packetBuffer, 255);
  if (len > 0) {
    packetBuffer[len] = 0;
  }


  for (int i=0; i<8; i++){
    
 HeatState[i] = String((char *)packetBuffer).substring(i,i+1).toInt();
 
 Serial.print("heat State Number: ");
 Serial.println(i);
 Serial.print("heat State");
 Serial.println(HeatState[i]);

if (HeatState[i] == 1)  {
   Serial.print("heat State pin ");
   Serial.print(i);
   Serial.print(" :");
   Serial.println(HeatState[i]);
   digitalWrite(pinOuts[i], HIGH);

} else {
        digitalWrite(pinOuts[i], LOW);
}
delay(100);  

}

}

/*
Serial.print(packetBuffer[0]);
  String id = String((char*)packetBuffer);
      int Val = String((char *)packetBuffer).toInt();
//  int 1 = 
Serial.print("\nMessage From Udp\t");
Serial.println(id);
Serial.println(Val);*/
  
