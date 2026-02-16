#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

const char* ssid = "keshav";
const char* password = "0987654321";

WebServer server(80);

String getDateTime() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return "Time Not Synced";
  }

  char buffer[30];
  strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
  return String(buffer);
}

void handleRoot() {

  String page = "<!DOCTYPE html><html><head>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += "<title>ESP32 Message Sender</title>";
  page += "<style>";
  page += "body{font-family:Arial;text-align:center;margin-top:40px;}";
  page += "input{padding:10px;width:70%;font-size:16px;}";
  page += "button{padding:10px 20px;font-size:16px;margin-top:10px;}";
  page += "</style></head><body>";

  page += "<h2>ESP32 Message Sender</h2>";
  page += "<form action='/send'>";
  page += "<input type='text' name='msg' placeholder='Enter your message'>";
  page += "<br>";
  page += "<button type='submit'>Send</button>";
  page += "</form>";

  page += "</body></html>";

  server.send(200, "text/html", page);
}

void handleSend()
{

  if (server.hasArg("msg")) {

    String message = server.arg("msg");
    String timeNow = getDateTime();

    Serial.println("=================================");
    Serial.println("Time : " + timeNow);
    Serial.println("Message : " + message);
    Serial.println();
  }

  // Auto return to home page
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {

  Serial.begin(115200);

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

  Serial.println("Waiting for time sync...");
  delay(3000);

  server.on("/", handleRoot);
  server.on("/send", handleSend);

  server.begin();
}

void loop()
{
  server.handleClient();
}