volatile unsigned long SerialFlowMeterCounter = 0, LCDFlowMeterCounter = 0;
volatile double HeatEnergyMeter = 0;
double SessionElectricityEnergyMeterStart = 0;
boolean ThermostatInPreviousValue = false;
unsigned long ThermostatInPreviousMillis = 0;
byte ThermostatInDeBounce = 255;


InputRegister ThermostatIn(ThermostatInputPin, true, true); //
OutputRegister HeatPump(HeatPumpPin, true, false),
  CirculatorPump(CirculatorPumpPin, true, false),
  HeatingAgentThermostatOut(AmbientalNTCOptocouplerPin, true, false);

bool PowerOffFlag = false;
unsigned long ThermostatInOffPreviousMillis;

//DS18x20(byte pBus, byte* pAddr, byte pReadInterval, word pReadDelay, unsigned long pMinMaxClearDelay, unsigned long pMillisOffset);
DS18x20
THO (TPHO.Bus, TPHO.Addr, HeatExchangerReadingInterval, TempSensorReadDelay, TempSensorMinMaxClearDelay, 0, TPHO.Cal),
THI (TPHI.Bus, TPHI.Addr, HeatExchangerReadingInterval, TempSensorReadDelay, TempSensorMinMaxClearDelay, 0, TPHI.Cal),
TRG (TPRG.Bus, TPRG.Addr, HeatExchangerReadingInterval, TempSensorReadDelay, TempSensorMinMaxClearDelay, 0, TPRG.Cal),
TRL (TPRL.Bus, TPRL.Addr, HeatExchangerReadingInterval, TempSensorReadDelay, TempSensorMinMaxClearDelay, 0, TPRL.Cal),
THE (TPHE.Bus, TPHE.Addr, HeatExchangerReadingInterval, TempSensorReadDelay, TempSensorMinMaxClearDelay, 0, TPHE.Cal),
TE (TPE.Bus, TPE.Addr, HeatExchangerReadingInterval, TempSensorReadDelay, TempSensorMinMaxClearDelay, 0, TPE.Cal);
  
//Thermostat(Thermometer * pT, OutputRegister * pBR, float pTT/*Target temperature*/, float pHyst/*Hysterezis; negative for trendUp false*/,
//int pInterval/*Interval between switching*/, int pDelUp, int pDelDown/*Relay command delay*/, int pTTime/*test time at startup*/,
//boolean pHC/*Heating = true, Cooling = false*/, boolean pErrorState/*if Thermometer.Read() returns false then sets output to pErrorState*/, float pMin, float pMax);  
Thermostat HeatingAgentThermostat ( &THO, &HeatingAgentThermostatOut, HeatingAgentTargetTemp, HeatingAgentHisterezis,
    HeatingAgentSwitchingInterval, 0, 0, 0,
    true, false, HeatingAgentMinTargetTemp, HeatingAgentMaxTargetTemp),
  ProtectionThermostatH ( &THO, &HeatPump, HeatingAgentMaxTemp, HeatingAgentHisterezis,
    HeatingAgentSwitchingInterval, 0, 0, 0,
    true, false, HeatingAgentMinTargetTemp, HeatingAgentMaxTargetTemp);
//  ProtectionThermostatL ( &THO, &HeatPump, HeatingAgentMaxTemp, HeatingAgentHisterezis,
//    HeatingAgentSwitchingInterval, 0, 0, 0,
//    true, false, HeatingAgentMinTargetTemp, HeatingAgentMaxTargetTemp);//debug - in lucru


struct PersistentStorage {
  double HeatEnergyMeter;
} 
pS = {0};
