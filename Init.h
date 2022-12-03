volatile unsigned long SerialFlowMeterCounter = 0, LCDFlowMeterCounter = 0;
volatile double HeatEnergyMeter = 0;
double SessionElectricityEnergyMeterStart = 0;
boolean ThermostatInPreviousValue = false;
unsigned long ThermostatInPreviousMillis = 0;

struct PersistentConfigurationDT {
    double HeatEnergyMeter;
    byte HeatingAgentTargetTemp;
    byte HeatingAgentMaxTemp;
    byte HeatExchangerMinTemp;
    byte HeatingAgentHysterezis;
    boolean StartUp;
    byte ErrorCode;
    byte WarningCode;
} PC = {0, 32, 38, 10, 6, true, 0, 0};

enum ErrorCodes {
    NoError,
    HeatingAgentProtection,
    HeatExchangerProtection,
  };

enum WarningCodes {
    NoWarning,
    HeatExchangerOneWireBus,
    OusideOneWireBus,
  };

DebouncedContact ThermostatIn( THERMOSTAT_INPUT_PIN, true, true, DEBOUNCE_INTERVAL);

OutputRegister HeatPump(HEAT_PUMP_OUTPUT_PIN, true, false),
  CirculatorPump(CIRCULATOR_PUMP_OUTPUT_PIN, true, false),
  HeatingAgentThermostatOut(AMBIENTAL_NTC_OPTOCOUPLER_OUTPUT_PIN, true, false),
  BackupHeaterOut(BACKUP_HEATER_OUTPUT_PIN, false, false);
unsigned long ThermostatInOffPreviousMillis;

DS18x20
THO (TPHO.Bus, TPHO.Addr, HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPHO.Cal),
THI (TPHI.Bus, TPHI.Addr, HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPHI.Cal),
TRG (TPRG.Bus, TPRG.Addr, HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPRG.Cal),
TRL (TPRL.Bus, TPRL.Addr, HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPRL.Cal),
THE (TPHE.Bus, TPHE.Addr, HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPHE.Cal),
TE (TPE.Bus, TPE.Addr, OUTSIDE_THERMOMETER_READING_INTERVAL, THERMOMETER_READING_DELAY, THERMOMETER_MIN_MAX_CLEAR_INTERVAL, 0, TPE.Cal);
  
Thermostat
  HeatingAgentThermostat ( &THO, &HeatingAgentThermostatOut, PC.HeatingAgentTargetTemp, PC.HeatingAgentHysterezis,
    HEATING_AGENT_SWITCHING_INTERVAL, 0, 0, true,
    true, HEATING_AGENT_TARGET_TEMPERATURE_MIN_LIMIT, HEATING_AGENT_TARGET_TEMPERATURE_MAX_LIMIT),
  HeatingAgentProtectionThermostat ( &THO, &HeatPump, PC.HeatingAgentMaxTemp, PC.HeatingAgentHysterezis,
    HEATING_AGENT_SWITCHING_INTERVAL, 0, 0, true,
    false, HEATING_AGENT_TARGET_TEMPERATURE_MIN_LIMIT, HEATING_AGENT_TARGET_TEMPERATURE_MAX_LIMIT),
  HeatExchangerProtectionThermostat ( &THE, &HeatPump , PC.HeatExchangerMinTemp, PC.HeatingAgentHysterezis,
    HEATING_AGENT_SWITCHING_INTERVAL, 0, 0, false,
    true, HEATING_AGENT_TARGET_TEMPERATURE_MIN_LIMIT, HEATING_AGENT_TARGET_TEMPERATURE_MAX_LIMIT);

CmdMessenger cmdMessenger = CmdMessenger(Serial,' ','\r');
auto circulatorPumpTimer = timer_create_default();
auto energyMetersTimer = timer_create_default();
LED L( LED_PIN, false, LED_STEPS_COUNT, PS, LED_STEP_SETS_COUNT, DSS);
TimedPushButton TPB( BUTTON_INPUT_PIN, true, true, DEBOUNCE_INTERVAL);

