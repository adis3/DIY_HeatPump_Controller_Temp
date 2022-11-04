void Version(){
  Serial.print(F("File: "));
  Serial.println(FileName);
  Serial.println(F("Version: "VERSION));
  Serial.print(F("IDE v: "));
  Serial.println(ARDUINO);
  Serial.println(F("Uploaded: " __DATE__ ", " __TIME__ ", " __VERSION__));
  Serial.print(F("Environment: "));
  //0 - Development, 1- Testing, 2- Acceptance, 3 - Production, 4 - Backup
  #if ENV == 0
    Serial.println("Development");
  #elif ENV == 1
    Serial.println("Testing");
  #elif ENV == 2
    Serial.println("Acceptance");
  #elif ENV == 3
    Serial.println("Production");
  #elif ENV == 4
    Serial.println("Backup");
  #endif
}
