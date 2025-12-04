#include <NTPClient.h>

#include <ArduinoWebsockets.h>

#include <ESP8266HTTPClient.h>
#include <ArduinoWiFiServer.h>
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
    Serial.println("Connnection closed, reconnecting...");
    delay(1000);
    ws.connect("wss://urbanf.kresnawan.com:443/v1/ws");
  } 
  // else if (event == WebsocketsEvent::GotPing) {
  //   Serial.println("Got a Ping!");
  // } else if (event == WebsocketsEvent::GotPong) {
  //   Serial.println("Got a Pong!");
  // }
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
  while (WiFi.status() != WL_CONNECTED) {  // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.println(++i);
  }

  

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  pinMode(4, OUTPUT);
  timeClient.begin();

  // run callback when events are occuring
  ws.onEvent(onEventsCallback);
  ws.onMessage(onMessageCallback);

  ws.setFingerprint(ssl_fingerprint);
  // Connect to server
  ws.addHeader("Authorization", "device");
  ws.connect("wss://urbanf.kresnawan.com:443/v1/ws");
  
  

  // Send a message
  ws.send("Hello Server");
  ws.ping();
}

unsigned long previousMillisLoop = 0;
const long intervalLoop = 1;  // Tugas perulangan berjalan setiap 5ms (contoh)

unsigned long previousMillisPeriodic = 0;
const long intervalPeriodic = 1000 * 50;

void loop() {
  unsigned long currentMillis = millis();

  // Continuous
  if (currentMillis - previousMillisLoop >= intervalLoop) {
    previousMillisLoop = currentMillis;
    ws.poll();
    
  }

  // Periodic (a minute)
  if (currentMillis - previousMillisPeriodic >= intervalPeriodic) {
    previousMillisPeriodic = currentMillis;
    // ws.ping();
  }

  // digitalWrite(4, HIGH);
  // ws.send("ON");
  // delay(5000);

  // digitalWrite(4, LOW);
  // ws.send("OFF");
  // delay(5000);

  // if(WiFi.status() == WL_CONNECTED) {
  //   digitalWrite(4, HIGH);
  //   http.begin(client, "http://4f79dbd51f55.ngrok-free.app:80/v1");
  //   int httpCode = http.GET();

  //   if (httpCode > 0) {
  //     String payload = http.getString();
  //     Serial.println(httpCode);
  //     Serial.println(payload);
  //     digitalWrite(4, LOW);
  //   } else {
  //     Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  //   }

  //   http.end();

  //   delay(5000);
  // }
}
