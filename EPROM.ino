#define EEPROMSaveStartAddr 10

void SaveToEEPROM(){
  unsigned int addr = EEPROMSaveStartAddr;
  for (unsigned int i = 0; i < sizeof(pS); i++)
    if ( EEPROM.read(addr + i) != (*((unsigned char *)&pS + i)) )
      EEPROM.write( addr + i, *((unsigned char *)&pS + i));
  addr += sizeof(pS);
}

void LoadFromEEPROM(){
  unsigned int addr = EEPROMSaveStartAddr;
  for (unsigned int i = 0; i < sizeof(pS); i++)
    *((unsigned char *)&pS + i) = EEPROM.read(addr + i);
  addr += sizeof(pS);
}
