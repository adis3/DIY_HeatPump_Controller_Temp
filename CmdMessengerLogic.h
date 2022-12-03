class CmdMessengerLogic
{
private:
  enum{
    kComandList,
    kSetHeatingAgentTargetTemp,
    kSetHeatingAgentMaxTemp,
    kSeHeatExchangerMinTemp,
    kSetHeatingAgentHysterezis,
    kTestFunction
  };

public:
  static void setup(){
    attachCommandCallbacks();
  }
  static void loop(){
    cmdMessenger.feedinSerialData();
  }
  static void attachCommandCallbacks(){
    cmdMessenger.attach(OnUnknownCommand);
    cmdMessenger.attach(kComandList, OnUnknownCommand);
    cmdMessenger.attach(kSetHeatingAgentTargetTemp, SetHeatingAgentTargetTemp);
    cmdMessenger.attach(kSetHeatingAgentMaxTemp, SetHeatingAgentMaxTemp);
    cmdMessenger.attach(kSeHeatExchangerMinTemp, SeHeatExchangerMinTemp);
    cmdMessenger.attach(kSetHeatingAgentHysterezis, SetHeatingAgentHysterezis);
    cmdMessenger.attach(kTestFunction, TestFunction);
  }
  static void SetHeatingAgentTargetTemp(){
//#if SERIAL_DEBUGGING == 1
    Serial.print(F("CmdMessengerLogic::SetHeatingAgentTargetTemp\t"));
//#endif
    int16_t tt = cmdMessenger.readInt16Arg();
    if ((( HeatingAgentThermostat.Min + HeatingAgentThermostat.Hyst / 2 ) <= tt) && (tt <= ( HeatingAgentThermostat.Max - HeatingAgentThermostat.Hyst / 2 ))){
      PC.HeatingAgentTargetTemp = HeatingAgentThermostat.TT = tt;
      Serial.print(tt);
      Serial.println(F("\tOk"));
      PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
    } else
      Serial.println(F("Parameter error"));
  }
  static void SetHeatingAgentMaxTemp(){
//#if SERIAL_DEBUGGING == 1
    Serial.print(F("CmdMessengerLogic::SetHeatingAgentMaxTemp\t"));
//#endif
    int16_t tt = cmdMessenger.readInt16Arg();
    if ((( HeatingAgentProtectionThermostat.Min + HeatingAgentProtectionThermostat.Hyst / 2 ) <= tt) && (tt <= ( HeatingAgentProtectionThermostat.Max - HeatingAgentProtectionThermostat.Hyst / 2 ))){
      PC.HeatingAgentMaxTemp = HeatingAgentProtectionThermostat.TT = tt;
      Serial.print(tt);
      Serial.println(F("\tOk"));
      PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
    } else
      Serial.println(F("Parameter error"));
  }
  static void SeHeatExchangerMinTemp(){
//#if SERIAL_DEBUGGING == 1
    Serial.print(F("CmdMessengerLogic::SeHeatExchangerMinTemp\t"));
//#endif
    int16_t tt = cmdMessenger.readInt16Arg();
    if ((( HeatExchangerProtectionThermostat.Min + HeatExchangerProtectionThermostat.Hyst / 2 ) <= tt) && (tt <= ( HeatExchangerProtectionThermostat.Max - HeatExchangerProtectionThermostat.Hyst / 2 ))){
      PC.HeatExchangerMinTemp = HeatExchangerProtectionThermostat.TT = tt;
      Serial.print(tt);
      Serial.println(F("\tOk"));
      PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
    } else
      Serial.println(F("Parameter error"));
  }
  static void SetHeatingAgentHysterezis(){
//#if SERIAL_DEBUGGING == 1
    Serial.print(F("CmdMessengerLogic::SetHeatingAgentHysterezis\t"));
//#endif
    int16_t hyst = cmdMessenger.readInt16Arg();
    if ((( HeatingAgentThermostat.Min + hyst / 2 ) <= HeatingAgentThermostat.TT )
      && ( HeatingAgentThermostat.TT <= ( HeatingAgentThermostat.Max - hyst / 2  ))
      && (( HeatingAgentProtectionThermostat.Min + hyst / 2 ) <= HeatingAgentProtectionThermostat.TT )
      && ( HeatingAgentProtectionThermostat.TT <= ( HeatingAgentProtectionThermostat.Max - hyst / 2  ))
      && (( HeatExchangerProtectionThermostat.Min + hyst / 2 ) <= HeatExchangerProtectionThermostat.TT )
      && ( HeatExchangerProtectionThermostat.TT <= ( HeatExchangerProtectionThermostat.Max - hyst / 2  ))){
      PC.HeatingAgentHysterezis = HeatingAgentThermostat.Hyst  = HeatingAgentProtectionThermostat.Hyst  = HeatExchangerProtectionThermostat.Hyst = hyst;
      Serial.print(hyst);
      Serial.println(F("\tOk"));
      PersistentInternalStorage::SaveToEEPROM(INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR, &PC, sizeof(PC));
    } else
      Serial.println(F("Parameter error"));
  }
  static void TestFunction(){
//#if SERIAL_DEBUGGING == 1
    Serial.print(F("CmdMessengerLogic::TestFunction\t"));
//#endif
  }
  static void OnUnknownCommand(){
#if SERIAL_DEBUGGING == 1
    Serial.println(F("CmdMessengerLogic::OnUnknownCommand"));
#endif
    Serial.println(F("Available commands:"));
    Serial.println(F("0 - Command list"));
    Serial.println(F("1 <5...70> - Set heating agent target temperature"));
    Serial.println(F("2 <5...70> - Set heating agent maximum temperature"));
    Serial.println(F("3 <5...70> - Set heat exchanger minimum temperature"));
    Serial.println(F("4 x - Set heating agent hysterezis"));
    Serial.println(F("5 Test function"));
    Serial.println(F("Actual parameters:"));
    Serial.print(F("1 "));
    Serial.println(PC.HeatingAgentTargetTemp);
    Serial.print(F("2 "));
    Serial.println(PC.HeatingAgentMaxTemp);
    Serial.print(F("3 "));
    Serial.println(PC.HeatExchangerMinTemp);
    Serial.print(F("4 "));
    Serial.println(PC.HeatingAgentHysterezis);
  }
};
