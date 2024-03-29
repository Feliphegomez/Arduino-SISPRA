/*
  #####################################################################################
  #   File:               SISPRA.ino
  #   Processor:          Arduino UNO, MEGA ou Teensy++ 2.0
  #   Language:           Wiring / C / Processing / Fritzing / Arduino IDE
  #    
  #   Objectives:         Watering System - AUTOMATIC
  #     
  #   Author:             Andres Felipe Gomez Maya | FelipheGomez
  #   Date:               04/10/2019
  #   place:              Medellín, Colombia
  #   
  #   Nota: Este proyecto contiene código de dominio público.
  #     La modificación está permitida sin previo aviso.
  #   
  #####################################################################################
*/

// libraries definition
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

// frequency musical notes
#define NOTE_C6  1047
#define NOTE_C3  131
#define NOTE_G3  196

// pins definition
int levelSensorPin = 0;
int moistureSensorPin = 1;
int audioPin = 2;
int soggyLEDPin = 3;
int moistsoilLEDPin = 4;
int drysoilLEDPin = 5;
int pumpLEDPin = 6;
int pumpPin = 7;

// variables
int levelSensorValue;       // stores the level sensor values
int moistureSensorValue;    // stores the moisture sensor values
int j = 0;

// system messages
const char *string_table[] =
{   
  "Bienvenido!  =) ",
  "Tanque sin agua ",
  "Suelo seco      ",
  "Suelo humedo    ",
  "Suelo empapado  ",
  "Bomba agua ENC  ",
  "FelipheGomez    ",
  "Sistema Riego MV",
  "Por favor espera"
};

// objects definition
RTC_DS1307 RTC;
LiquidCrystal_I2C lcd(0x27,16,2);


void setup(){
  // serial initialization
  Serial.begin(9600);
  
  // LCD initialization
  lcd.init();          
  lcd.backlight();     // with Backlight
  lcd.clear();         // clearscreen
  
  // Wire initialization
  Wire.begin();
  
  // RTC initialization
  RTC.begin();
  //RTC.adjust(DateTime(__DATE__, __TIME__));
  if (!RTC.isrunning()){
    // date and time adjust as the PC computer date and time
     
  }
  
  // Arduino pins initalization
  pinMode(audioPin, OUTPUT);
  pinMode(soggyLEDPin, OUTPUT);
  pinMode(moistsoilLEDPin,OUTPUT);
  pinMode(drysoilLEDPin,OUTPUT);
  pinMode(pumpLEDPin,OUTPUT);
  pinMode(pumpPin,OUTPUT);
  
  // LCD initial messages
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(string_table[6]); 
  lcd.setCursor(0,1);
  lcd.print(string_table[7]);
  delay(2000);
  lcd.setCursor(0,1);
  lcd.print(string_table[0]); 
  // initialization delay
  delay(5000); 
}


void loop(){
  // RTC parameters definition
  DateTime myRTC = RTC.now();
  int H = myRTC.hour();
  int M = myRTC.minute();
  int S = myRTC.second();
  // call Clock Function
  //RightHour();
  // reads the sensors
  levelSensorValue = analogRead(levelSensorPin);
  moistureSensorValue = analogRead(moistureSensorPin);
  // if low water level: plays the low level alarm
  if(levelSensorValue < 100){
    // system messages
    lcd.clear();
    RightHour();
    lcd.setCursor(0,0);
    lcd.print(string_table[1]); 
      delay(1000);
    // plays the alarm sound
    for(int i=0;i<2;i++){
      tone(audioPin, NOTE_G3, 200);
      delay(200);
      tone(audioPin, NOTE_C3, 200);
      delay(200);
      noTone(audioPin);
    }
  }
  
  // check the moisture range
  if(moistureSensorValue >= 700){
    // in case of dry soil:
    // system messages
    lcd.clear();
    RightHour();
    lcd.setCursor(0,1);
    lcd.print(string_table[2]);
    // lights up the correct LED
    digitalWrite(drysoilLEDPin,HIGH);
    digitalWrite(moistsoilLEDPin,LOW);
    digitalWrite(soggyLEDPin,LOW);
    // plays the alarm sound
    tone(audioPin, NOTE_C6, 100);
    delay(750);
    noTone(audioPin);
  } else if((moistureSensorValue < 700) && (moistureSensorValue >= 300)){
    // in case of moist soil:
    // system messages
    lcd.clear();
    RightHour();
    lcd.setCursor(0,1);
    lcd.print(string_table[3]);
    // lights up the correct LED
    digitalWrite(drysoilLEDPin,LOW);
    digitalWrite(moistsoilLEDPin,HIGH);
    digitalWrite(soggyLEDPin,LOW);
    delay(750);
  }
  
  if(moistureSensorValue < 300){
    // in case of soggy soil:
    // system messages
    lcd.clear();
    RightHour();
    lcd.setCursor(0,0);
    lcd.print(string_table[4]); 
    // lights up the correct LED
    digitalWrite(drysoilLEDPin,LOW);
    digitalWrite(moistsoilLEDPin,LOW);
    digitalWrite(soggyLEDPin,HIGH);
    delay(650);
  }
  
  // if the soil is dry and if it is the right time: turn on the pump for 1 minute
  //if((H == 16) && (M == 50) && (S == 00)){} // Hora exacta
  
  while(moistureSensorValue >= 700){
    // system messages
    RightHour();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(string_table[8]);
    lcd.setCursor(0,1);
    lcd.print(string_table[5]);
    // turn the pump on
    digitalWrite(pumpPin,HIGH);
    digitalWrite(pumpLEDPin,HIGH);
    delay(1000);
    // if the soil is not moist so far
    // reads the moisture sensor once more
    moistureSensorValue = analogRead(moistureSensorPin);
  }
  // turn the pump off
  digitalWrite(pumpPin,LOW);
  digitalWrite(pumpLEDPin,LOW);
  Serial.println(moistureSensorValue);
}

// Real Time Clock Function
void RightHour()
{
  DateTime Now = RTC.now();
  String clock_date = "D: ";
  String clock_hour = "H: ";
  
  int _day = Now.day();
  int _month = Now.month();
  int _year = Now.year();
  
  clock_date += fixZero(_day);
  clock_date += "/";
  clock_date += fixZero(_month);
  clock_date += "/";
  clock_date += _year;
  
  int _hour = Now.hour();
  int _minute = Now.minute();
  int _second = Now.second();
  
  clock_hour += fixZero(_hour);
  clock_hour += ":";
  clock_hour += fixZero(_minute);
  clock_hour += ":";
  clock_hour += fixZero(_second);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(clock_date);
  lcd.setCursor(0, 1);
  lcd.print(clock_hour);
  
  delay(1000);
}


String fixZero(int i)
{
  String ret;
  if (i < 10) ret += "0";
  ret += i;
  return ret;
}
