#include <SPI.h>
#include <SD.h>

#define SD_CS 13

File file;

void setup() 
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n--- ESP32 Serial to SD Logger ---");
  Serial.println("Initializing SD card...");

  SPI.begin(18, 19, 23, SD_CS);   // SCK, MISO, MOSI, CS

  if (!SD.begin(SD_CS))
  {
    Serial.println("SD Card Initialization Failed!");
    Serial.println("Check wiring + format (FAT32) + try another SD card.");
    while (true) { delay(1000); }
  }

  Serial.println("SD Card Initialized!");
  Serial.println("Type a message in Serial Monitor and press Enter.");
}

void loop()
{
  if (Serial.available()) 
  {
    String text = Serial.readStringUntil('\n');
    text.trim();

    if (text.length() == 0) return;

    file = SD.open("/log.txt", FILE_APPEND);
    if (!file)
    {
      Serial.println("Error opening /log.txt");
      return;
    }

    file.println(text);
    file.close();

    Serial.println("Saved to SD: " + text);
  }
}