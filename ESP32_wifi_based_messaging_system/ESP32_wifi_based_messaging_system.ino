#include <WiFi.h>
#include <WebServer.h>
#include "time.h"

const char* ssid = "keshav";
const char* password = "0987654321";

WebServer server(80);

#define MAX_MSG 10

String messages[MAX_MSG];
int msgIndex = 0;

// NTP settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;   // India GMT+5:30 (19800 sec)
const int   daylightOffset_sec = 0;

String getTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Time Error";
  }
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
  return String(buffer);
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h2>ESP32 Chat Server</h2>";
  html += "<form action='/send'>";
  html += "<input type='text' name='msg'>";
  html += "<input type='submit' value='Send'>";
  html += "</form><hr>";

  html += "<h3>Messages:</h3>";

  for (int i = 0; i < MAX_MSG; i++) {
    if (messages[i] != "") {
      html += "<p>" + messages[i] + "</p>";
    }
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSend() {
  if (server.hasArg("msg")) {
    String newMsg = server.arg("msg");
    String timeStamp = getTime();

    messages[msgIndex] = timeStamp + " : " + newMsg;
    msgIndex = (msgIndex + 1) % MAX_MSG;  // circular buffer
  }

  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  server.on("/", handleRoot);
  server.on("/send", handleSend);
  server.begin();
}

void loop() {
  server.handleClient();
}
