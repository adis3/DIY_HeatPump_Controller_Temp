//Version control
#define VERSION "v1.1.0 started 4/11/2022 released TBD"
String FileName = __FILE__;

#include "Config.h"

/*
 * Define features
 */

//Debug logging
#define DEBUG 0//1

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

#if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
  #include <SerialLCD.h>
  #include <SoftwareSerial.h> //this is a must
  SerialLCD slcd( LCDTxPin,LCDRxPin);
#endif

void setup() {
  
  //SaveToEEPROM();
  //pzem.resetEnergy();
  LoadFromEEPROM();
  Serial.begin(SERIAL_SPEED, SERIAL_PARAM);
  //delay(1000);
  //Serial.println(pS.HeatEnergyMeter,6);
  //delay(10000);
  //pS.HeatEnergyMeter += 19.43; 
  //SaveToEEPROM();
  //Serial.println("saved");
  //delay(100);
  Version();
  
  #if ENVIRONMENT == DEVELOPMENT
    SessionElectricityEnergyMeterStart = 0;
  #endif
  #if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
    float energy = pzem.energy();
    if (!isnan(energy))
      SessionElectricityEnergyMeterStart = energy * 1000;
  #endif
  
  attachInterrupt(digitalPinToInterrupt(FLOW_METER_INPUT_PIN), FlowMeterHandler, RISING);
  
  #if SERIAL_REPORT_INTERVAL != 0
    Serial.println(F("Reporting:"));    
  #endif

  #if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
    slcd.begin();
    slcd.backlight();
  #endif
  #if SERIAL_REPORT_LINES == 0
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
          #if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
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
  
  if (PowerOffFlag && ( millis() > (ThermostatInOffPreviousMillis + (unsigned long) POWER_OFF_DELAY * 60 * 1000) )) {
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
  #if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
    LCDReport();
  #endif
  #if ENVIRONMENT == DEVELOPMENT
    SerialFlowMeterCounter ++;
    LCDFlowMeterCounter ++;
    if (CirculatorPump.State() && (!THI.Error) && (!THO.Error) )
      HeatEnergyMeter += (float)(THO.Temperature-THI.Temperature) / FLOW_METER_IMP_PER_L * WATTS_PER_L_PER_K;
    delay(1000/77);
  #endif
}

void FlowMeterHandler(){
  SerialFlowMeterCounter ++;
  LCDFlowMeterCounter ++;
  if (CirculatorPump.State()){
    if ( (!THI.Error) && (!THO.Error) )
      HeatEnergyMeter += (float)(THO.Temperature-THI.Temperature) / FLOW_METER_IMP_PER_L * WATTS_PER_L_PER_K;
  }
}
