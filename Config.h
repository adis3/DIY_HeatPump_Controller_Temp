//Hw config
#define SerialSpeed 19200
#define SerialParam SERIAL_8N1

#define FlowMeterImpPerL 352.735642
#define WatsPerLPerKelvin 1.16388889 

#if ENV == 0
  #define ThermostatInputPin 3
  #define HeatPumpPin A1
  #define CirculatorPumpPin A2
  #define AmbientalNTCOptocouplerPin 10
  
  #define HeatExchangerOWPin A0
  #define OutsideOWPin A0
  
  #define FlowMeterInputPin 2
  
  #define HeatExchangerReadingInterval 1
  #define OutsideReadingInterval 60
  
  //#define TempSensorReadInterval 2
  #define TempSensorReadDelay 750
  #define TempSensorMinMaxClearDelay 72000000
  
  #define HeatingAgentTargetTemp 27
  #define HeatingAgentMaxTemp 38
  #define HeatingAgentHisterezis 1
  #define HeatingAgentSwitchingInterval 1//debug 30
  #define HeatingAgentMinTargetTemp 25
  #define HeatingAgentMaxTargetTemp 40
  
  #define PowerOffDelay 1 //minutes
  #define ReportInterval 1 //minutes
  #define LCDInterval 3 //seconds
  
  #define ReportLines 40
  
  struct TParams {
    byte Bus;
    byte Addr[8];
    float Cal;
    }
    TPHO = {HeatExchangerOWPin, {0x28, 0x87, 0xB1, 0x79, 0xA2, 0x0, 0x3, 0x93}, -0.28},
    TPHI = {HeatExchangerOWPin, {0x28, 0x20, 0x32, 0x79, 0xA2, 0x0, 0x3, 0xCF}, 0.22},
    TPRG = {HeatExchangerOWPin, {0x28, 0x87, 0xB1, 0x79, 0xA2, 0x0, 0x3, 0x93}, 0.09},
    TPRL = {HeatExchangerOWPin, {0x28, 0x20, 0x32, 0x79, 0xA2, 0x0, 0x3, 0xCF}, 0.03},
    TPHE = {HeatExchangerOWPin, {0x28, 0x87, 0xB1, 0x79, 0xA2, 0x0, 0x3, 0x93}, -0.28}, //firul scurt
    TPE = {OutsideOWPin, {0x28, 0x20, 0x32, 0x79, 0xA2, 0x0, 0x3, 0xCF}, 0}; //exterior
#endif

#if ENV == 2
  #define PowerOffDelay 1
  #define HeatingAgentTargetTemp 25
  #define HeatingAgentMaxTemp 38
  #define HeatingAgentHisterezis 6
  #define HeatingAgentSwitchingInterval 1//debug 30
  #define HeatingAgentMinTargetTemp 25
  #define HeatingAgentMaxTargetTemp 40
#endif
#if ENV == 3 or ENV == 4
  #define PowerOffDelay 10
  #define HeatingAgentTargetTemp 32
  #define HeatingAgentMaxTemp 38
  #define HeatingAgentHisterezis 6
  #define HeatingAgentSwitchingInterval 1//debug 30
  #define HeatingAgentMinTargetTemp 25
  #define HeatingAgentMaxTargetTemp 40
#endif

#if ENV == 2 or ENV == 3 or ENV == 4
  #define ThermostatInputPin 3
  #define HeatPumpPin 12
  #define CirculatorPumpPin 11
  #define AmbientalNTCOptocouplerPin 10
  
  #define HeatExchangerOWPin 7
  #define OutsideOWPin 7 //debug de schimbat
  
  #define FlowMeterInputPin 2

  #define LCDTxPin 53 //as noted on display, not actual Arduino Tx
  #define LCDRxPin 51 //as noted on display, not actual Arduino Rx
    
  #define HeatExchangerReadingInterval 6
  #define OutsideReadingInterval 60
  
  //#define TempSensorReadInterval 2
  #define TempSensorReadDelay 750
  #define TempSensorMinMaxClearDelay 72000000
    
  #define ReportInterval 1 //minutes
  #define LCDInterval 5 //seconds

  #define ReportLines 0
  
  struct TParams {
    byte Bus;
    byte Addr[8];
    float Cal;
    }
    TPHO = {HeatExchangerOWPin, {0x28, 0x34, 0x8, 0x79, 0xA2, 0x0, 0x3, 0x71}, -0.28},
    TPHI = {HeatExchangerOWPin, {0x28, 0xA0, 0x89, 0x79, 0xA2, 0x0, 0x3, 0x1A}, 0.22},
    TPRG = {HeatExchangerOWPin, {0x28, 0x8F, 0x0, 0x79, 0xA2, 0x0, 0x3, 0xB0}, 0.09},
    TPRL = {HeatExchangerOWPin, {0x28, 0xD5, 0x1, 0x79, 0xA2, 0x0, 0x3, 0x9C}, 0.03},
    TPHE = {HeatExchangerOWPin, {0x28, 0x75, 0xC9, 0x79, 0xA2, 0x0, 0x3, 0xDC}, -0.28}, //firul scurt
    TPE = {OutsideOWPin, {0x28, 0x90, 0xE, 0x79, 0xA2, 0x0, 0x3, 0x6D}, 0.22}; //exterior

/*
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */
    
#endif


/*
 * intrare optocuplor centrala - 9
 * iesire optocuplor NTC pompa de caldura - 10
 * iesire pornire pompa de caldura - 12
 * iesire pornire pompa de recirculare - 11
 * intrare bus OnwWire - 7
 * intrare comanda caldura de la pardoseala - 3
 * intrare debimetru - 2
 */
