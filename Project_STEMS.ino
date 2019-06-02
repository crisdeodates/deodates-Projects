//-------------------------------------------------------------------------------//
//-- PROJECT STEMS                                                               //
//                                                                               //
//-- PROGRAM GOALS                                                               //
//-- A smart energy monitoring system that will ease the inspection,      			 //
//-- monitoring, analysis, and calculation of energy fare.        		      		 //
//                                                                               //
//-- Programmers: Jiss Joseph Thomas, Cris Thomas                                //
//-- References: WIO LTE                                                         //
//-------------------------------------------------------------------------------//

#include "rgb_lcd.h"
#include "gnss.h"
#include <SD.h>

#define Pin A0
rgb_lcd lcd;
const int chipSelect = 43;
File dataFile;
int sd_write = 0;
const int house_id = 1;
const int averageValue = 10;
int sensorValue = 0;
float sensitivity = 1000.0 / 800.0;
float Vref = 265;
String msg = "";
unsigned int Sensitivity = 185;
float Vpp = 0; // peak-peak voltage 
float Vrms = 0; // rms voltage
float Irms = 0; // rms current
float Supply_Voltage = 233.0;           // reading from DMM
float Vcc = 5.0;         // ADC reference voltage // voltage at 5V pin 
float power = 0;         // power in watt              
float Wh =0 ;             // Energy in kWh
unsigned long last_time =0;
unsigned long current_time =0;
unsigned long interval = 100;
unsigned int calibration = 100;  // V2 slider calibrates this
unsigned int pF = 85;           // Power Factor default 95
float bill_amount = 0;   // 30 day cost as present energy usage incl approx PF 
unsigned int energyTariff = 8.0; // Energy cost in INR per unit (kWh)

void setup() 
{
  Wire.begin();

  gnss.Power_On();
    while(false == gnss.Check_If_Power_On()){
      delay(1000);
    }
    if(!(gnss.open_GNSS())){
      return;
    }
    delay(2000);

    lcd.begin(16, 2);
    delay(1000);

    pinMode(SS, OUTPUT);
    if (SD.begin(chipSelect)) 
    {
      sd_write = 1;
      dataFile = SD.open("STEMS_log.txt", FILE_WRITE);
      dataFile.println("Project STEMS");
      dataFile.println("by Jiss and Cris");
      dataFile.println("");
    }


    lcd.setCursor(0, 0);
    lcd.print("Project STEMS");
    lcd.setCursor(0, 1);
    lcd.print("By Jiss and Cris");

    delay(1000);

    lcd.setCursor(0, 0);
    lcd.print("House ID -->");
    lcd.setCursor(0, 1);
    lcd.print(vehicle_id);
    if(sd_write)
    {
      dataFile.println("House_id: " + house_id);
    }
}

void loop() 
{
  for (int i = 0; i < averageValue; i++)
  {
    sensorValue += analogRead(Pin);
    delay(2);
  }

  sensorValue = sensorValue / averageValue;
  float unitValue= RefVal / 1024.0*1000 ;
  float voltage = unitValue * sensorValue-Vref;
  float current = (voltage - Vref) * sensitivity;

  Vpp = voltage;
  Vrms = (Vpp/2.0) *0.707; 
  Vrms = Vrms - (calibration / 10000.0);
  Irms = (Vrms * 1000)/Sensitivity ;
  if((Irms > -0.015) && (Irms < 0.008))
  {
    Irms = 0.0;
  }
  power= (Supply_Voltage * Irms) * (pF / 100.0); 
  last_time = current_time;
  current_time = millis();    
  Wh = Wh+  power *(( current_time -last_time) /3600000.0) ; // calculating energy in Watt-Hour
  bill_amount = Wh * (energyTariff/1000);

    lcd.setCursor(0, 0);
    lcd.print("Current -->");
    lcd.setCursor(0, 1);
    lcd.print(current + " mA");
    if(sd_write)
    {
      dataFile.println("Current: " + current);
    }

    lcd.setCursor(0, 0);
    lcd.print("Power -->");
    lcd.setCursor(0, 1);
    lcd.print(power + " W");
    if(sd_write)
    {
      dataFile.println("Power: " + power);
    }

    lcd.setCursor(0, 0);
    lcd.print("Bill Amount -->");
    lcd.setCursor(0, 1);
    lcd.print(bill_amount + "INR");
    if(sd_write)
    {
      dataFile.println("Bill Amount: " + bill_amount);
    }

    if(gnss.getCoordinate())
    {
    lon = gnss.longitude;
    lat = gnss.latitude;

    lcd.setCursor(0, 0);
    lcd.print("GNSS : Lat -->");
    lcd.setCursor(0, 1);
    lcd.print(lat);
    if(sd_write)
    {
      dataFile.println("Lat: " + lat);
    }

    lcd.setCursor(0, 0);
    lcd.print("GNSS : Long -->");
    lcd.setCursor(0, 1);
    lcd.print(lon);
    if(sd_write)
    {
      dataFile.println("Long: " + lon);
    }
  } 

  sensorValue = 0;
  delay(1000);
}

