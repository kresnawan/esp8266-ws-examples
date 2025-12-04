#include <NTPClient.h>

#include <ArduinoWebsockets.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

const char* ssid = "WIFI TAMU";
const char* password = "tehpucukharum";

WiFiClient client;
const char ssl_fingerprint[] PROGMEM = "F0 AD EE F3 A0 60 D6 DC 7F 45 8C 25 27 1F A2 DD A3 9F 2A E2";

using namespace websockets;

WebsocketsClient ws;
WiFiUDP ntpUDP;

// Used for timestamping the log
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000);

void onMessageCallback(WebsocketsMessage message) {
  if (message.data() == "ON") {
    digitalWrite(4, HIGH);
  } else if (message.data() == "OFF") {
    digitalWrite(4, LOW);
  } else if (message.data() == "DC") {
    ws.close();
  }
}

void onEventsCallback(WebsocketsEvent event, String data) {
  
  if (event == WebsocketsEvent::ConnectionOpened) {
    timeClient.update();
    Serial.printf("[%s] ", timeClient.getFormattedTime());
    Serial.println("Connnection Opened");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    timeClient.update();
    Serial.printf("[%s] ", timeClient.getFormattedTime());

    // Reconnect when connection is closed
    Serial.println("Connnection closed, reconnecting...");
    delay(1000);

    
    ws.connect("wss://yourdomain.com:443/path");
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(2000);
  Serial.println('\n');

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(++i);
  }

  

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  pinMode(4, OUTPUT);
  timeClient.begin();

  // Run callback when events are occuring
  ws.onEvent(onEventsCallback);
  ws.onMessage(onMessageCallback);

  // Adding fingerprint is a must for wss
  ws.setFingerprint(ssl_fingerprint);
  ws.addHeader("Authorization", "device");

  // The port must correct
  // 80 for ws and 443 for wss
  ws.connect("wss://yourdomain.com:443/path");

  ws.ping();
}

unsigned long previousMillisLoop = 0;
const long intervalLoop = 1;

unsigned long previousMillisPeriodic = 0;
const long intervalPeriodic = 1000 * 50;

void loop() {
  unsigned long currentMillis = millis();

  // Continuous execution
  if (currentMillis - previousMillisLoop >= intervalLoop) {
    previousMillisLoop = currentMillis;
    ws.poll();
    
  }
}
