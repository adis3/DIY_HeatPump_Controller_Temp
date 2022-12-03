//Environment instances, 0 - Undefined
#define DEVELOPMENT 1
#define TESTING 2 // debug compilation errors due to incomplete configuration in config.h
#define ACCEPTANCE 3
#define PRODUCTION 4
#define BACKUP 5

//Environment
#define ENVIRONMENT DEVELOPMENT

//Hardware components values
#define HW_PRESENT 1
#define HW_ABSENT 2

//Hardware components
#if ENVIRONMENT == DEVELOPMENT
  #define FLOW_METER HW_ABSENT
  #define THERMOSTAT HW_PRESENT
  #define BACKUP_HEATER HW_PRESENT
  #define AMBIENTAL_NTC_OPTOCOUPLER HW_PRESENT
  #define CH376_USB_FLASH_DRIVE_HOST HW_ABSENT
  #define PZEM_004T_v30 HW_ABSENT
  #define PZEM_004T_PREV HW_ABSENT
  #define HC_06 HW_ABSENT
  #define LCD_SERIAL HW_ABSENT
  #define LCD_I2C HW_ABSENT
  #define LCD_RTC HW_ABSENT
  #define LED_PUSH_BUTTON HW_ABSENT
  #define ROTARY_ENCODER HW_ABSENT
  #define JOYSTICK HW_ABSENT
#elif ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
  #define FLOW_METER HW_PRESENT
  #define THERMOSTAT HW_PRESENT
  #define BACKUP_HEATER HW_PRESENT
  #define AMBIENTAL_NTC_OPTOCOUPLER HW_PRESENT
  #define CH376_USB_FLASH_DRIVE_HOST HW_ABSENT
  #define PZEM_004T_v30 HW_PRESENT
  #define PZEM_004T_PREV HW_ABSENT
  #define HC_06 HW_ABSENT
  #define LCD_SERIAL HW_ABSENT
  #define LCD_I2C HW_ABSENT
  #define LCD_RTC HW_ABSENT
  #define LED_PUSH_BUTTON HW_ABSENT
  #define ROTARY_ENCODER HW_ABSENT
  #define JOYSTICK HW_ABSENT
#endif

//Serial params
#define SERIAL_SPEED 19200
#define SERIAL_PARAM SERIAL_8N1

#define FLOW_METER_IMP_PER_L 352.735642
#define WATTS_PER_L_PER_K 1.16388889

#define DEBOUNCE_INTERVAL 200

#define LED_PIN 13
#define LED_STEPS_COUNT 16
#define LED_STEP_SETS_COUNT 10
#define LED_STEPSET_FOR_PUSH_BUTTON_COUNTER 10
#define LED_STEPSET_FOR_ERROR 6
#define LED_STEPSET_FOR_WARNING 8

LEDPWMStep PS[] = {{0,0,2995},{60,60,5},//PowerOff
  {3,3,2950},{60,60,50},//StandBy
  {3,60,3000},//Heating
  {60,3,2000},//HeatingNTCLimit
  {3,60,500},{60,3,500},//ShuttingDown
  {60,3,500},//HeatingDefrosting - debug neimplementat
  {0,0,400},{255,255,100},{0,0,3000},//Error
  {30,30,400},{255,255,100},{30,30,3000},//Warning
  {120,120,900},{255,255,100}};//Push button counter
LEDStepSet DSS[] = {{0,1,3,0},//PowerOff
  {2,3,3,1},//StandBy
  {4,4,3,2},//Heating
  {5,5,3,3},//HeatingNTCLimit
  {6,7,3,4},//ShuttingDown
  {8,8,3,5},//HeatingDefrosting - debug neimplementat
  {9,10,2,7},{11,11,1,6},//Error
  {12,13,2,9},{14,14,1,8},//Warning
  {15,16,3,10}};//Push button counter

#define BUTTON_COMMAND_1_LIMIT 1
#define BUTTON_COMMAND_2_LIMIT 2 //Power off
#define BUTTON_COMMAND_3_LIMIT 3 //Clear warning
#define BUTTON_COMMAND_4_LIMIT 4 //Clear error
#define BUTTON_COMMAND_5_LIMIT 5
#define BUTTON_COMMAND_6_LIMIT 10
#define BUTTON_COMMAND_7_LIMIT 11 //reset counters


#if ENVIRONMENT == DEVELOPMENT
  #define THERMOSTAT_INPUT_PIN 3
  #define BUTTON_INPUT_PIN 4
  #define HEAT_PUMP_OUTPUT_PIN 12
  #define CIRCULATOR_PUMP_OUTPUT_PIN A2
  #define AMBIENTAL_NTC_OPTOCOUPLER_OUTPUT_PIN 10
  #define BACKUP_HEATER_OUTPUT_PIN 9
  
  #define HEAT_EXCHANGER_ONE_WIRE_PIN 7
  #define OUTSIDE_ONE_WIRE_PIN 7
  
  #define FLOW_METER_INPUT_PIN 2
  
  #define HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL 1
  #define OUTSIDE_THERMOMETER_READING_INTERVAL 60
  
  //#define TempSensorReadInterval 2
  #define THERMOMETER_READING_DELAY 750
  #define THERMOMETER_MIN_MAX_CLEAR_INTERVAL 72000000
  
  //#define HEATING_AGENT_MAX_TEMPERATURE 38
  //#define HEATING_AGENT_HISTEREZIS 1
  #define HEATING_AGENT_SWITCHING_INTERVAL 1//debug 30
  #define HEATING_AGENT_TARGET_TEMPERATURE_MIN_LIMIT 5
  #define HEATING_AGENT_TARGET_TEMPERATURE_MAX_LIMIT 70
  
  #define POWER_OFF_DELAY 1 //minutes
  #define SERIAL_REPORT_INTERVAL 1 //minutes
  #define LCD_REPORT_INTERVAL 3 //seconds
  
  #define SERIAL_REPORT_LINES 40
  
  struct TParams {
    byte Bus;
    byte Addr[8];
    float Cal;
    }
    TPHO = {HEAT_EXCHANGER_ONE_WIRE_PIN, {0x28, 0x87, 0xB1, 0x79, 0xA2, 0x0, 0x3, 0x93}, 1},//-0.28}, debug forced heated
    TPHI = {HEAT_EXCHANGER_ONE_WIRE_PIN, {0x28, 0x20, 0x32, 0x79, 0xA2, 0x0, 0x3, 0xCF}, 0.22},
    TPRG = {HEAT_EXCHANGER_ONE_WIRE_PIN, {0x28, 0x87, 0xB1, 0x79, 0xA2, 0x0, 0x3, 0x93}, 0.09},
    TPRL = {HEAT_EXCHANGER_ONE_WIRE_PIN, {0x28, 0x20, 0x32, 0x79, 0xA2, 0x0, 0x3, 0xCF}, 0.03},
    TPHE = {HEAT_EXCHANGER_ONE_WIRE_PIN, {0x28, 0x87, 0xB1, 0x79, 0xA2, 0x0, 0x3, 0x93}, -0.28}, //firul scurt
    TPE = {OUTSIDE_ONE_WIRE_PIN, {0x28, 0x20, 0x32, 0x79, 0xA2, 0x0, 0x3, 0xCF}, 0}; //exterior

  #define INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR 20
  #define AUTO_SAVE_ENERGY_METERS 180//debug nu pot salva la fiecare minut in EPROM decat in teste limitate
#endif

#if ENVIRONMENT == ACCEPTANCE
  #define POWER_OFF_DELAY 1
//  #define HEATING_AGENT_MAX_TEMPERATURE 38
//  #define HEATING_AGENT_HISTEREZIS 6
  #define HEATING_AGENT_SWITCHING_INTERVAL 1//debug 30
  #define HEATING_AGENT_TARGET_TEMPERATURE_MIN_LIMIT 5
  #define HEATING_AGENT_TARGET_TEMPERATURE_MAX_LIMIT 70
  #define INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR 60
  #define AUTO_SAVE_ENERGY_METERS 180
#endif
#if ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
  #define POWER_OFF_DELAY 10
//  #define HEATING_AGENT_MAX_TEMPERATURE 38
//  #define HEATING_AGENT_HISTEREZIS 6
  #define HEATING_AGENT_SWITCHING_INTERVAL 1//debug 30
  #define HEATING_AGENT_TARGET_TEMPERATURE_MIN_LIMIT 5
  #define HEATING_AGENT_TARGET_TEMPERATURE_MAX_LIMIT 70
  #define INTERNAL_EEPROM_PERSISTENT_CONF_START_ADDR 40
  #define AUTO_SAVE_ENERGY_METERS 180
#endif

#if ENVIRONMENT == ACCEPTANCE or ENVIRONMENT == PRODUCTION or ENVIRONMENT == BACKUP
  #define THERMOSTAT_INPUT_PIN 3
  #define BUTTON_INPUT_PIN 4
  #define HEAT_PUMP_OUTPUT_PIN 12
  #define CIRCULATOR_PUMP_OUTPUT_PIN 11
  #define AMBIENTAL_NTC_OPTOCOUPLER_OUTPUT_PIN 10
  #define BACKUP_HEATER_OUTPUT_PIN 9
  
  #define HEAT_EXCHANGER_ONE_WIRE_PIN 7
  #define OUTSIDE_ONE_WIRE_PIN 7 //debug de schimbat
  
  #define FLOW_METER_INPUT_PIN 2

  #define LCDTxPin 53 //as noted on display, not actual Arduino Tx
  #define LCDRxPin 51 //as noted on display, not actual Arduino Rx
    
  #define HEAT_EXCHANGER_THERMOMETER_READING_INTERVAL 6
  #define OUTSIDE_THERMOMETER_READING_INTERVAL 60
  
  //#define TempSensorReadInterval 2
  #define THERMOMETER_READING_DELAY 750
  #define THERMOMETER_MIN_MAX_CLEAR_INTERVAL 72000000
    
  #define SERIAL_REPORT_INTERVAL 1 //minutes
  #define LCD_REPORT_INTERVAL 5 //seconds

  #define SERIAL_REPORT_LINES 0
  
  struct TParams {
    byte Bus;
    byte Addr[8];
    float Cal;
    }
    TPHO = {HEAT_EXCHANGER_ONE_WIRE_PIN, {0x28, 0x34, 0x8, 0x79, 0xA2, 0x0, 0x3, 0x71}, -0.28},
    TPHI = {HEAT_EXCHANGER_ONE_WIRE_PIN, {0x28, 0xA0, 0x89, 0x79, 0xA2, 0x0, 0x3, 0x1A}, 0.22},
    TPRG = {HEAT_EXCHANGER_ONE_WIRE_PIN, {0x28, 0x8F, 0x0, 0x79, 0xA2, 0x0, 0x3, 0xB0}, 0.09},
    TPRL = {HEAT_EXCHANGER_ONE_WIRE_PIN, {0x28, 0xD5, 0x1, 0x79, 0xA2, 0x0, 0x3, 0x9C}, 0.03},
    TPHE = {HEAT_EXCHANGER_ONE_WIRE_PIN, {0x28, 0x75, 0xC9, 0x79, 0xA2, 0x0, 0x3, 0xDC}, -0.28}, //firul scurt
    TPE = {OUTSIDE_ONE_WIRE_PIN, {0x28, 0x90, 0xE, 0x79, 0xA2, 0x0, 0x3, 0x6D}, 0.22}; //exterior

/*
 * HW Connections:
 * 
 * Power 2P connector
 * VIN - spacer
 * GND - spacer
 * GND - PWR - Should move on the 2x18P connector with 2P connector for contact reliability
 * +5V - PWR - Should move on the 2x18P connector with 2P connector for contact reliability
 * +3.3V - NC
 * RESET - NC
 * IOREF - NC
 * NC - NC
 * 
 * D0-D7 6P connector
 * D0/RX0/PE0 - spacer
 * D1/TX0/PE1 - spacer
 * D2/PE4/INT4 - FLOW_METER_INPUT_PIN
 * D3/PE5/INT5 - THERMOSTAT_INPUT_PIN - with 10ko, 100nF and clampind diodes for protection
 * D4/PG5 - BUTTON_INPUT_PIN
 * D5/PE3 - 
 * D6/PH3 - 
 * D7/PH4 - HEAT_EXCHANGER_ONE_WIRE_PIN / OUTSIDE_ONE_WIRE_PIN //debug de facut mai multe magistrale
 * 
 * D8-D21 6P connector
 * D8/PH5 - 
 * D9/PH6 - BACKUP_HEATER_OUTPUT_PIN
 * D10/PB4 - AMBIENTAL_NTC_OPTOCOUPLER_OUTPUT_PIN
 * D11/PB5 - CIRCULATOR_PUMP_OUTPUT_PIN
 * D12/PB6 - HEAT_PUMP_OUTPUT_PIN
 * D13/PB7 - LED_PIN
 * GND - spacer
 * AREF - spacer
 * D20/SDA/PD1 - spacer
 * D21/SCL/PD0 - spacer
 * 
 * D14-D21 8P connector
 * D14/TX3 - CH376 USB flash drive host - tested, but not implemented
 * D15/RX3 - CH376 USB flash drive host - tested, but not implemented
 * D16/TX2 - PZEM_RX_PIN - PZEM Module
 * D17/RX2 - PZEM_TX_PIN - PZEM Module
 * D18/TX1 - HC-06 bluetooth module - not received
 * D19/RX1 - HC-06 bluetooth module - not received
 * D20/SDA/PD1 - LCD/RTC - tested, but not implemented
 * D21/SCL/PD0 - LCD/RTC - tested, but not implemented
 * 
 * D22-D53
 * 5Vx2 - PWR 2P connector
 * 
 * D51 - LCDRxPin - SeeedStudio SerialLCD - To be replaced by I2C LCD
 * D53 - LCDTxPin - SeeedStudio SerialLCD - To be replaced by I2C LCD
 * GNDx2 - PWR 2P connector
 * 
 * 
 * https://content.arduino.cc/assets/Pinout-Mega2560rev3_latest.png
 * https://www.electronicshub.org/wp-content/uploads/2021/01/Arduino-Mega-Pinout.jpg
 * https://aws1.discourse-cdn.com/arduino/original/4X/f/6/2/f6258a2ebd00a3f75099715f82914ed17175130f.jpeg
 * 
 */
    
#endif