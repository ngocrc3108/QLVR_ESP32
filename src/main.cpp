#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include "main.h"

#define CONNECT_TIME_OUT 10

String webAppUrl = "https://htn-server.onrender.com";
WiFiManager wifiManager;

void setup() {
	Serial.begin(115200);
	Serial2.begin(115200);
	pinMode(BUILTIN_LED, OUTPUT);
	digitalWrite(BUILTIN_LED, LOW);

	xTaskCreate(wifiIndicator, "wifiIndicator", 1024, NULL, 0, NULL);
	xTaskCreate(checkWriteRequest, "checkWriteRequest", 1024*5, NULL, 0, NULL);

	// reset settings - wipe stored credentials for testing
	// these are stored by the esp library
	//wifiManager.resetSettings();

	wifiManager.setConnectTimeout(CONNECT_TIME_OUT);
	wifiManager.autoConnect("ESP32", "31082003");
}

void loop() {
	Serial.println("Here1");
	while(!Serial2.available());
	  Serial.println("Here2");
	Serial2.flush();
	String query = Serial2.readString();
	Serial.println(query);

	String path = "";

	if(query.indexOf("cmd=read") == 0) {
		path = "/system/ESP32/read";
	} else if(query.indexOf("cmd=writeRes") == 0)
		path = "/admin/ESP32/write";
	else {
		Serial.println("ignore");
		return;
	}

	HTTPClient http;
	http.setConnectTimeout(5000);
	String tempUrl = webAppUrl + path + "?secretKey=R1IC7I58XKKXPPAJXAGMGDJ3KWUI7U&" + query;
	http.begin(tempUrl);
	Serial.println("GET");
	int responseCode = http.GET();
	Serial.printf("response code: %d\n", responseCode);
	String data = http.getString();
	Serial.println(data);
	Serial2.print(data);
	http.end();
}

void readHandler() {
	
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

void checkWriteRequest(void * parameters) {
	while(1) {
		if(WiFi.status() == WL_CONNECTED) {
		HTTPClient http;
		http.setConnectTimeout(5000);
		http.begin(webAppUrl + "/admin/ESP32/write?cmd=writeReq&secretKey=R1IC7I58XKKXPPAJXAGMGDJ3KWUI7U");
		Serial.println("GET");
		int responseCode = http.GET();
		Serial.printf("response code: %d\n", responseCode);
		String data = http.getString();

		if(data.indexOf("cmd=write") != -1) 
			Serial2.print(data);

		Serial.println(data);
		http.end();
		}
	}
}