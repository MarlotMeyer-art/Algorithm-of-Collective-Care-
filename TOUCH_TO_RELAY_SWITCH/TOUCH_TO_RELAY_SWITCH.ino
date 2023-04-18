#include <NoiselessTouchESP32.h>

#define number_of_74hc595s 1
#define numOfRegisterPins number_of_74hc595s * 8
#define SER_Pin 25
#define RCLK_Pin 18 //33
#define SRCLK_Pin 5 //32

#define TOUCH_HISTORY_LEN 6
#define TOUCH_HYSTERESIS 2

int TOUCH_PIN[] = {T3, T0, T9, T8, T7, T6, T5, T4}; 

boolean registers [numOfRegisterPins];

NoiselessTouchESP32 touchsensor1(TOUCH_PIN[0], TOUCH_HISTORY_LEN, 4);
NoiselessTouchESP32 touchsensor2(TOUCH_PIN[1], TOUCH_HISTORY_LEN, 4);
NoiselessTouchESP32 touchsensor3(TOUCH_PIN[2], TOUCH_HISTORY_LEN, 4);
NoiselessTouchESP32 touchsensor4(TOUCH_PIN[3], TOUCH_HISTORY_LEN, 4);
NoiselessTouchESP32 touchsensor5(TOUCH_PIN[4], TOUCH_HISTORY_LEN, 4);
NoiselessTouchESP32 touchsensor6(TOUCH_PIN[5], TOUCH_HISTORY_LEN, 4);
NoiselessTouchESP32 touchsensor7(TOUCH_PIN[6], TOUCH_HISTORY_LEN, 4);
NoiselessTouchESP32 touchsensor8(TOUCH_PIN[7], TOUCH_HISTORY_LEN, 4);

NoiselessTouchESP32 touchsensor[8] = {touchsensor1,touchsensor2,touchsensor3,touchsensor4,touchsensor5,touchsensor6,touchsensor7,touchsensor8}; //T2? =0

void setup() {
  
Serial.begin(115200);

//Init register
pinMode(SER_Pin, OUTPUT);
pinMode(RCLK_Pin, OUTPUT);
pinMode(SRCLK_Pin, OUTPUT);

clearRegisters();
writeRegisters();
delay(500);

}
  
int counter[8];
int touch[8];
int raw_mean_value[8];
int highest[8] = {0,0,0,0,0,0,0,0};
int lowest[8] = {255,255,255,255,255,255,255,255};


void loop() {

  for (int i=0; i<8; i++){

touch[i] = touchsensor[i].touching();
  raw_mean_value[i] = touchsensor[i].value_from_history();
  highest[i] = _max(highest[i], raw_mean_value[i]);
  lowest[i] = _min(lowest[i], raw_mean_value[i]);
 
  
  if (touch[i] == 1) { 
    Serial.print("touch detected on pin: ");
    Serial.println(i);
    setRegisterPin(i, LOW);
    writeRegisters();

    // can add delay here if you dont want to wait and touch so long

    } 
    else {
    setRegisterPin(i, HIGH);
    writeRegisters();
    }

  

  }



    delay(100);

  
}




void writeRegisters(){       /* function writeRegisters */
//// Write register after being set 

digitalWrite(RCLK_Pin, LOW);
 for(int i = numOfRegisterPins-1; i >=  0; i--){
  digitalWrite(SRCLK_Pin, LOW); int val = registers[i];
  digitalWrite(SER_Pin, val);
  digitalWrite(SRCLK_Pin, HIGH);
}
  digitalWrite(RCLK_Pin, HIGH);
}

void clearRegisters(){     /* function clearRegisters */
//// Clear registers variables 

for(int i = numOfRegisterPins-1; i >=   0; i--){
  registers[i] = HIGH;//LOW;
}}

void setRegisterPin(int index,int value){   /* function setRegisterPin */
////Set register variable to HIGH or LOW
registers[index] = value;
}
