#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include "main.h"

#define CONNECT_TIME_OUT 10

typedef enum {
	READ,
	WRITE
} SMT_Command; 

SMT_Command cmd;

String webAppUrl = "https://htn-server.onrender.com";
WiFiManager wifiManager;
HTTPClient http;

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

    http.setReuse(true); // allow reuse (if server supports it)
	http.setConnectTimeout(5000);

	http.begin(webAppUrl);
	http.GET();
	Serial.println("handshake success");
	http.end();
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
		cmd = READ;
		path = "/system/ESP32/read";
	} else if(query.indexOf("cmd=writeRes") == 0) {
		cmd = WRITE;
		path = "/admin/ESP32/write";
	}
	else {
		Serial.println("ignore");
		return;
	}

	String tempUrl = webAppUrl + path + "?secretKey=R1IC7I58XKKXPPAJXAGMGDJ3KWUI7U&" + query;
	http.begin(tempUrl);
	Serial.println("GET");
	int responseCode = http.GET();
	Serial.printf("response code: %d\n", responseCode);
	String data = http.getString();

	if(cmd == READ) 
		Serial2.print(data);

	Serial.println(data);
	http.end();
}

void readHandler() {
	
}

void wifiIndicator(void* parameters) {
	while(1) {
		if(WiFi.status() != WL_CONNECTED)
			for(int i = 0; i < 10; i++) {
			digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
			vTaskDelay(100 / portTICK_PERIOD_MS);
			}
		else {
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			digitalWrite(BUILTIN_LED, HIGH);
		}
	}
}

void checkWriteRequest(void* parameters) {
	HTTPClient httpCheckRequest;
	httpCheckRequest.setConnectTimeout(5000);
	httpCheckRequest.setReuse(true); // allow reuse (if server supports it)
	while(1) {
		if(WiFi.status() != WL_CONNECTED) 
			continue;

		httpCheckRequest.begin(webAppUrl + "/admin/ESP32/write?cmd=writeReq&secretKey=R1IC7I58XKKXPPAJXAGMGDJ3KWUI7U");
		// Serial.println("GET");
		int responseCode = httpCheckRequest.GET();
		// Serial.printf("response code: %d\n", responseCode);
		String data = httpCheckRequest.getString();

		if(data.indexOf("cmd=write") != -1) {
			Serial2.print(data);
			Serial.println(data);
		}
		
		httpCheckRequest.end();
	}
}