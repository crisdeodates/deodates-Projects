//-------------------------------------------------------------------------------//
//-- SIGFOX PROJECT                                                              //
//                                                                               //
//-- PROGRAM GOALS                                                               //
//-- A smart survey / energy monitoring system that will ease the inspection,    //
//-- monitoring, and analysis.                                                   //
//                                                                               //
//-- Programmers: Cris Thomas, Jiss Joseph Thomas                                //
//-- References: Arduino MKRFOX1200                                              //
//-------------------------------------------------------------------------------//

#include <ArduinoLowPower.h>
#include <SigFox.h>

#define Sensor A6
#define Relay D0
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

typedef struct __attribute__ ((packed)) sigfox_message {
  uint8_t status;
  int16_t houseID;
  int16_t Wh;
  int16_t bill_amnt;
  uint8_t lastMessageStatus;
} SigfoxMessage;

// stub for message which will be sent
SigfoxMessage msg;

void setup() {

  if (oneshot == true) {
    // Wait for the serial
    Serial.begin(115200);
    while (!Serial) {}
  }

  if (!SigFox.begin()) {
    // Something is really wrong, try rebooting
    // Reboot is useful if we are powering the board using an unreliable power source
    // (eg. solar panels or other energy harvesting methods)
    reboot();
  }

  //Send module to standby until we need to send a message
  SigFox.end();

  if (oneshot == true) {
    // Enable debug prints and LED indication if we are testing
    SigFox.debug();
  }

  // Configure the sensors and populate the status field
  if (!bmp.begin()) {
    msg.status |= STATUS_BMP_KO;
  } else {
    Serial.println("BMP OK");
  }

  if (!htu.begin()) {
    msg.status |= STATUS_HTU_KO;
  } else {
    Serial.println("HTU OK");
  }

  if (!tsl.begin()) {
    msg.status |= STATUS_TSL_KO;
  } else {
    Serial.println("TLS OK");
    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
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

  msg.houseID = 1;
  msg.Wh = Wh;
  msg.bill_amnt = bill_amnt;  

   // Start the module
  SigFox.begin();
  // Wait at least 30ms after first configuration (100ms before)
  delay(100);

  // Clears all pending interrupts
  SigFox.status();
  delay(1);

  SigFox.beginPacket();
  SigFox.write((uint8_t*)&msg, 12);

  msg.lastMessageStatus = SigFox.endPacket();

  //Sleep for 15 minutes
  LowPower.sleep(15 * 60 * 1000);  

  sensorValue = 0;
  delay(1000);
}
