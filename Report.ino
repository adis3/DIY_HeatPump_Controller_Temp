unsigned long ReportPreviousMillis = millis() - (unsigned long)ReportInterval * 1000 * 60;
byte ReportLinesCounter = 0;

void ReportHeader () {
  Serial.print(F("Min.\t"));
  Serial.print(F("THO(C)\tTHI(C)\tTE(C)\t"));
  #if ENV != 0
    Serial.print(F("TRG(C)\tTRL(C)\t"));
    Serial.print(F("THE(C)\t"));
  #endif
  Serial.print(F("Tin\tNTC\tHP\tCP\tF(Hz)\tF(l/m)\tPh(W)\tPe(W)\tCOP1\tEh(Wh)\tEe(Wh)\tCOP2\tEhs(kWh)\tEes(kWh)\tCOP3"));
  Serial.println();
}

void Report () {
  if (( ReportInterval != 0 ) && ( millis() - ReportPreviousMillis > (unsigned long)ReportInterval * 1000 * 60)) {
    ReportPreviousMillis = millis();
    #if ReportLines != 0
      if ( ReportLinesCounter == 0 )
        ReportHeader();
      ReportLinesCounter ++;
      ReportLinesCounter = ReportLinesCounter % ReportLines;
    #endif
    Serial.print(millis() / 1000 / 60);
    Serial.print(F("\t"));
    if (!THO.Error)
      Serial.print(THO.Temperature,1);
    else
      Serial.print(F("----"));
    Serial.print(F("\t"));
    if (!THI.Error)
      Serial.print(THI.Temperature,1);
    else
      Serial.print(F("----"));
    Serial.print(F("\t"));
    if (!TE.Error)
      Serial.print(TE.Temperature,1);
    else
      Serial.print(F("----"));
    Serial.print(F("\t"));
    #if ENV != 0
      if (!TRG.Error)
        Serial.print(TRG.Temperature,1);
      else
        Serial.print(F("----"));
      Serial.print(F("\t"));
      if (!TRL.Error)
        Serial.print(TRL.Temperature,1);
      else
        Serial.print(F("----"));
      Serial.print(F("\t"));
      if (!THE.Error)
        Serial.print(THE.Temperature,1);
      else
        Serial.print(F("----"));
      Serial.print(F("\t"));
    #endif
    Serial.print(ThermostatIn.State());
    Serial.print(F("\t"));
    Serial.print(HeatingAgentThermostatOut.State());
    Serial.print(F("\t"));
    Serial.print(HeatPump.State()); //debug probabil ca voi schimba daca exista si o limita de jos
    Serial.print(F("\t"));
    Serial.print(CirculatorPump.State());
    Serial.print(F("\t"));
    double Freq = (double)SerialFlowMeterCounter / 60 / ReportInterval;
    SerialFlowMeterCounter = 0;
    Serial.print(Freq,1);
    Serial.print(F("\t"));
    double Flow = Freq * 60 / FlowMeterImpPerL;
    Serial.print(Flow,1);
    Serial.print(F("\t"));
    long HeatingPower = 0;
    if ( (!THI.Error) && (!THO.Error) ){
      HeatingPower = (double)(THO.Temperature - THI.Temperature) * Flow * WatsPerLPerKelvin * 60;
      Serial.print(HeatingPower);
    } else
      Serial.print("----");
    Serial.print(F("\t"));
    #if ENV == 0
      int power = 1234;
    #endif
    #if ENV == 2 or ENV == 3 or ENV == 4
      int power = pzem.power();
    #endif
    if (isnan(power))
      Serial.print(F("Err\tErr"));
    else {
      Serial.print(power);
      Serial.print(F("\t"));
      if (power != 0)
        Serial.print((float)HeatingPower / power);
      else
        Serial.print(F("----"));
    }
    Serial.print(F("\t"));
    Serial.print((long)HeatEnergyMeter);
    Serial.print(F("\t"));
    #if ENV == 0
      float energy = HeatEnergyMeter / 4 / 1000;
    #endif
    #if ENV == 2 or ENV == 3 or ENV == 4
      float energy = pzem.energy();
    #endif
    if (isnan(energy))
      Serial.print(F("Err\tErr"));
    else {
      Serial.print((long)(energy * 1000 - SessionElectricityEnergyMeterStart));
      Serial.print(F("\t"));
      if ((energy * 1000 - SessionElectricityEnergyMeterStart) != 0)
        Serial.print(HeatEnergyMeter / (energy * 1000 - SessionElectricityEnergyMeterStart));
      else
        Serial.print(F("----"));
    }
    Serial.print(F("\t"));
    if (CirculatorPump.State())
      Serial.print(pS.HeatEnergyMeter + HeatEnergyMeter / 1000);
    else
      Serial.print(pS.HeatEnergyMeter);
    Serial.print(F("\t"));
    if (isnan(energy))
      Serial.print(F("Err\tErr"));
    else {
      Serial.print(energy);
      Serial.print(F("\t"));
      if ((energy) != 0)
        Serial.print((pS.HeatEnergyMeter + (CirculatorPump.State()?HeatEnergyMeter / 1000:0) ) / energy);
      else
        Serial.print(F("----"));
    }
    Serial.print(F("\t"));
    Serial.println();
  }
}
