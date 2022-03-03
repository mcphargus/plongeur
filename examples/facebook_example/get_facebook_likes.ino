/*
There's a bunch of setup required before this code can even do anything.

- Create a facebook app
- add your own user as an admin to the app
- (maybe) be the admin of a page (not sure if I can pull fan_count for pages that I don't admin)
- load all the variables below 
*/

#include <HTTPClient.h>;
#include <ssl_client.h>
#include <Preferences.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// stored as preferences non-volatile storage

String ssid;
String password;
String appId;
String appSecret;
String clientToken;
String userId;
String pageId;
String server;

// end preferences we set manually, PAT gets set below if it's not available

int httpCode;
String req;
String long_code;
String SLAT;
String LLAT;
String PAT;
int fan_count;

Preferences pref;

// preload facebooks graph cert

const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
"+OkuE6N36B9K\n" \
"-----END CERTIFICATE-----\n";

HTTPClient http;

void setup() {  
  Serial.begin(115200);
  delay(10);

  pref.begin("HFPFanCount",false);
  ssid = pref.getString("ssid","");
  password = pref.getString("password","");  
  appId = pref.getString("appId","");
  appSecret = pref.getString("appSecret","");
  clientToken = pref.getString("clientToken","");
  userId = pref.getString("userId","");
  pageId = pref.getString("pageId","");
  PAT = pref.getString("PAT","");
  server = "graph.facebook.com";
  
  if (ssid == "" || password == ""){
    Serial.println("credentials need to be set first");
  } else {
    Serial.println("Connecting to wifi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(),password.c_str());
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }

    http.useHTTP10(true);

    if (PAT == ""){
      Serial.println("PAT not set, going throug fb oauth flow");
      // with auth code, be quick to copy and paste the link and authorize!
      // would definitely be smarter to set up an http server endpoint for this flow
      // or, just do all this crap in postman if we haven't turned on the chip in 60 days
      String scope = "read_insights,pages_show_list,pages_read_engagement,pages_read_user_content,public_profile";
      String httpRequestData = "access_token=" + appId + "|" + clientToken + "&scope=" + scope;
      req = "https://" + server + "/v2.6/device/login";
      http.useHTTP10(true);
      http.begin(req, ca_cert);
      httpCode = http.POST(httpRequestData);
      if (httpCode > 0){
        DynamicJsonDocument doc(2048);        
        deserializeJson(doc, http.getStream());      
        String user_code = doc["user_code"].as<String>();  
        long_code = doc["code"].as<String>();      
        Serial.println("auth link is https://www.facebook.com/device?user_code=" + user_code);
      } else {
        Serial.println("Error on http request");
      }
      
  
      // get short lived user access token (SLAT)
      Serial.println("checking for SLAT in 20 seconds");
      delay(20000);    
      httpRequestData = "access_token=" + appId + "|" + clientToken + "&code=" + long_code;
      req = "https://" + server + "/v2.6/device/login_status";
      http.begin(req,ca_cert);
      httpCode = http.POST(httpRequestData);
      if (httpCode > 0){
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, http.getStream());
        SLAT = doc["access_token"].as<String>();
      } else {
        Serial.println("Error on http request");
      }    
      
      // get long lived user access token
      Serial.println("getting long lived user access token");
      req = "https://" + server + "/oauth/access_token?" + \
        "grant_type=fb_exchange_token&client_id=" + appId + \
        "&client_secret=" + appSecret + \
        "&fb_exchange_token=" + SLAT;
      http.begin(req,ca_cert);
      httpCode = http.GET();
      if (httpCode > 0){
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, http.getStream());
        LLAT = doc["access_token"].as<String>();
      } else {
        Serial.println("Error on http request");
      }
  
      // get page auth
      Serial.println("getting page auth");
      req = "https://" + server + "/" + pageId + \
        "?fields=access_token&access_token=" + LLAT;
      http.begin(req,ca_cert);
      httpCode = http.GET();
      if (httpCode > 0){
        DynamicJsonDocument doc(2048);
        deserializeJson(doc,http.getStream());
        PAT = doc["access_token"].as<String>();
        pref.putString("PAT",PAT.c_str());
      } else {
        Serial.println("Error on http request");
      }

      // get fan count (freaking victory)
      // This needs to be moved into the loop and run every few hours, possibly with some sort of sleep enabled to save battery
      Serial.println("getting fan count");
      req = "https://" + server + "/" + pageId + \
        "?fields=fan_count&access_token=" + PAT;
      http.begin(req,ca_cert);
      httpCode = http.GET();
      if (httpCode > 0) {
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, http.getStream());
        fan_count = doc["fan_count"].as<int>();
        Serial.print("Hope For Pits has ");
        Serial.print(fan_count);
        Serial.println(" likes!");
      }   
      
    } else {
      // get fan count (freaking victory)
      // This needs to be moved into the loop and run every few hours, possibly with some sort of sleep enabled to save battery
      Serial.println("getting fan count");
      req = "https://" + server + "/" + pageId + \
        "?fields=fan_count&access_token=" + PAT;
      http.begin(req,ca_cert);
      httpCode = http.GET();
      if (httpCode > 0) {
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, http.getStream());
        fan_count = doc["fan_count"].as<int>();
        Serial.print("Hope For Pits has ");
        Serial.print(fan_count);
        Serial.println(" likes!");
      }    
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:  
  
  Serial.println("getting fan count");
  req = "https://" + server + "/" + pageId + \
    "?fields=fan_count&access_token=" + PAT;
  http.begin(req,ca_cert);
  httpCode = http.GET();
  if (httpCode > 0) {
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, http.getStream());
    fan_count = doc["fan_count"].as<int>();
    Serial.print("Hope For Pits has ");
    Serial.print(fan_count);
    Serial.println(" likes!");
  }  
  delay(60000); // chill for 1 minute

}