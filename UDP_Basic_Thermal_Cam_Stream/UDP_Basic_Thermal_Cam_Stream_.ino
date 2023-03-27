#include "Arduino.h"
#include <Wire.h>
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"


#define EMMISIVITY 0.95
#define TA_SHIFT 8 
paramsMLX90640 mlx90640;

const byte MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640
static float tempValues[32 * 24];

//#include <WiFiClient.h>
//#include <strings_en.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <WiFiUdp.h>

//const char * networkName = "TMNL-8A72C1";
//const char * networkPswd = "WJTU7VTXD44WJFP5";

const char * udpAddress = "192.168.1.55";


const int size = 768; /*Array Size defined*/
int Array_New[size]; /*New Array is defined*/

const int udpPort = 7000;

String myId = "ESP1";

boolean connected = false;

WiFiManager wm;
WiFiUDP udp;

//WiFiClient client;


void readTempValues() {
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

/* -------------------- get array --------------------*/

  for (int i = 0; i < 768; i++) {

   

     Array_New[i] = tempValues[i]*100;

    
     }
}



void setup() {
  
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  
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
  
  sendMessage("I am:" + myId);
  Serial.println(String("Sent message: I am:" + myId));

  Wire.begin();
  Wire.setClock(400000); 
  Wire.beginTransmission((uint8_t)MLX90640_address);
 
  if (Wire.endTransmission() != 0) {
    Serial.println("MLX90640 not detected at default I2C address. Starting scan the device addr...");
    Device_Scan();
  }
  else {   Serial.println("MLX90640 online!"); 
  }
  
  int status;
  uint16_t eeMLX90640[832];
  status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);
  if (status != 0) Serial.println("Failed to load system parameters");
  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  if (status != 0) Serial.println("Parameter extraction failed");
  MLX90640_SetRefreshRate(MLX90640_address, 0x05); 
  Wire.setClock(800000);
}

void loop() {
  
  readTempValues();
 // for (int i=0;i<size;i++)
//{ 
  sendMessage(String(Array_New[i]));
  Serial.print (Array_New[i]); /*Array elements are printed*/
  Serial.print (" "); /*Array elements are printed*/
}
  sendMessage("/n");
  delay(2000);
}

void sendMessage( String message ) {
  udp.beginPacket(udpAddress,udpPort);
  for( int i = 0; i < message.length(); i++ ) {
    udp.write(message[i]);
  }
  udp.endPacket();
}


void Device_Scan() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)  {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
      Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;  }
    else if (error == 4)  {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
      Serial.print("0");
      Serial.println(address, HEX);  }
 }
  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("done"); }
