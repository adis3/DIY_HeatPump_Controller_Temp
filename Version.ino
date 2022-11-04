void Version(){
  Serial.print(F("File: "));
  Serial.println(FileName);
  Serial.println(F("Version: "VERSION));
  Serial.print(F("IDE v: "));
  Serial.println(ARDUINO);
  Serial.println(F("Uploaded: " __DATE__ ", " __TIME__ ", " __VERSION__));
  Serial.print(F("Environment: "));
  //0 - Development, 1- Testing, 2- Acceptance, 3 - Production, 4 - Backup
  #if ENVIRONMENT == 0
    Serial.println(F("Undefined"));
  #elif ENVIRONMENT == DEVELOPMENT
    Serial.println(F("Development"));
  #elif ENVIRONMENT == TESTING
    Serial.println(F("Testing"));
  #elif ENVIRONMENT == ACCEPTANCE
    Serial.println(F("Acceptance"));
  #elif ENVIRONMENT == PRODUCTION
    Serial.println(F("Production");
  #elif ENVIRONMENT == BACKUP
    Serial.println(F("Backup"));
  #endif
}
