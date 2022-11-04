#if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP

unsigned long LCDPreviousMillis = millis() - (unsigned long)LCD_REPORT_INTERVAL * 1000;

void LCDReport () {
  if (( LCD_REPORT_INTERVAL != 0 ) && ( millis() - LCDPreviousMillis > (unsigned long)LCD_REPORT_INTERVAL * 1000)) {
    LCDPreviousMillis = millis();
    slcd.clear();
    slcd.setCursor(0,0);
    slcd.print((ThermostatIn.State()?"1":"0"));
    slcd.setCursor(2,0);
    if (!THO.Error)
      slcd.print(THO.Temperature,0);
    else
      slcd.print("--");
    slcd.setCursor(5,0);
    if (!TE.Error){
      if (TE.Temperature<0) slcd.print("-");
      slcd.print(abs(TE.Temperature),0);
    } else
      slcd.print("---");
    slcd.setCursor(9,0);

    double Freq = (double)LCDFlowMeterCounter / LCD_REPORT_INTERVAL;
    LCDFlowMeterCounter = 0;
    double Flow = Freq * 60 / FLOW_METER_IMP_PER_L;
    long HeatingPower = 0;
    if ( (!THI.Error) && (!THO.Error) )
      HeatingPower = (double)(THO.Temperature - THI.Temperature) * Flow * WATTS_PER_L_PER_K * 60;
    // #if ENV == 0
    //   int power = 1234;
    // #endif
    // #if ENV == 2 or ENV == 3 or ENV == 4
      int power = pzem.power();
    // #endif
    if (isnan(power))
      slcd.print("---");
    else {
      if (power != 0)
        slcd.print((float)HeatingPower / power, 1);
      else
        slcd.print("---");
    }

    slcd.setCursor(13,0);

    // #if ENV == 0
    //   float energy = HeatEnergyMeter / 4 / 1000;
    // #endif
    // #if ENV == 2 or ENV == 3 or ENV == 4
      float energy = pzem.energy();
    // #endif
    if (isnan(energy))
      slcd.print("---");
    else {
      if ((energy * 1000 - SessionElectricityEnergyMeterStart) != 0)
        slcd.print((float) (HeatEnergyMeter / (energy * 1000 - SessionElectricityEnergyMeterStart) ), 1);
      else
        slcd.print("---");
    }

    slcd.setCursor(0,1);

    slcd.print((HeatingAgentThermostatOut.State()?"1":"0"));


    slcd.setCursor(2,1);

    if (isnan(power))
      slcd.print("----");
    else 
      slcd.print((float)power,0);

    slcd.setCursor(7,1);

    if (isnan(energy))
      slcd.print("----- ---");
    else {
      slcd.print((float)energy,0);
      slcd.setCursor(13,1);
      if ((energy) != 0)
        slcd.print((float)((pS.HeatEnergyMeter + (CirculatorPump.State()?HeatEnergyMeter / 1000:0) ) / energy),1);
      else
        slcd.print("---");
    }
      }
}

#endif
