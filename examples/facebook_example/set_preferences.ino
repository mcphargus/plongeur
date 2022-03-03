/*
The Preferences library stores preferences in non-volatile memory 
so we can use it between power off events.
*/

#include <Preferences.h>
Preferences pref;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("configuring stuff");

  pref.begin("HFPFanCount",false);
  pref.putString("ssid","..");
  pref.putString("password","..");
  pref.putString("appId","..");
  pref.putString("appSecret","..");
  pref.putString("clientToken","..");
  pref.putString("userId","..");
  pref.putString("pageId","..");
  // room for Page Authentication Token (as PAT) if the device flow on the esp32 is too harrowing   
  pref.end();
  Serial.println("stuff configured");
}

void loop() {
  // kick back and do nothing
}