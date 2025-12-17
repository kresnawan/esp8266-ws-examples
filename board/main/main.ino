#include <NTPClient.h>

#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

/* Match with your wifi */

const char* ssid = "your_ssid";
const char* password = "your_password";

WiFiClient client;

using namespace websockets;

WebsocketsClient ws;
WiFiUDP ntpUDP;

/* Match the port with your preferences, 80 for ws and 443 for wss */
const char URL[] PROGMEM = "wss://ipaddress:443/path";

/* Used for timestamping the serial print */
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000);

void logd(const char* message) {
  timeClient.update();
  Serial.printf("[%s] ", timeClient.getFormattedTime());
  Serial.printf("%s\n", message);
}

void wsConnect() {
  int i = 0;
  bool connected = ws.connect(URL);

  if (!connected) {
    logd("Websocket connection failed");
  }
}

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
    logd("Websocket connection opened");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    logd("Websocket connection closed, reconnecting...");
    delay(1000);


    wsConnect();
  }
}

void setup() {
  timeClient.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  delay(5000);
  Serial.print('\n');

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(++i);
  }

  logd("WiFi connected");

  pinMode(4, OUTPUT);
  
  ws.onEvent(onEventsCallback);
  ws.onMessage(onMessageCallback);

  /* Set insecure so it is not required to change the fingerprint periodically */
  /* Not recommended for production, go with fingerprint for safe actions */ 
  ws.setInsecure();
  ws.addHeader("Authorization", "device");

  wsConnect();

  ws.ping();
}

unsigned long previousMillisLoop = 0;
const long intervalLoop = 1;

unsigned long previousMillisPeriodic = 0;
const long intervalPeriodic = 1000 * 50;

void loop() {

  /* WiFi connectivity checker */
  if (WiFi.status() != WL_CONNECTED) {
    logd("WiFi disconnected, reconnecting...");
    bool connected = WiFi.reconnect();

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }

    logd("WiFi reconnected");
    wsConnect();
  }

  unsigned long currentMillis = millis();

  /* Continuous execution */
  if (currentMillis - previousMillisLoop >= intervalLoop) {
    previousMillisLoop = currentMillis;
    ws.poll();
  }
}
