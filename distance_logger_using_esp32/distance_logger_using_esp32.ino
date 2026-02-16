#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include "SPIFFS.h"

#define TRIG 5
#define ECHO 18

const char* ssid = "keshav";
const char* password = "0987654321";

WebServer server(80);

float readDistanceCM() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  unsigned long duration = pulseIn(ECHO, HIGH, 30000);
  if (duration == 0) return -1;

  return (duration * 0.0343f) / 2.0f;
}

String getDateTimeCSV() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "1970-01-01,00:00:00";
  }
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%d,%H:%M:%S", &timeinfo);
  return String(buf);
}

void ensureLogFile() {
  if (!SPIFFS.exists("/log.csv")) {
    File f = SPIFFS.open("/log.csv", FILE_WRITE);
    if (f) {
      f.println("date,time,distance_cm");
      f.close();
    }
  }
}

bool appendLogLine(const String& line)
{
  File f = SPIFFS.open("/log.csv", FILE_APPEND);
  if (!f) return false;
  f.println(line);
  f.close();
  return true;
}

String readLastNLines(int n) 
{
  File f = SPIFFS.open("/log.csv", FILE_READ);
  if (!f) return "No data";

  String content = f.readString();
  f.close();

  return content;   // show full log
}

void handleRoot() 
{
  String page =
    "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<title>Ultrasonic Logger</title>"
    "<style>"
    "body{font-family:Arial;margin:16px;} button{width:100%;padding:14px;font-size:18px;margin:8px 0;}"
    "pre{background:#f4f4f4;padding:12px;border-radius:10px;overflow:auto;}"
    "</style></head><body>"
    "<h2>Ultrasonic NOTE Logger</h2>"
    "<button onclick=\"location.href='/note'\">NOTE (Save Reading)</button>"
    "<button onclick=\"if(confirm('Clear all readings?')) location.href='/clear';\">Clear Log</button>"
    "<h3>Saved Readings</h3>"
    "<pre>";

  page += readLastNLines(50);

  page += "</pre></body></html>";

  server.send(200, "text/html", page);
}

void handleNote() 
{
  float d = readDistanceCM();

  String dt = getDateTimeCSV();
  int comma = dt.indexOf(',');
  String date = dt.substring(0, comma);
  String timeStr = dt.substring(comma + 1);

  String distStr = (d < 0) ? "NO_ECHO" : String(d, 2);
  String line = date + "," + timeStr + "," + distStr;

  appendLogLine(line);

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleClear()
{
  SPIFFS.remove("/log.csv");
  ensureLogFile();
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() 
{
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(TRIG, LOW);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed!");
  }
  ensureLogFile();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("Open this IP in mobile browser: ");
  Serial.println(WiFi.localIP());

  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");

  server.on("/", handleRoot);
  server.on("/note", handleNote);
  server.on("/clear", handleClear);

  server.begin();
}

void loop() {
  server.handleClient();
}
