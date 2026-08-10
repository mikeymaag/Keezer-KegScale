#include <Arduino.h>
#include "secret.h"
#include "config.h"
#include "KegScaleObj.h"

#include <WiFi.h>
#include <SPI.h>
#include <MQTT.h>

#include <DHT.h>


//Instantiate the temperature probe
DHT dht(DHTPIN, DHTTYPE);

//Instantiate the kegScale objects
KegScaleObj kegScale[KEGCOUNT];

//Initialize script level variables.
float temp = 0.0;
float humidity = 0.0;
bool reconnect = false;
char msgBuffer[20];
//Setup MQTT topic tags
String ipaddrMQTTTag = "home/keezer/kegscale/pv/ipaddr";
String tempPVMQTTTag = "home/keezer/pv/temp";

WiFiClient wifi; // @suppress("Abstract class cannot be instantiated")
MQTTClient mqttClient;


//the setup() function runs once, when powered on...
void setup() {
	Serial.begin(115200);
	Serial.print("\nStarting...");

	//Initialize the kegScale objects
	for (int i = 0; i < KEGCOUNT; i++) {
		kegScale[i].init(i);
	}

	//Turn on temperature probe
	dht.begin();

	//connect to WiFi and MQTT Broker
	connectme();
}

// the loop function runs over and over again forever in an endless loop.
void loop() {
	for (int i = 0; i < KEGCOUNT; i++) {
		kegScale[i].readScale(10, 10, 100);
	}
	sendData();
	mqttClient.loop();
}

// Connect WiFI and MQTT Broker
void connectme() {
	Serial.print("checking wifi...");
	// check for the WiFi module:
	Serial.print("WiFi.status(): ");
	Serial.println(WiFi.status());
	delay(1000);
	//Loop until WiFI connects...
	while (WiFi.status() != WL_CONNECTED) {
		if (reconnect) {
			mqttClient.disconnect();
			reconnect = false;
		}
		WiFi.begin(SECRET_SSID, SECRET_PASS);
		Serial.print("WiFi.status(): ");
		Serial.println(WiFi.status());
		delay(10000);
		Serial.print(".");
	}
	if (WiFi.status() == WL_CONNECTED) {
		// MQTT brokers usually use port 8883 for secure connections.
		mqttClient.begin(BROKER_IP, 1883, wifi);
		mqttClient.onMessage(messageReceived);
		Serial.print("\n MQTT connecting...");
		delay(1000);
		if (!mqttClient.connect(CLIENT_ID, SECRET_MQTTID, SECRET_MQTTPASS)) {
			sprintf(msgBuffer, "%s.%s.%s", CLIENT_ID, SECRET_MQTTID,
					SECRET_MQTTPASS);
			Serial.println(msgBuffer);
		}
	}

	if (!mqttClient.connected()) {
		Serial.println("\nMQTT not connected!");
		return;
	}
	IPAddress ip;
	ip = WiFi.localIP();
	sprintf(msgBuffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	mqttClient.publish(ipaddrMQTTTag, msgBuffer);
	Serial.println(WiFi.localIP());
	for (int i = 0; i < KEGCOUNT; i++) {
		mqttClient.subscribe(kegScale[i].getKegEmptyCountMQTTTag());
		mqttClient.subscribe(kegScale[i].getKegFullCountMQTTTag());
		mqttClient.subscribe(kegScale[i].getScaleCalibrateEmptyMQTTTag());
		mqttClient.subscribe(kegScale[i].getScaleCalibrateFullMQTTTag());
	}
	reconnect = true;
}

// Publish keg data to MQTT...
void sendData() {
	if (mqttClient.connected()) {
		sprintf(msgBuffer, "%.1f",readTempF());
		mqttClient.publish(tempPVMQTTTag, msgBuffer);
		Serial.print(tempPVMQTTTag);
		Serial.print(" = ");
		Serial.println(msgBuffer);
		for (int i = 0; i < KEGCOUNT; i++) {
			sprintf(msgBuffer, "%ld", kegScale[i].getScaleCount());
			mqttClient.publish(kegScale[i].getScaleCountMQTTTag(), msgBuffer);
			Serial.print(kegScale[i].getScaleCountMQTTTag());
			Serial.print(" = ");
			Serial.println(msgBuffer);

			sprintf(msgBuffer, "%.2f", kegScale[i].getGallons());
			mqttClient.publish(kegScale[i].getKegGallonsMQTTTag(), msgBuffer);
			Serial.print(" ");
			Serial.print(kegScale[i].getKegGallonsMQTTTag());
			Serial.print(" = ");
			Serial.println(msgBuffer);

			sprintf(msgBuffer, "%.1f", kegScale[i].getPercentFull());
			mqttClient.publish(kegScale[i].getKegPercentFullMQTTTag(),
					msgBuffer);
			Serial.print("  ");
			Serial.print(kegScale[i].getKegPercentFullMQTTTag());
			Serial.print(" = ");
			Serial.println(msgBuffer);

			sprintf(msgBuffer, "%ld", kegScale[i].getGlassesLeft());
			mqttClient.publish(kegScale[i].getKegGlassesLeftMQTTTag(),
					msgBuffer);
			Serial.print("   ");
			Serial.print(kegScale[i].getKegGlassesLeftMQTTTag());
			Serial.print(" = ");
			Serial.println(msgBuffer);

			sprintf(msgBuffer, "%ld", kegScale[i].getEmptyCount());
			Serial.print("    ");
			Serial.print(kegScale[i].getKegEmptyCountMQTTTag());
			Serial.print(" = ");
			Serial.println(msgBuffer);

			sprintf(msgBuffer, "%ld", kegScale[i].getFullCount());
			Serial.print("     ");
			Serial.print(kegScale[i].getKegFullCountMQTTTag());
			Serial.print(" = ");
			Serial.println(msgBuffer);
		}
		Serial.println();
	} else {
		connectme();
	}
}

//Callback function to receive incoming MQTT messages...
void messageReceived(String &topic, String &payload) {
	char buf[10];

	Serial.println("incoming: " + topic + " - " + payload);
	payload.toCharArray(buf, 10);

	//Loop through the kegs to identify the keg needing the message
	for (int i = 0; i < KEGCOUNT; i++) {

		//Execute the message when found
		//Empty Count Processing
		if (topic == kegScale[i].getKegEmptyCountMQTTTag()) {
			kegScale[i].setEmptyCount(atol(buf));
			Serial.print("Keg ");
			Serial.print(i + 1);
			Serial.print(" empty count is now = ");
			Serial.println(kegScale[i].getEmptyCount());
		}
		//Full Count Processing
		if (topic == kegScale[i].getKegFullCountMQTTTag()) {
			kegScale[i].setFullCount(atol(buf));
			Serial.print("Keg ");
			Serial.print(i + 1);
			Serial.print(" full count is now = ");
			Serial.println(kegScale[i].getFullCount());
		}

		//Calibrate Empty Processing
		if (topic == kegScale[i].getScaleCalibrateEmptyMQTTTag()) {
			kegScale[i].calibrateEmpty(100, 10, 100);
			sprintf(msgBuffer, "%ld", kegScale[i].getEmptyCount());
			mqttClient.publish(kegScale[i].getKegEmptyCountMQTTTag(), msgBuffer, true, 0);
		}

		//Calibrate Full processing
		if (topic == kegScale[i].getScaleCalibrateFullMQTTTag())  {
			kegScale[i].calibrateFull(100, 10, 100);
			sprintf(msgBuffer, "%ld", kegScale[i].getFullCount());
			mqttClient.publish(kegScale[i].getKegFullCountMQTTTag(), msgBuffer,	true, 0);
		}
	}
}

//Read the temperature...
float readTempF() {
// Get temperature event and print its value.
	float temp = dht.readTemperature(true);
	if (isnan(temp) != 0) {
		Serial.println(F("\nError reading temperature!"));
	} else {
		return temp;
	}
	return 0.0;
}

//Read the humidity...
float readHumidity() {
	float humidity = dht.readHumidity();
	if (isnan(humidity) != 0) {
		Serial.println(F("\nError reading humidity!"));
	} else {
		return humidity;
	}
	return 0.0;
}
