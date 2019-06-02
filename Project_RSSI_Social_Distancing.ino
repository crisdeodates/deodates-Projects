//--------------------------------------------------------------------------//
//-- PROJECT RSSI based Social Distancing                        
//                                                                               
//-- PROGRAM GOALS                                                               
//-- 1. Calculate RSSI   
//-- 2. Warn if nearest person is below threshold                               
//                                                                               
//-- Programmers: Jiss Joseph Thomas, Cris Thomas                                
//-- References: Arduino WiFi libraries, RSSI                           
//--------------------------------------------------------------------------//

#include <SPI.h>
#include <WiFiNINA.h>
#include <utility/wifi_drv.h>
#include <Vector.h>

#define LED_R 26
#define LED_G 25
#define LED_B 27
#define thresh_warn 2
#define thresh_critical 1

// Variables
const int SSID_COUNT = 25;
Array<int,SSID_COUNT> rssi_array;
int8_t M_Pwr = -60;
int8_t RSSI_v = 0;
int8_t N = 2;
float Distance = 0.0;

void setup() {

  WiFiDrv::pinMode(LED_R, OUTPUT);
  WiFiDrv::pinMode(LED_G, OUTPUT);
  WiFiDrv::pinMode(LED_B, OUTPUT);
  
  Serial.begin(9600);
  while (!Serial) { ; }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // print the MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void loop() {
  // scan for existing networks:
  Serial.println("Scanning available networks...");
  ScannedNetworks();
  delay(10000);
}

void ScannedNetworks() {
  int num_ssid = WiFi.scanNetworks();
  if (num_ssid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  Serial.print("number of available networks:");
  Serial.println(num_ssid);

  // print the network number and name for each network found:
  for (int i = 0; i < num_ssid; i++) {
    Serial.print(i);
    Serial.print(") ");
    Serial.print(WiFi.SSID(i));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(i));
    Serial.print(" dBm");
  }

  RSSI_v = WiFi.RSSI(i)
  rssi_array.push_back(RSSI_v); // If further processing required

  float ratio = (M_Pwr - RSSI_v)/(10 * N);
  Distance = pow(10,ratio);
  
  if(Distance >= thresh_warn)
  {
    setLEDs(0,255,0);
  }
  else if(Distance < thresh_warn && Distance >= thresh_critical)
  {
    setLEDs(255,255,0);
  }
  else
  {
    setLEDs(255,0,0);
  }
}

void setLEDs(uint8_t red, uint8_t green, uint8_t blue) 
{
  WiFiDrv::analogWrite(LED_R, red);
  WiFiDrv::analogWrite(LED_G, green);
  WiFiDrv::analogWrite(LED_B, blue);
}
