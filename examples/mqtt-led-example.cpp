/* Cobbled this together with a few tutorials I found online.

 It connects to my wifi, then connects to the mqtt server on radmax

 Then, it accepts input from an mqtt topic called 'esp32/output' and blinks the onboard led */


#include <WiFi.h>
#include <PubSubClient.h>

#define LED_BUILTIN 2

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "10.0.0.13";

long lastMsg = 0;
char msg[50];
int value = 0;

void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network
    WiFi.begin("omitted","omitted");  
    client.setServer(mqtt_server,1883);  
    client.setCallback(callback);
    pinMode(LED_BUILTIN,OUTPUT);
    Serial.println();
    Serial.println();
    Serial.print("Waiting for WiFi... ");

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}

void callback(char* topic, byte* message, unsigned int length){
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++){
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "esp32/output"){
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(LED_BUILTIN,LOW);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("attempting mqtt connection");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop()
{
  if (!client.connected()) {
    reconnect();    
  }

  client.loop();
  long now = millis();
  if (now - lastMsg > 5000){
    lastMsg = now;
  }
}