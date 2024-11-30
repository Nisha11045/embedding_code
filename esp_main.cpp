#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6vDy3JGgi"
#define BLYNK_TEMPLATE_NAME "ESP32 Main"
#define BLYNK_AUTH_TOKEN "hWv5ybBN5tI8L9qmAuemgcieHK8yNsas"

#include <Arduino.h>
#include <BlynkSimpleEsp32.h>

int last1 = 0;
int last1buf = 0;
int last2 = 0;
int last2buf = 0;

BlynkTimer timer;
void serverTick();

void setup() {
    Serial.begin(115200);
    Blynk.begin(BLYNK_AUTH_TOKEN, "Rb-i", "frxsne5vz837k");
    timer.setInterval(10000L, serverTick);
}

void serverTick() {
    Blynk.syncVirtual(V11, V12);
}

BLYNK_WRITE(V11) {
    Serial.println("New data from Node 1");
    double raw_data[5];
    for (int i = 0; i < 5; i++) {
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
    last1 = last1buf;
    last1buf = raw_data[4];
}

BLYNK_WRITE(V12) {
    Serial.println("New data from Node 2");
    double raw_data[4];
    for (int i = 0; i < 4; i++) {
        raw_data[i] = param[i].asDouble();
    }
    Serial.printf("%.2f %.2f %.2f %.2f", raw_data[0], raw_data[1], raw_data[2], raw_data[3]);
    Serial.println();

    Blynk.virtualWrite(V4, raw_data[0]);
    Blynk.virtualWrite(V5, raw_data[1]);
    Blynk.virtualWrite(V6, raw_data[2]);
    Blynk.virtualWrite(V8, raw_data[3]);
    Blynk.virtualWrite(V10, last2 != int(raw_data[3]));
    last2 = last2buf;
    last2buf = raw_data[3];
}

void loop() {
    Blynk.run();
    timer.run();
}