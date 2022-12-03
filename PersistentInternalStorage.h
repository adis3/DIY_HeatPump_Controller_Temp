// #ifndef PersistentInternalStorage_h
// #define PersistentInternalStorage_h

#include <EEPROM.h>

class PersistentInternalStorage
{
public:
    static void SaveToEEPROM(unsigned int StartAddr, void *Data, unsigned int Length)
    {
#if SERIAL_DEBUGGING == 1
        Serial.print(F("PersistentInternalStorage::SaveToEEPROM\t"));
        Serial.print(StartAddr);
        Serial.print(F("\t"));
#endif
        for (unsigned int i = 0; i < Length; i++)
        {
            if (EEPROM.read(StartAddr + i) != (*((unsigned char *)Data + i)))
                EEPROM.write(StartAddr + i, *((unsigned char *)Data + i));
#if SERIAL_DEBUGGING == 1
            Serial.print(*((unsigned char *)Data + i));
            Serial.print(F("\t"));
#endif
        }
#if SERIAL_DEBUGGING == 1
        Serial.println(Length);
#endif
    }
    static void LoadFromEEPROM(unsigned int StartAddr, void *Data, unsigned int Length)
    {
#if SERIAL_DEBUGGING == 1
        Serial.print(F("PersistentInternalStorage::LoadFromEEPROM\t"));
        Serial.print(StartAddr);
        Serial.print(F("\t"));
#endif
        for (unsigned int i = 0; i < Length; i++)
        {
            *((unsigned char *)Data + i) = EEPROM.read(StartAddr + i);
#if SERIAL_DEBUGGING == 1
            Serial.print(*((unsigned char *)Data + i));
            Serial.print(F("\t"));
#endif
        }
#if SERIAL_DEBUGGING == 1
        Serial.println(Length);
#endif
    }
};