// To Do versiunea curenta
// logare pe USB la : 2 protectii termostate, schimbari de stare, raport periodic, erori termometre
// de facut partea de comenzi si raportare pe bluetooth
// salvare in eprom la max 1h
// cop instant 1 / 3 / 6 / 12 / 24 h / sesiune
// procent de functionare luand in considerare termostatul ntc
// de facut logarea pe usb, integrat si rtc si salvarea in eprom extern
// epromul extern trebuie sa contina parametri de corectie a ceasului
// de integrat lcd i2c si combinat cu functiile butonului
// lcd i2c si serial lcd trebuie sa se comporte la fel
// Cu Arduino nealimentat, cele 2 relee trebuie sa conduca si un NTC trebuie sa fie cuplat pentru limita temperatura pe tur
// Daca dupa x (5 minute) de la pornirea pompei de caldura  nu apare diferenta de temperatura pe refrigerant, se opreste pompa de caldura si se porneste centrala. Se va activa un cod de eroare
// Daca temperatura nu creste suficient de repede sau cop dupa x minute nu depaseste 2 (config) se va trece pe backup cu un cod de alerta

// DONE
// de terminat salvarea in EEPROM intern - de verificat ca la diferite adrese pot pune diverse configuratii si pus in functiile de eprom info cu privire la salvare
// de facut partea de comenzi pe seriala
// de integrat debounce cu intrarea de termostat
// de modificat termostatele ca sa faca un callback
// Pe power off transfer thermostat input to backup
// de facut masina de stari
// integrat led cu masina de stari
// de integrat butonul cu functie de schimbare ecran, oprire, pornire si reset contoare
// de vazut care este dimensiunea PC si ajustat adresele de salvare
// de adaugat protectie la supraincalzire cu semnalizare vizuala eroare
// de adaugat protectie la subracire cu semnalizare vizuala eroare
// de salvat contor caldura imediat dupa ce apas shutdown.
// de adaugat stare de eroare in care Pompa de caldura este definitiv oprita (si pompa de recirculare merge pentru intervalul deja definit) si se trece pe backup si indicator de alerta in care pompa functioneaza, dar exista o indicatie vizuala

// Versiunea urmatoare
// integrare web service automatizare pardoseala pentru a manipula temperaturile

// POSTPONED
// mutat persistent internal storage din proiect in librarii, scris un exemplu si urcat pe github
// de pus in masina de stari defrosting
// Version control

#define VERSION "v1.1.0 started 4/11/2022 released TBD"
String FileName = __FILE__;
// Debug logging
#define SERIAL_DEBUGGING 1
// Reset persistent configuration. Only with SERIAL_DEBUGGING 1
#define RESET_PERSISTENT_CONFIGURATION 0
// preeset persistent configuration. Only with SERIAL_DEBUGGING 1
#define PRESET_PERSISTENT_CONFIGURATION 0
// Reset PZEM. Only with SERIAL_DEBUGGING 1
#define RESET_PZEM 0

//#include <Arduino.h>

#include <LED.h>
#include <TimedPushButton.h>
#include "Config.h"
#include "Version.h"
#include "PersistentInternalStorage.h"
//#include "LCD.ino" // debug de facut o clasa

/*
 * Define features
 */

// Includes
//#include <MemoryFree.h>
//#include <Debug.h>
//#include <Messenger.h>

//#include <OneWire.h>
//#include <EtherCard.h>
//#include <aJSON.h>
//#include <Wire.h>
//#include <Time.h>
//#include <DS1307RTC.h>
#include <Thermometer.h> // debug de scris versiunea
//#include <OneWire.h>
#include <DS18x20.h>        // debug de scris versiunea
#include <Thermostat.h>     // debug de scris versiunea
#include <InputRegister.h>  // debug de scris versiunea
#include <OutputRegister.h> // debug de scris versiunea
//#include <Delay.h>
//#include <Monostable.h>
#include <CmdMessenger.h>  // CmdMessenger
#include <DebouncedContact.h>
#include <arduino-timer.h>
#include "Init.h"

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

#include "Reporting.h"
unsigned long Reporting::SerialReportPreviousMillis;
byte Reporting::SerialReportLinesCounter;
#include "CmdMessengerLogic.h"
#include "StateMachineLogic.h"
StateMachineLogic::HeatPumpStates StateMachineLogic::HeatPumpState = PowerOff;

#if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
#include <SerialLCD.h>
#include <SoftwareSerial.h> //this is a must
SerialLCD slcd(LCDTxPin, LCDRxPin);
#endif

void setup()
{
#if SERIAL_DEBUGGING == 1
  Reporting::SerialReportSetup();
#if RESET_PERSISTENT_CONFIGURATION == 1
  PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
#endif
#if PRESET_PERSISTENT_CONFIGURATION == 1
  PC.HeatEnergyMeter = 986.43;
  PC.StartUp = true;
  PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
#endif
#if RESET_PZEM == 1
  pzem.resetEnergy();
#endif
  PersistentInternalStorage::LoadFromEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
  {
    Serial.print(F("Persistent configuration\t"));
    Serial.print(F("HeatEnergyMeter\t"));
    Serial.print(PC.HeatEnergyMeter, 4);
    Serial.print(F("\tHeatingAgentTargetTemp\t"));
    Serial.print(PC.HeatingAgentTargetTemp);
    Serial.print(F("\tHeatingAgentMaxTemp\t"));
    Serial.print(PC.HeatingAgentMaxTemp);
    Serial.print(F("\tHeatExchangerMinTemp\t"));
    Serial.println(PC.HeatExchangerMinTemp);
    Serial.print(F("\tHeatingAgentHysterezis\t"));
    Serial.print(PC.HeatingAgentHysterezis);
    Serial.print(F("\tStartUp\t"));
    Serial.print((PC.StartUp ? F("On") : F("Off")));
    Serial.print(F("\tErrorCode\t"));
    Serial.print(PC.ErrorCode);
    Serial.print(F("\tWarningCode\t"));
    Serial.println(PC.WarningCode);
  }
#else
  PersistentInternalStorage::LoadFromEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
  Reporting::SerialReportSetup();
#endif
CmdMessengerLogic::OnUnknownCommand();
HeatingAgentThermostat.TT = PC.HeatingAgentTargetTemp;
HeatingAgentThermostat.Hyst = PC.HeatingAgentHysterezis;
HeatingAgentProtectionThermostat.TT = PC.HeatingAgentMaxTemp;
HeatingAgentProtectionThermostat.Hyst = PC.HeatingAgentHysterezis;
HeatExchangerProtectionThermostat.TT = PC.HeatExchangerMinTemp;
HeatExchangerProtectionThermostat.Hyst = PC.HeatingAgentHysterezis;
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
  Reporting::SerialReportHeader();
#endif
  StateMachineLogic::setup();
  CmdMessengerLogic::setup();
}

void loop()
{
  StateMachineLogic::loop();
  Reporting::SerialReport();

#if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
  LCDReport();
#endif
  CmdMessengerLogic::loop();
#if ENVIRONMENT == DEVELOPMENT
  //debug test data
  if (CirculatorPump.State()){
    SerialFlowMeterCounter++;
    LCDFlowMeterCounter++;
    if ((!THI.Error) && (!THO.Error))
      HeatEnergyMeter += (float)(THO.Temperature - THI.Temperature) / FLOW_METER_IMP_PER_L * WATTS_PER_L_PER_K;
    delay(1000 / 77);
  }
#endif
}

void FlowMeterHandler(){
  SerialFlowMeterCounter++;
  LCDFlowMeterCounter++;
  if (CirculatorPump.State())
  {
    if ((!THI.Error) && (!THO.Error))
      HeatEnergyMeter += (float)(THO.Temperature - THI.Temperature) / FLOW_METER_IMP_PER_L * WATTS_PER_L_PER_K;
  }
}
