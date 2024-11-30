#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6vDy3JGgi"
#define BLYNK_TEMPLATE_NAME "ESP32 Main"
#define BLYNK_AUTH_TOKEN "hWv5ybBN5tI8L9qmAuemgcieHK8yNsas"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <map>
#include <Keypad.h>
String Web_App_URL = "https://script.google.com/macros/s/AKfycbxka-r2PtzPzcCwZP3QdiTzR44ibV_QulRlY3MqlOGV4_u1V4cF0RE0VYZeY4C-QmKX/exec";
const byte ROW_NUM = 4;
const byte COL_NUM = 4;
char keys[ROW_NUM][COL_NUM] = {
    {'1', '2', '3', 'U'},
    {'4', '5', '6', 'D'},
    {'7', '8', '9', 'C'},
    {'L', '0', 'R', 'E'}};
byte pin_rows[ROW_NUM] = {32, 33, 25, 26}; // Row pins (D26, D25, D33, D32)
byte pin_cols[COL_NUM] = {13, 12, 14, 27}; // Column pins (D13, D12, D14, D27)
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_cols, ROW_NUM, COL_NUM);
const char *ssid = "GalaxyWarich";
const char *password = "Warich2264";
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000); // Offset for Thailand (GMT+7)
const int maxRetries = 20;
#include <Arduino.h>
#include <BlynkSimpleEsp32.h>

double Humidity = 0.0;
double Temperature = 0.0;
double PM25 = 0.0;
double PM10 = 0.0;
double Light = 0.0;
double Sound = 0.0;
double Carbon = 0.0;

int last1 = 0;
int last1buf = 0;
int last2 = 0;
int last2buf = 0;

// BlynkTimer timer;
// void serverTick();

void setup()
{
  Serial.begin(115200);
  // Blynk.begin(BLYNK_AUTH_TOKEN, "Rb-i", "frxsne5vz837k");
  // timer.setInterval(10000L, serverTick);

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  WiFi.begin(ssid, password);
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries)
  {
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("Retry: ");
    lcd.print(++retryCount);
  }
  lcd.clear();
  if (WiFi.status() == WL_CONNECTED)
  {
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!");
    lcd.setCursor(0, 1);
    lcd.print("Restart device");
    while (true)
      ;
  }
  timeClient.begin();
  lcd.clear();
}

// void serverTick()
// {
//   Blynk.syncVirtual(V11, V12);
// }

BLYNK_WRITE(V11)
{
  Serial.println("New data from Node 1");
  double raw_data[5];
  for (int i = 0; i < 5; i++)
  {
    raw_data[i] = param[i].asDouble();
  }
  Serial.printf("%.2f %.2f %.2f %.2f %.2f", raw_data[0], raw_data[1], raw_data[2], raw_data[3], raw_data[4]);
  Serial.println();

  Blynk.virtualWrite(V0, raw_data[0]);
  Blynk.virtualWrite(V1, raw_data[1]);
  Blynk.virtualWrite(V2, raw_data[2]);
  Blynk.virtualWrite(V3, raw_data[3]);
  Blynk.virtualWrite(V7, raw_data[4]);
  Blynk.virtualWrite(V9, last1 != int(raw_data[4]));

  Humidity = raw_data[0];
  Temperature = raw_data[1];
  PM25 = raw_data[2];
  PM10 = raw_data[3];

  last1 = last1buf;
  last1buf = raw_data[4];
}

BLYNK_WRITE(V12)
{
  Serial.println("New data from Node 2");
  double raw_data[4];
  for (int i = 0; i < 4; i++)
  {
    raw_data[i] = param[i].asDouble();
  }
  Serial.printf("%.2f %.2f %.2f %.2f", raw_data[0], raw_data[1], raw_data[2], raw_data[3]);
  Serial.println();

  Blynk.virtualWrite(V4, raw_data[0]);
  Blynk.virtualWrite(V5, raw_data[1]);
  Blynk.virtualWrite(V6, raw_data[2]);
  Blynk.virtualWrite(V8, raw_data[3]);
  Blynk.virtualWrite(V10, last2 != int(raw_data[3]));

  Light = raw_data[0];
  Sound = raw_data[1];
  Carbon = raw_data[2];

  last2 = last2buf;
  last2buf = raw_data[3];
}

char buffer_key = '1';
double value = 0;

std::map<char, std::pair<String, String>> map_int_toString = {
    {'1', {"Humidity", "%"}},
    {'2', {"Temp", "C"}},
    {'3', {"PM25", "PM"}},
    {'4', {"PM10", "PM"}},
    {'5', {"Light", "L"}},
    {'6', {"Sound", "S"}},
    {'7', {"Carbon", "C"}},
};

void updateLCDRow1(char map, double value)
{
  if (map_int_toString.find(map) != map_int_toString.end())
  {                      // Check if key exists in map
    lcd.setCursor(0, 1); // Set cursor for the second row (line 1)
    lcd.print(map_int_toString[map].first);
    lcd.print(":");
    lcd.print(value);
    if (map_int_toString[map].first == "Temp")
    {
      lcd.print((char)223);
    }
    lcd.print(map_int_toString[map].second);
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("Invalid key"); // Handle invalid key press
  }
}

const unsigned long debounceDelay = 50;        // Debounce delay in milliseconds
unsigned long lastPressTime = 0;               // Store the time of the last key press
unsigned long lastTimeUpdate = 0;              // Time when the time was last updated
const unsigned long timeUpdateInterval = 1000; // Update time every 1 second

void loop()
{
  // Blynk.run();
  // timer.run();

  unsigned long currentMillis = millis();
  char key = keypad.getKey();
  if (key)
  {
    if (currentMillis - lastPressTime > debounceDelay)
    {
      buffer_key = key;
      lastPressTime = currentMillis;
    }
  }

  if (currentMillis - lastTimeUpdate >= timeUpdateInterval)
  {
    lcd.clear();

    // update LCD Row0
    timeClient.update();
    String currentTime = timeClient.getFormattedTime(); // Format: HH:MM:SS
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    lcd.print(currentTime);

    // update LCD Row1
    std::vector<double> sensorValues = {Humidity, Temperature, PM25, PM10, Light, Sound, Carbon};
    int int_Value = buffer_key - '0'; // Convert char to int ('1' -> 1, '2' -> 2, etc.)
    // Check if the key is valid (1-7)
    if (int_Value >= 1 && int_Value <= 7)
    {
      double value = sensorValues[int_Value - 1]; // Get the corresponding sensor value
      updateLCDRow1(buffer_key, value);           // Update LCD with the value
    }

    //----------------------------------------Conditions that are executed when WiFi is connected.
    // This condition is the condition for sending or writing data to Google Sheets.
    if (WiFi.status() == WL_CONNECTED)
    {
      // Create a URL for sending or writing data to Google Sheets.
      String Send_Data_URL = Web_App_URL + "?sts=write";
      Send_Data_URL = Send_Data_URL + "&temp=" + String(Temperature);
      Send_Data_URL = Send_Data_URL + "&humd=" + String(Humidity);
      // Send_Data_URL += "&humd=" + String(PM25);
      // Send_Data_URL += "&humd=" + String(PM10);
      // Send_Data_URL += "&humd=" + String(Light);
      // Send_Data_URL += "&humd=" + String(Sound);
      // Send_Data_URL += "&humd=" + String(Carbon);

      // Initialize HTTPClient as "http".
      HTTPClient http;

      // HTTP GET Request.
      http.begin(Send_Data_URL.c_str());
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

      // Gets the HTTP status code.
      int httpCode = http.GET();
      Serial.print("HTTP Status Code : ");
      Serial.println(httpCode);

      // Getting response from google sheets.
      String payload;
      if (httpCode > 0)
      {
        payload = http.getString();
        Serial.println("Payload : " + payload);
      }

      http.end();
    }

    lastTimeUpdate = currentMillis;
  }
}