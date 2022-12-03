class Reporting
{
public:
    static unsigned long SerialReportPreviousMillis;
    static byte SerialReportLinesCounter;
    static void SerialReportSetup(){
        Serial.begin(SERIAL_SPEED, SERIAL_PARAM);
        Version::Print();
        SerialReportPreviousMillis = (unsigned long)0 - (unsigned long) SERIAL_REPORT_INTERVAL * 1000 * 60;
        SerialReportLinesCounter = 0;
    }
    static void SerialReportHeader()
    {
        Serial.print(F("Min.\t"));
        Serial.print(F("THO(C)\tTHI(C)\tTE(C)\t"));
#if ENVIRONMENT != DEVELOPMENT
        Serial.print(F("TRG(C)\tTRL(C)\t"));
        Serial.print(F("THE(C)\t"));
#endif
        Serial.print(F("Tin\tNTC\tHP\tCP\tF(Hz)\tF(l/m)\tPh(W)\tPe(W)\tCOP1\tEh(Wh)\tEe(Wh)\tCOP2\tEhs(kWh)\tEes(kWh)\tCOP3"));
        Serial.println();
    }
    static void SerialReport()
    {
        if ((SERIAL_REPORT_INTERVAL != 0) && (millis() - SerialReportPreviousMillis > (unsigned long)SERIAL_REPORT_INTERVAL * 1000 * 60 ))
        {
            //extern DS18x20 THO, THI, TE, TRG, TRL, THE;
            SerialReportPreviousMillis = millis();
#if SERIAL_REPORT_LINES != 0
            if (SerialReportLinesCounter == 0)
                Reporting::SerialReportHeader();// ReportHeader();
            SerialReportLinesCounter++;
            SerialReportLinesCounter = SerialReportLinesCounter % SERIAL_REPORT_LINES;
#endif
            Serial.print(millis() / 1000 / 60);
            Serial.print(F("\t"));
            if (!THO.Error)
                Serial.print(THO.Temperature, 1);
            else
                Serial.print(F("----"));
            Serial.print(F("\t"));
            if (!THI.Error)
                Serial.print(THI.Temperature, 1);
            else
                Serial.print(F("----"));
            Serial.print(F("\t"));
            if (!TE.Error)
                Serial.print(TE.Temperature, 1);
            else
                Serial.print(F("----"));
            Serial.print(F("\t"));
#if ENVIRONMENT != DEVELOPMENT
            if (!TRG.Error)
                Serial.print(TRG.Temperature, 1);
            else
                Serial.print(F("----"));
            Serial.print(F("\t"));
            if (!TRL.Error)
                Serial.print(TRL.Temperature, 1);
            else
                Serial.print(F("----"));
            Serial.print(F("\t"));
            if (!THE.Error)
                Serial.print(THE.Temperature, 1);
            else
                Serial.print(F("----"));
            Serial.print(F("\t"));
#endif
            Serial.print(ThermostatIn.State());
            Serial.print(F("\t"));
            Serial.print(HeatingAgentThermostatOut.State());
            Serial.print(F("\t"));
            Serial.print(HeatPump.State()); // debug probabil ca voi schimba daca exista si o limita de jos
            Serial.print(F("\t"));
            Serial.print(CirculatorPump.State());
            Serial.print(F("\t"));
            double Freq = (double)SerialFlowMeterCounter / 60 / SERIAL_REPORT_INTERVAL;
            SerialFlowMeterCounter = 0;
            Serial.print(Freq, 1);
            Serial.print(F("\t"));
            double Flow = Freq * 60 / FLOW_METER_IMP_PER_L;
            Serial.print(Flow, 1);
            Serial.print(F("\t"));
            long HeatingPower = 0;
            if ((!THI.Error) && (!THO.Error))
            {
                HeatingPower = (double)(THO.Temperature - THI.Temperature) * Flow * WATTS_PER_L_PER_K * 60;
                Serial.print(HeatingPower);
            }
            else
                Serial.print("----");
            Serial.print(F("\t"));
#if ENVIRONMENT == DEVELOPMENT
            int power = 1234;
#endif
#if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
            int power = pzem.power();
#endif
            if (isnan(power))
                Serial.print(F("Err\tErr"));
            else
            {
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
#if ENVIRONMENT == DEVELOPMENT
            float energy = HeatEnergyMeter / 4 / 1000;
#endif
#if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
            float energy = pzem.energy();
#endif
            if (isnan(energy))
                Serial.print(F("Err\tErr"));
            else
            {
                Serial.print((long)(energy * 1000 - SessionElectricityEnergyMeterStart));
                Serial.print(F("\t"));
                if ((energy * 1000 - SessionElectricityEnergyMeterStart) != 0)
                    Serial.print(HeatEnergyMeter / (energy * 1000 - SessionElectricityEnergyMeterStart));
                else
                    Serial.print(F("----"));
            }
            Serial.print(F("\t"));
            if (CirculatorPump.State())
                Serial.print(PC.HeatEnergyMeter + HeatEnergyMeter / 1000);
            else
                Serial.print(PC.HeatEnergyMeter);
            Serial.print(F("\t"));
            if (isnan(energy))
                Serial.print(F("Err\tErr"));
            else
            {
                Serial.print(energy);
                Serial.print(F("\t"));
                if ((energy) != 0)
                    Serial.print((PC.HeatEnergyMeter + (CirculatorPump.State() ? HeatEnergyMeter / 1000 : 0)) / energy);
                else
                    Serial.print(F("----"));
            }
            Serial.print(F("\t"));
            Serial.println();
        }
    }
};
