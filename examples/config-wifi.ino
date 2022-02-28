#include <Preferences.h>

Preferences pref;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("configuring wifi");

  pref.begin("wifi-led",false);
  pref.putString("ssid","omitted");
  pref.putString("password","omitted");
  pref.end();

  Serial.println("wifi configured");
}

void loop() {
  // put your main code here, to run repeatedly:

}