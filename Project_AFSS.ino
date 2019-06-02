//-----------------------------------------------------------------------------#
//-- PROJECT AFSS                                                              #
//                                                                             #
//-- PROGRAM GOALS                                                             #
//-- 1. Detect Battery Anomaly                                                 #
//-- 2. Activate Suppression system                                            #
//                                                                             #
//-- Programmers: Cris Thomas, Jiss Joseph Thomas                              #
//-- References: Hackster-Powering the Future with Infineon                    #
//-----------------------------------------------------------------------------#

#include <LiquidCrystal.h>
#include <Servo.h>

const int flexPin = 7;
const int TempPin = A0;
const int trigger = 9;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int servoPin = 6;

int TempValue;
Servo servo;

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  servo.attach(servoPin);

  pinMode(flexPin, INPUT);
  pinMode(TempPin, INPUT);
  pinMode(trigger, OUTPUT);

  digitalWrite(trigger,LOW);
}

void loop()
{
  int flexValue;
  flexValue = digitalRead(flexPin);             // Getting Flex value and saving it in variable
  Serial.print("sensor: ");
  Serial.println(flexValue);

  TempValue = analogRead(TempPin);              // Getting LM35 value and saving it in variable
  float TempCel = ( TempValue / 1024.0) * 500;  // Getting the celsius value from 10 bit analog value

  if (flexValue == HIGH)
  {
    lcd.setCursor(0, 0);
    lcd.print("Flex detected");
    digitalWrite(trigger,HIGH);
    servo.write(170);
    delay(5);
    while(1)
    {
      //Permanent Blocking
    }
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("No Flex         ");
    digitalWrite(trigger,LOW);
  }
  if (TempCel > 50)
  {
    lcd.setCursor(0, 1);
    lcd.print("Temp detected");
    digitalWrite(trigger,HIGH);
    servo.write(170);
    delay(5);
    while(1)
    {
      //Permanent Blocking
    }
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("Temp Normal     ");
    digitalWrite(trigger,LOW);
  }
}
