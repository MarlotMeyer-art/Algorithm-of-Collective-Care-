//Constants
#define number_of_74hc595s 1
#define numOfRegisterPins number_of_74hc595s * 8
#define SER_Pin 25
#define RCLK_Pin 18 //33
#define SRCLK_Pin 5 //32

//Variables
boolean registers [numOfRegisterPins];

void setup(){
  
Serial.begin(115200);

Serial.println(F("Initialize System"));

//Init register
pinMode(SER_Pin, OUTPUT);
pinMode(RCLK_Pin, OUTPUT);
pinMode(SRCLK_Pin, OUTPUT);

clearRegisters();
writeRegisters();
delay(500);

}


void loop(){
writeGrpRelay();

}



void clearRegisters(){     /* function clearRegisters */
for(int i = numOfRegisterPins-1; i >=   0; i--){
  registers[i] = HIGH;//LOW;
}}


void writeRegisters(){       /* function writeRegisters */
digitalWrite(RCLK_Pin, LOW);
 
 for(int i = numOfRegisterPins-1; i >=  0; i--){
  digitalWrite(SRCLK_Pin, LOW); int val = registers[i];
  digitalWrite(SER_Pin, val);
  digitalWrite(SRCLK_Pin, HIGH);
}
  digitalWrite(RCLK_Pin, HIGH);
}


void setRegisterPin(int index,int value){   /* function setRegisterPin */
registers[index] = value;
}


void writeGrpRelay(){/* function writeGrpRelay */
for(int i = numOfRegisterPins-1; i >=   0; i--){

    Serial.print(F("Relay "));Serial.print(i);Serial.println(F(" HIGH"));
    setRegisterPin(i, LOW);
    writeRegisters(); 

    /* THE WIRE NEEDS SOME TIME TO REALLY GET HOT, SO THATS WHY THE DELAY IS SO LONG HERE */ 
    
    delay(10000);  
    
    Serial.print(F("Relay "));Serial.print(i);Serial.println(F(" LOW"));
    setRegisterPin(i, HIGH);
    writeRegisters();
    delay(1000);
          
}
}
