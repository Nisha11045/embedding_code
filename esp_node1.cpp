#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID      "TMPL6FkoEitWB"
#define BLYNK_TEMPLATE_NAME    "ESP32 Sensor 1"
#define BLYNK_AUTH_TOKEN       "aanpYaDhcNIyAMfuJUi1lP0MlPg8yBfG"

#include <Arduino.h>
#include "DHT.h"
#include <BlynkSimpleESP32.h>

#define DHTPIN 27
#define DHTTYPE DHT11
//DHTTYPE = DHT11, but there are also DHT22 and 21
float totaltemp = 0;
float totalhum = 0;
unsigned long validsamples = 0;

#define PM10PIN 34
#define PM25PIN 35
unsigned long duration;
unsigned long samplestart;
unsigned long sampleend;
unsigned long sampletime = 10000;
unsigned long occupancy10 = 0;
unsigned long occupancy25 = 0;

DHT dht(DHTPIN, DHTTYPE); // constructor to declare our sensor

BlynkTimer timer;

void serverTick();

void setup() {
  pinMode(PM10PIN, INPUT);
  pinMode(PM25PIN, INPUT);
  Serial.begin(115200);
  dht.begin();
  samplestart = millis();
  Blynk.begin(BLYNK_AUTH_TOKEN, "Rb-i", "frxsne5vz837k");
  timer.setInterval(1000L, serverTick);
}

void serverTick() {
  // The DHT11 returns at most one measurement every 1s
  float h = dht.readHumidity();
  //Read the moisture content in %.
  float t = dht.readTemperature();
  //Read the temperature in degrees Celsius

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed reception");
    return;
    //Returns an error if the ESP32 does not receive any measurements
  }

  validsamples++;
  totaltemp += t;
  totalhum += h;

  Serial.printf("Humidity %.2f%% | Temperature %.2f°C", h, t);
  Serial.println();
  // Transmits the measurements received in the serial monitor

  occupancy10 += pulseIn(PM10PIN, LOW);
  occupancy25 += pulseIn(PM25PIN, LOW);
  sampleend = millis();
  if ((sampleend - samplestart) >= sampletime) {
    float ratio10 = (occupancy10 - sampleend + samplestart + sampletime) / (sampletime * 10.0);
    float concentration10 = 1.1 * pow(ratio10, 3) - 3.8 * pow(ratio10, 2) + 520 * ratio10 + 0.62;
    float ratio25 = (occupancy25 - sampleend + samplestart + sampletime) / (sampletime * 10.0);
    float concentration25 = 1.1 * pow(ratio25, 3) - 3.8 * pow(ratio25, 2) + 520 * ratio25 + 0.62;

    Serial.printf("PM1.0 %.0f | PM2.5 %.0f", concentration10, concentration25);
    Serial.println();

    occupancy10 = 0;
    occupancy25 = 0;
    samplestart = millis();

    float datahum = totalhum / validsamples;
    float datatemp = totaltemp / validsamples;

    Blynk.virtualWrite(V0, datahum);
    Blynk.virtualWrite(V1, datatemp);
    Blynk.virtualWrite(V2, concentration10);
    Blynk.virtualWrite(V3, concentration25);

    Blynk.virtualWrite(V4, datahum, datatemp, concentration25, concentration10, millis() / 1000);

    validsamples = 0;
    totaltemp = 0;
    totalhum = 0;
  }
  Blynk.virtualWrite(V5, round(millis() / 1000));
}

void loop() {
  Blynk.run();
  timer.run();
}