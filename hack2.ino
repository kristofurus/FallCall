#include<Wire.h>
#include "AlertMe.h"

//WIFI MODULE
AlertMe alert;
uint8_t config_pin = 12;
String to_email = "krzysztof.ladaa@gmail.com";

String subject_line = "I can't stand up! Please help!";
String message = "I have fallen and can't stand up!\n Please come help me or call the doctor\n.";

//GYROSCOPE + ACCELEROMETER
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
float ax=0, ay=0, az=0, gx=0, gy=0, gz=0;

boolean fall = false; //stores if a fall has occurred
boolean trigger1=false; //stores if first trigger (lower threshold) has occurred
boolean trigger2=false; //stores if second trigger (upper threshold) has occurred
boolean trigger3=false; //stores if third trigger (orientation change) has occurred

byte trigger1count=0; //stores the counts past since trigger 1 was set true
byte trigger2count=0; //stores the counts past since trigger 2 was set true
byte trigger3count=0; //stores the counts past since trigger 3 was set true
int angleChange=0;

//BUZZER ALARM

boolean alarm = false;
int button_count = 0;

//PINS

#define BUTTON 4
#define BUZZER 12

void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.begin(9600);

  pinMode(config_pin,INPUT_PULLUP);
  if(config_pin == LOW)
  {
    alert.config();
  }
    alert.connect();

  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
}
void loop()
{  
  mpu_read();
  ax = (AcX-2050)/16384.00;
  ay = (AcY-77)/16384.00;
  az = (AcZ-1947)/16384.00;

  gx = (GyX+270)/131.07;
  gy = (GyY-351)/131.07;
  gz = (GyZ+136)/131.07;

  float Raw_AM = pow(pow(ax,2)+pow(ay,2)+pow(az,2),0.5);
  int AM = Raw_AM * 10;
  
  //Serial.println(AM);
  if (trigger3==true)
  {
     trigger3count++;
     if (trigger3count>=15)
     { 
        angleChange = pow(pow(gx,2)+pow(gy,2)+pow(gz,2),0.5);
        if ((angleChange>=0) && (angleChange<=15))
        {
          fall=true; trigger3=false; trigger3count=0;
        }
        else
        {
           trigger3=false; trigger3count=0;
        }
      }
   }
  if (fall==true)
  {
    
    digitalWrite(BUZZER, HIGH);
    alarm = true;
    for(int i = 0; i < 100; i++)
    {
      if(digitalRead(BUTTON)==0)
      {
        button_count++;
        Serial.println(button_count);
        delay(100);
        if(button_count == 2)
        {
          digitalWrite(BUZZER, LOW);  
          alarm = false;
          button_count = 0;
          break; 
        }
      }
      delay(100);
    }
    digitalWrite(BUZZER, LOW);
    if(alarm == true)
    {
      alert.send(subject_line, message, to_email);
      Serial.println("ALARM ALARM t3");
    }
    alarm = false;
    button_count = 0;   
    fall=false;
  }
  if (trigger2==true)
  {
    trigger2count++;
    angleChange = pow(pow(gx,2)+pow(gy,2)+pow(gz,2),0.5); //Serial.println(angleChange);
    if (angleChange>=30 && angleChange<=400)
    {
      trigger3=true; trigger2=false; trigger2count=0;
      digitalWrite(BUZZER, HIGH);
      alarm = true;
      for(int i = 0; i < 100; i++)
      {
        if(digitalRead(BUTTON)==0)
        {
          button_count++;
          Serial.println(button_count);
          delay(100);
          if(button_count == 2)
          {
            digitalWrite(BUZZER, LOW);  
            alarm = false;
            button_count = 0;
            break; 
          }
        }
        delay(100);
      }
      digitalWrite(BUZZER, LOW);
       if(alarm == true)
      {
        alert.send(subject_line, message, to_email);
        Serial.println("ALARM ALARM t2");
      }
      alarm = false;
      button_count = 0;   
    }
  }
  if (trigger1==true)
  {
    trigger1count++;
    if (AM>=12)
    {
      trigger2=true;
      trigger1=false; trigger1count=0;
      digitalWrite(BUZZER, HIGH);
      alarm = true;
      for(int i = 0; i < 100; i++)
      {
        if(digitalRead(BUTTON)==0)
        {
          button_count++;
          Serial.println(button_count);
          delay(100);
          if(button_count == 2)
          {
            digitalWrite(BUZZER, LOW);  
            alarm = false;
            button_count = 0;
            break; 
          }
        }
        delay(100);
      }
      digitalWrite(BUZZER, LOW);
      if(alarm == true)
      {
        alert.send(subject_line, message, to_email);
        Serial.println("ALARM ALARM t1");
      }
      alarm = false;
      button_count = 0;   
    }
  }
  if (AM<=2 && trigger2==false)
  {
    trigger1=true;
    digitalWrite(BUZZER, HIGH);
    alarm = true;
    for(int i = 0; i < 100; i++)
    {
      if(digitalRead(BUTTON)==0)
      {
        button_count++;
        Serial.println(button_count);
        delay(100);
        if(button_count == 2)
        {
          digitalWrite(BUZZER, LOW);  
          alarm = false;
          button_count = 0;
          break; 
        }
      }
      delay(100);
    }
    digitalWrite(BUZZER, LOW);
    if(alarm == true)
      {
        alert.send(subject_line, message, to_email);
        Serial.println("ALARM ALARM unknown");
      }
    alarm = false;
    button_count = 0;   
  }
  delay(100);
}

void mpu_read(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  }
