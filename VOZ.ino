/**
  ******************************************************************************
  * @file    VOZ.ino
  * @author  desarrollo de software

  ******************************************************************************

  ******************************************************************************
  * @section  HISTORY
    
    2022/09/05    Initial version.
  */
  
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

/**        
  Connection
  Arduino    VoiceRecognitionModule
   2   ------->     TX
   3   ------->     RX
*/
VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.

uint8_t records[7]; // save record
uint8_t buf[64];

int ledSala = 13;

#define sala   (0)

#define encender (5)
#define apagar (6)

/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf     --> command length
           len     --> number of parameters
*/
void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf  -->  VR module return value when voice is recognized.
             buf[0]  -->  Group mode(FF: None Group, 0x8n: User, 0x0n:System
             buf[1]  -->  number of record which is recognized. 
             buf[2]  -->  Recognizer index(position) value of the recognized record.
             buf[3]  -->  Signature length
             buf[4]~buf[n] --> Signature
*/
void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}

void setup()
{
  /** initialize */
  myVR.begin(9600);
  
  Serial.begin(115200);
  Serial.println("Elechouse Voice Recognition V3 Module\r\nControl LED sample");
  
  pinMode(ledSala, OUTPUT);

  digitalWrite(ledSala, LOW);

    
  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  
  if(myVR.load((uint8_t)sala) >= 0){
    Serial.println("sala loaded");
  }

   if (myVR.load((uint8_t)encender) >= 0) {
 Serial.println("Encender loaded");
 }

 if (myVR.load((uint8_t)apagar) >= 0) {
 Serial.println("Apagar loaded");
 }
  
 
}

void loop()
{
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      
        
        case sala:
        if (digitalRead(ledSala) == 0) {
          digitalWrite(ledSala, HIGH);
          Serial.println("Sala Encendida");
        }
        else {
          digitalWrite(ledSala, LOW);
          Serial.println("Sala Apagada");
        }
        break;


         case encender:

        digitalWrite(ledSala, HIGH);
      

        break;
        
      case apagar:

        digitalWrite(ledSala, LOW);
        
         break;

       
      
      default:
        Serial.println("Record function undefined");
        break;

    }

    
    
    
    /** voice recognized */
    printVR(buf);
  }
}
