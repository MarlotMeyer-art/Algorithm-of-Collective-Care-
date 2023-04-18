#include <ArduinoJson.h>

#include "Arduino.h"
#include <Wire.h>
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"
#include <SPI.h>


#define EMMISIVITY 0.95
#define TA_SHIFT 8 
paramsMLX90640 mlx90640;

const byte MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640
static float tempValues[32 * 24];


#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiUdp.h>


const char * udpAddress = "192.168.22.114"; /*  "192.168.1.55";*/

const int udpPort = 7000;

boolean connected = false;

WiFiManager wm;
WiFiUDP udp;



/*void readTempValues() {
  for (byte x = 0 ; x < 2 ; x++)  {
  
    uint16_t mlx90640Frame[834];
    int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
  
     if (status < 0) {
      Serial.print("GetFrame Error: ");
      Serial.println(status);
      }

    float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
    float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
    float tr = Ta - TA_SHIFT; 
    MLX90640_CalculateTo(mlx90640Frame, &mlx90640, EMMISIVITY, tr, tempValues); }

}*/



void setup() {
  Serial.begin(115200);
  
  Wire.begin();
  Wire.setClock(400000); 
  
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
  
  Wire.beginTransmission((uint8_t)MLX90640_address);
 
  if (Wire.endTransmission() != 0) {
    Serial.println("MLX90640 not detected at default I2C address. Starting scan the device addr...");
  }
  else {   Serial.println("MLX90640 online!"); 
  }
  
  int status;
  uint16_t eeMLX90640[832];
  status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);
  if (status != 0) Serial.println("Failed to load system parameters");
  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  if (status != 0) Serial.println("Parameter extraction failed");
  //MLX90640_SetRefreshRate(MLX90640_address, 0x02); 
  MLX90640_SetRefreshRate(MLX90640_address, 0x04);
  Wire.setClock(1000000);
}

void loop(void) {
    
  DynamicJsonDocument doc(12288);
  JsonArray matrixValues = doc.createNestedArray("temp");
  //readTempValues();

  for (byte x = 0 ; x < 2 ; x++)  {
  
    uint16_t mlx90640Frame[834];
    int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
  
     if (status < 0) {
      Serial.print("GetFrame Error: ");
      Serial.println(status);
      }

    float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
    float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
    float tr = Ta - TA_SHIFT; 
    MLX90640_CalculateTo(mlx90640Frame, &mlx90640, EMMISIVITY, tr, tempValues); }

  for (int i = 0; i < 768; i++) {

    int value = tempValues[i]*100;
    matrixValues.add(value);
  }

 //   StaticJsonDocument<16> pulsedoc;
 //   pulsedoc[0] = analogRead(39);

  udp.beginPacket(udpAddress,udpPort);
  serializeJson(doc, udp);
  serializeJson(doc, Serial);
  Serial.println();
 // serializeJson(pulsedoc, udp);
 // serializeJson(pulsedoc, Serial);
  udp.println();
  udp.endPacket();
}
