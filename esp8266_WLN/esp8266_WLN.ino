/*
 Name:		esp8266_WLN.ino
 Created:	07.01.2019 11:06:51
 Author:	Will
*/

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#define WINBACK

uint32_t BAUD = 115200;
double weight = 0.0;

bool weight_ovl_netto = false;		// Netto-Overflow	(Tara-Wert zu groß)
bool weight_ovl_brutto = false;		// Brutto-Overflow	(Skalierung zu empfindlich)
bool weight_ovl_adu = false;		// ADU-Overflow		(Analogwandler übersteuert)
bool weight_ok = false;				// Waage Stillstand
bool weight_liv_1 = false;			// Grenzwert 1 erreicht/überschritten
bool weight_liv_2 = false;			// Grenzwert 2 erreicht/überschritten
bool weight_conf = false;			// Konfigurations-Fehler (COF)

bool winbackOffline = true;
bool ad104Offline = false;

// the setup function runs once when you press reset or power the board
void setup() {
	
	// startup serial console ...
	Serial.begin(BAUD);
	Serial.println();

	// Setup TCPIP
	setupTcpIP();

	// Setup AD104
	setupAD104();

	//Setup WebServer
	//setupWeb();
}


// the loop function runs over and over again until power down or reset
void loop() {
	// IP-Connection
	loopTcpIP();

	// Serial-Connection
	loopAD104();

	// WebServer
	// loopWeb();

	// Analog Input (Akku)
	//loopAkku();
}







