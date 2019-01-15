
// WLAN-CONNECT
#ifdef  WINBACK
char SSDI[] = "WinBack";
char PASS[] = "de2la6de2";
#else
char SSDI[] = "WillWeinstadt";
char PASS[] = "de2!a6DE2";
#endif //  WINBACK

uint16_t PORT = 1023;
uint32_t timeout = 15000;
uint32_t timeout_wifiserver = 0;

String ip;
WiFiClient client;
WiFiServer server(PORT);

bool alreadyConnected = false;
bool DataOK = false;

void setupTcpIP() {
	Serial.print("Connection to AccessPoint ");
	Serial.println(SSDI);
	Serial.print("Password                  ");
	Serial.println(PASS);

	WiFi.begin(SSDI, PASS);
	// wait for network to start-up
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println();

	server.begin();
	server.setNoDelay(true); // does not do much if anything
	ip = WiFi.localIP().toString();

	Serial.print("Server startet at        ");
	Serial.println(ip.c_str());
	Serial.println("Listening for connections... ");
}

void loopTcpIP() {
	// Check/Open Connection
	OpenConnection();
	// Send Scale-Data 
	if (client.connected()) {
		SendData();
	}
	// See if we should drop the connection
	if (alreadyConnected && (timeout > 0) && ((millis() - timeout_wifiserver) > timeout)) {
		Serial.print("TimeOut ");
		closeConnection();
	}
}

void SendData() {
	if (client.available() > 4) {	   // WinBack sends MSV?;
		timeout_wifiserver = millis();  // reset timer if we have incoming data
		client.flush(); 			   // NOTE: client.readString braucht >1 Sekunde !!!
		DataOK = true;
		//Logger.Log(LOGLEVEL_DEBUG, "MSV?; ");
	}
	else {
		if (DataOK) {
			client.flush();
			client.println(weight);
			DataOK = false;
			//Logger.Log(LOGLEVEL_DEBUG, "SendData\n %i", weight);
			delay(100);
		}
	}
}

void OpenConnection() {
	// Avoid creating WiFiClient object if no connection,  ESP8266 specific
	if (server.hasClient()) {
		WiFiClient anotherClient = server.available();
		if (!client) {
			client = anotherClient; // take this connection
		}
		else {
			Serial.println("Start extra connection");
			client.stop();
			client = anotherClient;
		}
	}  // anotherClient is disposed here at end of its block

	   // Have client
	if (client) {
		if (!client.connected()) {
			if (alreadyConnected) {
				// client closed so clean up
				Serial.print("Client disconnected ");
				closeConnection();
			}
		}
		else {
			// have connected client
			if (!alreadyConnected) {
				Serial.println("Connection Opened");
				client.setNoDelay(true); // does not do much if anything
				alreadyConnected = true;
				// start timer
				timeout_wifiserver = millis();
			}
		}
	}
}


void closeConnection() {
	Serial.println("Connection Closed");
	alreadyConnected = false;
	client.stop(); // stop client and tcp buffer.
	if (server.hasClient()) {   // avoid creating WiFiClient object if no connection,  ESP8266 specific
		client = server.available();
	} // else just keep client that was just stopped will evaluate to false
}

