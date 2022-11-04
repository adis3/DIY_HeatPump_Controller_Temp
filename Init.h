volatile unsigned long SerialFlowMeterCounter = 0, LCDFlowMeterCounter = 0;
volatile double HeatEnergyMeter = 0;
double SessionElectricityEnergyMeterStart = 0;
boolean ThermostatInPreviousValue = false;
unsigned long ThermostatInPreviousMillis = 0;
byte ThermostatInDeBounce = 255;


InputRegister ThermostatIn(THERMOSTAT_INPUT_PIN, true, true); //
OutputRegister HeatPump(HEAT_PUMP_OUTPUT_PIN, true, false),
  CirculatorPump(CIRCULATOR_PUMP_OUTPUT_PIN, true, false),
  HeatingAgentThermostatOut(AMBIENTAL_NTC_OPTOCOUPLER_OUTPUT_PIN, true, false);

bool PowerOffFlag = false;
unsigned long ThermostatInOffPreviousMillis;

//DS18x20(byte pBus, byte* pAddr, byte pReadInterval, word pReadDelay, unsigned long pMinMaxClearDelay, unsigned long pMillisOffset);
DS18x20
THO (TPHO.Bus, TPHO.Addr, HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPHO.Cal),
THI (TPHI.Bus, TPHI.Addr, HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPHI.Cal),
TRG (TPRG.Bus, TPRG.Addr, HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPRG.Cal),
TRL (TPRL.Bus, TPRL.Addr, HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPRL.Cal),
THE (TPHE.Bus, TPHE.Addr, HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPHE.Cal),
TE (TPE.Bus, TPE.Addr, OUTSIDE_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPE.Cal);
  
//Thermostat(Thermometer * pT, OutputRegister * pBR, float pTT/*Target temperature*/, float pHyst/*Hysterezis; negative for trendUp false*/,
//int pInterval/*Interval between switching*/, int pDelUp, int pDelDown/*Relay command delay*/, int pTTime/*test time at startup*/,
//boolean pHC/*Heating = true, Cooling = false*/, boolean pErrorState/*if Thermometer.Read() returns false then sets output to pErrorState*/, float pMin, float pMax);  
Thermostat HeatingAgentThermostat ( &THO, &HeatingAgentThermostatOut, HEATING_AGENT_TARGET_TEMPERATURE, HEATING_AGENT_HISTEREZIS,
    HEATING_AGENT_SWITCHING_INTERVAL, 0, 0, 0,
    true, false, HEATING_AGENT_TARGET_TEMPERATURE_MIN_LIMIT, HEATING_AGENT_TARGET_TEMPERATURE_MAX_LIMIT),
  ProtectionThermostatH ( &THO, &HeatPump, HEATING_AGENT_MAX_TEMPERATURE, HEATING_AGENT_HISTEREZIS,
    HEATING_AGENT_SWITCHING_INTERVAL, 0, 0, 0,
    true, false, HEATING_AGENT_TARGET_TEMPERATURE_MIN_LIMIT, HEATING_AGENT_TARGET_TEMPERATURE_MAX_LIMIT);
//  ProtectionThermostatL ( &THO, &HeatPump, HEATING_AGENT_MAX_TEMPERATURE, HEATING_AGENT_HISTEREZIS,
//    HEATING_AGENT_SWITCHING_INTERVAL, 0, 0, 0,
//    true, false, HEATING_AGENT_TARGET_TEMPERATURE_MIN_LIMIT, HEATING_AGENT_TARGET_TEMPERATURE_MAX_LIMIT);//debug - in lucru


struct PersistentStorage {
  double HeatEnergyMeter;
} 
pS = {0};
