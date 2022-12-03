class StateMachineLogic
{
public:
  static enum HeatPumpStates {
    PowerOff,
    StandBy,
    Heating,
    HeatingNTCLimit,
    ShuttingDown,
    HeatingDefrosting, // debug neimplementat
    Error
  } HeatPumpState;
  static void ChangeLEDState(){
    if (PC.ErrorCode!=(byte)ErrorCodes::NoError){
      L.CurrentStepSet = LED_STEPSET_FOR_ERROR;
      DSS[LED_STEPSET_FOR_ERROR].Count = PC.ErrorCode + 1;
    } else if (PC.WarningCode !=0 ){
      L.CurrentStepSet = LED_STEPSET_FOR_WARNING;
      DSS[LED_STEPSET_FOR_WARNING].Count = PC.WarningCode + 1;
    } else
      L.CurrentStepSet = HeatPumpState;
    L.StartRestart = true;
  }
  static void powerOff(){
#if SERIAL_DEBUGGING == 1
    Serial.println(F("PowerOff"));
#endif
    HeatPumpState = PowerOff;
    ChangeLEDState();
  }
  static void standBy(){
#if SERIAL_DEBUGGING == 1
    Serial.println(F("StandBy"));
#endif
    HeatPumpState = StandBy;
    ChangeLEDState();
  }
  static void heating(){
#if SERIAL_DEBUGGING == 1
    Serial.println("Heating");
#endif
    HeatPumpState = Heating;
    ChangeLEDState();
    circulatorPumpTimer.cancel();
    if (!CirculatorPump.State()){
      CirculatorPump.State(true);
      if (HeatEnergyMeter != 0){
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
  }
  static void heatingNTCLimit(){
#if SERIAL_DEBUGGING == 1
    Serial.println(F("HeatingNTCLimit"));
#endif
    HeatPumpState = HeatingNTCLimit;
    ChangeLEDState();
  }
  static void shuttingDown(){
#if SERIAL_DEBUGGING == 1
    Serial.println(F("ShuttingDown"));
#endif
    HeatPumpState = ShuttingDown;
    ChangeLEDState();
    circulatorPumpTimer.in((unsigned long)POWER_OFF_DELAY * 60 * 1000, circulatorPumpTimerHandler);
    HeatingAgentThermostatOut.State(false);
  }
  static void error(){
#if SERIAL_DEBUGGING == 1
    Serial.println(F("Error"));
#endif
    HeatPumpState = Error;
    ChangeLEDState();
  }
  static void circulatorPumpTimerHandler()
  {
#if SERIAL_DEBUGGING == 1
    Serial.println(F("circulatorPumpTimerHandler"));
#endif
    CirculatorPump.State(false);
    HeatPump.State(false);
    PC.HeatEnergyMeter += HeatEnergyMeter / 1000;
    //HeatEnergyMeter = 0;
    PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
// #if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
//     float energy = pzem.energy();
//     if (!isnan(energy))
//     SessionElectricityEnergyMeterStart = energy * 1000;
// #else
//     SessionElectricityEnergyMeterStart = 0;
// #endif
    if(PC.ErrorCode!=(byte)ErrorCodes::NoError){
      error();
    } else if (PC.StartUp) {
      standBy();
    } else
      powerOff();
  }
  static void heatingDefrosting(){// debug neimplementat
#if SERIAL_DEBUGGING == 1
    Serial.println(F("HeatingDefrosting"));
#endif
    HeatPumpState = HeatingDefrosting;
    ChangeLEDState();
  }
  static void setup() {
    ThermostatIn.OnChange(thermostatInOnChange);
    HeatingAgentThermostat.OnChange(heatingAgentThermostatOnChange);
    HeatingAgentProtectionThermostat.OnChange(heatingAgentProtectionThermostatOnChange);
    HeatExchangerProtectionThermostat.OnChange(heatExchangerProtectionThermostatOnChange);
    TPB.OnChange(buttonOnChange);
    TPB.OnDepress(buttonOnDepress);
    energyMetersTimer.every((unsigned long)AUTO_SAVE_ENERGY_METERS * 60 * 1000, energyMeterSave);

    if (PC.ErrorCode!=(byte)ErrorCodes::NoError)
      error();
    else if (PC.StartUp)
      standBy();
    else
      powerOff();
  }
  static void thermostatInOnChange(bool state){
#if SERIAL_DEBUGGING == 1
    Serial.print(F("thermostatInOnChange::"));
    Serial.println(state ? F("On") : F("Off"));
    Serial.print(F("ThermostatIn.State::"));
    Serial.println(ThermostatIn.State() ? F("On") : F("Off"));
#endif
    if (( HeatPumpState != PowerOff ) && ( HeatPumpState!=Error )){
      if (!state)
        shuttingDown();
    }
  }
  static void heatingAgentThermostatOnChange(bool state){
#if SERIAL_DEBUGGING == 1
    Serial.print(F("HeatingAgentThermostatOnChange::"));
    Serial.println(state ? F("On") : F("Off"));
#endif
    if (state)
      heating();
    else
      heatingNTCLimit();
  }
  static void heatingAgentProtectionThermostatOnChange(bool state){
#if SERIAL_DEBUGGING == 1
    Serial.print(F("HeatingAgentProtectionThermostatOnChange::"));
    Serial.println(state ? F("On") : F("Off"));
#endif
    if((!state)&&(!THO.Error)){
      PC.ErrorCode = ErrorCodes::HeatingAgentProtection ;
      PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
      shuttingDown();
    }
  }
  static void heatExchangerProtectionThermostatOnChange(bool state){
#if SERIAL_DEBUGGING == 1
    Serial.print(F("heatExchangerProtectionThermostatOnChange::"));
    Serial.println(state ? F("On") : F("Off"));
#endif
    if((!state)&&(!THE.Error)){
      PC.ErrorCode = ErrorCodes::HeatExchangerProtection ;
      PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
      shuttingDown();
    }
  }
  static void buttonOnChange(boolean pState){
#if SERIAL_DEBUGGING == 1
    Serial.print(F("ButtonOnChange::"));
    Serial.println(pState ? F("On") : F("Off"));
#endif
    if (pState){
      L.CurrentStepSet = LED_STEPSET_FOR_PUSH_BUTTON_COUNTER;
      L.StartRestart = true;
    }else{
      ChangeLEDState();
    }
  }
  static void buttonOnDepress(unsigned long pDuration){
#if SERIAL_DEBUGGING == 1
    Serial.print(F("ButtonOnDepress::"));
    Serial.println(pDuration);
#endif
    if (pDuration < BUTTON_COMMAND_1_LIMIT * 1000){
#if SERIAL_DEBUGGING == 1
      Serial.println(F("ButtonOnDepress::Command1"));
#endif
    }
    else if (pDuration < BUTTON_COMMAND_2_LIMIT * 1000){
#if SERIAL_DEBUGGING == 1
      Serial.println(F("ButtonOnDepress::Command2::TogglePower"));
#endif
      if ( PC.ErrorCode == (byte)ErrorCodes::NoError ){
        PC.StartUp = (HeatPumpState == PowerOff);
        if (HeatPumpState != PowerOff){
          PC.HeatEnergyMeter += HeatEnergyMeter / 1000;
          HeatEnergyMeter = 0;
#if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
          float energy = pzem.energy();
          if (!isnan(energy))
          SessionElectricityEnergyMeterStart = energy * 1000;
#else
          SessionElectricityEnergyMeterStart = 0;
#endif
        }
        PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
        if (HeatPumpState == PowerOff){
          standBy();
          if (ThermostatIn.State())
            thermostatInOnChange(true);
        }
        else if ((HeatPumpState != ShuttingDown) && (HeatPumpState != StandBy))
            shuttingDown();
        else if ( HeatPumpState == StandBy )
          powerOff();
      }
    } else if (pDuration < BUTTON_COMMAND_3_LIMIT * 1000){
#if SERIAL_DEBUGGING == 1
      Serial.println(F("ButtonOnDepress::Command3"));
#endif
      PC.WarningCode = WarningCodes::NoWarning;
      PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
    } else if (pDuration < BUTTON_COMMAND_4_LIMIT * 1000){
#if SERIAL_DEBUGGING == 1
      Serial.println(F("ButtonOnDepress::Command4"));
#endif
      if (PC.ErrorCode != (byte)ErrorCodes::NoError){
        PC.ErrorCode = ErrorCodes::NoError;
        PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
        if ( HeatPumpState == Error )
          powerOff();
      }
    } else if (pDuration < BUTTON_COMMAND_5_LIMIT * 1000){
#if SERIAL_DEBUGGING == 1
      Serial.println(F("ButtonOnDepress::Command5"));
#endif
    } else if (pDuration < BUTTON_COMMAND_6_LIMIT * 1000){
#if SERIAL_DEBUGGING == 1
      Serial.println(F("ButtonOnDepress::Command6"));
#endif
    } else if (pDuration < BUTTON_COMMAND_7_LIMIT * 1000){
#if SERIAL_DEBUGGING == 1
      Serial.println(F("ButtonOnDepress::Command7::ResetMeters"));
#endif
#if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
      pzem.resetEnergy();
#endif
      PC.HeatEnergyMeter = 0;
      PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
      HeatEnergyMeter = 0;
      SessionElectricityEnergyMeterStart = 0;
    } else {
#if SERIAL_DEBUGGING == 1
      Serial.println(F("ButtonOnDepress::Too long"));
#endif
    }
  }
  static void energyMeterSave(){
#if SERIAL_DEBUGGING == 1
    Serial.println(F("energyMeterSave"));
#endif
    PC.HeatEnergyMeter += HeatEnergyMeter / 1000;
    HeatEnergyMeter = 0;
    PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
#if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
    float energy = pzem.energy();
    if (!isnan(energy))
    SessionElectricityEnergyMeterStart = energy * 1000;
#else
    SessionElectricityEnergyMeterStart = 0;
#endif
  }
  static void loop()
  {
    L.ProcessPWM();
    ThermostatIn.Process();
    TPB.Process();
    energyMetersTimer.tick();
    {
      THO.Read();
      THI.Read();
      TE.Read();
      TRG.Read();
      TRL.Read();
      THE.Read();
    }
    if ( (( THO.Temperature != 0 ) && THO.Error )
      || (( THI.Temperature != 0 ) && THI.Error )
      || (( TRL.Temperature != 0 ) && TRL.Error )
      || (( TRG.Temperature != 0 ) && TRG.Error )
      || (( THE.Temperature != 0 ) && THE.Error )
      || (( TE.Temperature != 0 ) && TE.Error ) ){ // de scos TE in alt if daca il pun pe alt bus
      PC.WarningCode = WarningCodes::HeatExchangerOneWireBus;
    //   PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC)); // am scos scrierea in EPROM ca sa nu depasesc numarul de scrieri
    }
    if (( HeatPumpState == PowerOff ) || ( HeatPumpState == Error ))
      BackupHeaterOut.State(ThermostatIn.State());
    else {
      BackupHeaterOut.State(false);
      circulatorPumpTimer.tick();
      if ( ThermostatIn.State() && PC.StartUp && ( PC.ErrorCode == (byte)ErrorCodes::NoError )){
        HeatingAgentThermostat.Process();
        HeatingAgentProtectionThermostat.Process();
        if (HeatPump.State())
          HeatExchangerProtectionThermostat.Process();
      }
    }
  }
};
