//Version control
#define VERSION "v1.0.0 20/10/2022"
String FileName = __FILE__;

/*
 * Define features
 */

//Debug logging
#define DEBUG 0//1

//Environment
//#define PROD 111//999//
#define ENV 0 //0 - Development, 1 - Testing, 2 - Acceptance, 3 - Production, 4 - Backup

//Includes
//#include <MemoryFree.h>
//#include <Debug.h>
//#include <Messenger.h>
#include <EEPROM.h> // debug de scris versiunea
//#include <OneWire.h>
//#include <EtherCard.h>
//#include <aJSON.h>
//#include <Wire.h>
//#include <Time.h>
//#include <DS1307RTC.h>
#include <Thermometer.h> // debug de scris versiunea
//#include <OneWire.h>
#include <DS18x20.h> // debug de scris versiunea
#include <Thermostat.h> // debug de scris versiunea
#include <InputRegister.h> // debug de scris versiunea
#include <OutputRegister.h> // debug de scris versiunea
//#include <Delay.h>
//#include <Monostable.h>
#include "Config.h"
#include "Init.h"

//includes for VS Code
//#include "EPROM.ino"

//#include "DAvg.h"

#include <PZEM004Tv30.h>
#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#endif

#if !defined(PZEM_SERIAL)
#define PZEM_SERIAL Serial2
#endif
PZEM004Tv30 pzem(PZEM_SERIAL);

#if ENV == 2 or ENV == 3 or ENV == 4
  #include <SerialLCD.h>
  #include <SoftwareSerial.h> //this is a must
  SerialLCD slcd( LCDTxPin,LCDRxPin);
#endif

void setup() {
  
  //SaveToEEPROM();
  //pzem.resetEnergy();
  LoadFromEEPROM();
  Serial.begin(SerialSpeed, SerialParam);
  //delay(1000);
  //Serial.println(pS.HeatEnergyMeter,6);
  //delay(10000);
  //pS.HeatEnergyMeter += 19.43; 
  //SaveToEEPROM();
  //Serial.println("saved");
  //delay(100);
  Version();
  
  #if ENV == 0
    SessionElectricityEnergyMeterStart = 0;
  #endif
  #if ENV == 2 or ENV == 3 or ENV == 4
    float energy = pzem.energy();
    if (!isnan(energy))
      SessionElectricityEnergyMeterStart = energy * 1000;
  #endif
  
  attachInterrupt(digitalPinToInterrupt(FlowMeterInputPin), FlowMeterHandler, RISING);
  
  #if ReportInterval != 0
    Serial.println(F("Reporting:"));    
  #endif

  #if ENV == 2 or ENV == 3 or ENV == 4
    slcd.begin();
    slcd.backlight();
  #endif
  #if ReportLines == 0
    ReportHeader();
  #endif
}

void loop() {
  if ( (ThermostatIn.State()!=ThermostatInPreviousValue) && ( (millis()-ThermostatInDeBounce ) > ThermostatInPreviousMillis)){
    ThermostatInPreviousValue = ThermostatIn.State();
    ThermostatInPreviousMillis = millis();
    if (ThermostatIn.State()){
      PowerOffFlag = false;
      if (!CirculatorPump.State()){
        CirculatorPump.State(true);
        if ( HeatEnergyMeter != 0 ){
          HeatEnergyMeter = 0;
          #if ENV == 2 or ENV == 3 or ENV == 4
            float energy = pzem.energy();
            if (!isnan(energy))
              SessionElectricityEnergyMeterStart = energy * 1000;
          #else
            SessionElectricityEnergyMeterStart = 0;
          #endif
        }
      }
    } else {
      if (!PowerOffFlag){
        ThermostatInOffPreviousMillis = millis();
        PowerOffFlag = true;
        HeatingAgentThermostatOut.State(false);
      }
    }
  }
  
  if (PowerOffFlag && ( millis() > (ThermostatInOffPreviousMillis + (unsigned long) PowerOffDelay * 60 * 1000) )) {
    HeatPump.State(false);
    CirculatorPump.State(false);
    PowerOffFlag = false;
    pS.HeatEnergyMeter += HeatEnergyMeter/1000;
    SaveToEEPROM();    
  }
  
  THO.Read();
  THI.Read();
  TE.Read();
  TRG.Read();
  TRL.Read();
  THE.Read();
  
  if (ThermostatIn.State()){
    HeatingAgentThermostat.Process();
    ProtectionThermostatH.Process();
  }
  Report();
  #if ENV == 2 or ENV == 3 or ENV == 4
    LCDReport();
  #endif
  #if ENV == 0
    SerialFlowMeterCounter ++;
    LCDFlowMeterCounter ++;
    if (CirculatorPump.State() && (!THI.Error) && (!THO.Error) )
      HeatEnergyMeter += (float)(THO.Temperature-THI.Temperature) / FlowMeterImpPerL * WatsPerLPerKelvin;
    delay(1000/77);
  #endif
}

void FlowMeterHandler(){
  SerialFlowMeterCounter ++;
  LCDFlowMeterCounter ++;
  if (CirculatorPump.State()){
    if ( (!THI.Error) && (!THO.Error) )
      HeatEnergyMeter += (float)(THO.Temperature-THI.Temperature) / FlowMeterImpPerL * WatsPerLPerKelvin;
  }
}
