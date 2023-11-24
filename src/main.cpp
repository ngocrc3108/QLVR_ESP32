#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include "main.h"

#define CONNECT_TIME_OUT 10

String webAppUrl = "https://htn-server.onrender.com/ESP32/read?";
WiFiManager wifiManager;

void setup() {
	Serial.begin(115200);
	Serial2.begin(115200);
	pinMode(BUILTIN_LED, OUTPUT);
	digitalWrite(BUILTIN_LED, LOW);

	xTaskCreate(wifiIndicator, "wifiIndicator", 1000, NULL, 0, NULL);

	// reset settings - wipe stored credentials for testing
	// these are stored by the esp library
	//wifiManager.resetSettings();

	wifiManager.setConnectTimeout(CONNECT_TIME_OUT);
	wifiManager.autoConnect("ESP32", "31082003");
}

void loop() {
	// Serial.println("Here1");
	// while(!Serial2.available());
	//   Serial.println("Here2");
	// Serial2.flush();
	// String text = Serial2.readString();
	// Serial.println(text);
	// HTTPClient http;
	// http.setConnectTimeout(5000);
	// String tempUrl = webAppUrl + "id=ABCD";
	// http.begin(tempUrl);
	// Serial.println("GET");
	// int responseCode = http.GET();
	// Serial.printf("response code: %d\n", responseCode);
	// String data = http.getString();
	// Serial.println(data);
	// Serial2.print(data);
	// http.end();
	Serial.println("Hello world");
	Serial.println(WiFi.status());
	delay(5000);
}

void wifiIndicator(void * parameters) {
	while(1) {
	if(WiFi.status() != WL_CONNECTED)
		for(int i = 0; i < 5; i++) {
		digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
		vTaskDelay(200 / portTICK_PERIOD_MS);
		}
	else {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		digitalWrite(BUILTIN_LED, HIGH);
	}
	}
}