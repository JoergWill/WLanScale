#define MSV     0
#define COF8	2



// GPIO14 - D5 - RxD
// GPIO12 - D6 - TxD
SoftwareSerial irSerial(14, 12, false, 512); // RX, TX, inverse, buffsize

uint16_t anzdata;
uint16_t maxdata;

uint16_t state = 0;
uint16_t rs104 = 0;
sint32_t ad104 = 0;
uint16_t ad104_state = 0;


bool responseOK = false;
bool responseERR = false;

uint32_t max_ad104 = 1000;		// Timeout Antwort von AD104
uint32_t rdy_ad104 = 250;		// Pause zwischen zwei Abfragen AD104
uint32_t timeout_ad104 = 0;		// Timeout-Counter


void setupAD104() {
	// Verbindung zur AD104-Waage
	irSerial.begin(9600, 1, 'N');
	
	//TEST
	state = COF8;
}


void loopAD104() {

	// Read serial (6 digits)
	if (irSerial.available() >= 6) {
		anzdata = 0;

		// Read all digits from scale
		while (irSerial.available() > 0) {
			yield();
			anzdata = anzdata + 1;
			int i = irSerial.read();

			//Serial.print(i);
			//Serial.print(" ");

			switch (state) {
			case MSV:
				// Get response from MSV
				read_MSV_AD104(anzdata, i);
				break;
			case COF8:
				// Get response 
				read_CMD_AD104(anzdata, i);
				break;
			}

		}
		//Serial.print(weight_ok);
		//Serial.print(" ");
		Serial.print(weight);
		Serial.print(" ");
		Serial.println(state);
	}
	else {
		// Send Command after Timeout/Received data
		if ((max_ad104 > 0) && ((millis() - timeout_ad104) > max_ad104)) {

			switch (state) {
			case MSV:
				// Send MSV to ad104
				write_MSV_AD104();
				break;
			case COF8:
				write_COF8_AD104();
				break;
			}

			// Start timer
			timeout_ad104 = millis();
		}
	}

	// statemachine AD104
	state_AD104();
}

void state_AD104() {
	switch (state)
	{
	case MSV:
		break;
	case COF8:
		if (responseOK) {
			state = MSV;
		}
	default:
		break;
	}

	// Reset Flags
	responseOK = false;
	responseERR = false;
}

void write_MSV_AD104() {
	irSerial.write(0x4D); // "M"
	irSerial.write(0x53); // "S"
	irSerial.write(0x56); // "V"
	irSerial.write(0x3F); // "?"
	irSerial.write(0x3B); // ";"
}

void write_COF8_AD104() {
	irSerial.write(0x43); // "C"
	irSerial.write(0x4F); // "O"
	irSerial.write(0x46); // "F"
	irSerial.write(0x38); // "8"
	irSerial.write(0x3B); // ";"
}

void read_MSV_AD104(int anzdata, int i) {

	switch(anzdata) {
		case 1:
			ad104 = i * 0x1000000;
			break;
		case 2:
			ad104 = ad104 + i * 0x10000;
			break;
		case 3:
			ad104 = ad104 + i * 0x100;
			break;
		case 4:
			// weight-state ad104
			ad104_state = i;
			break;
		case 5:
			if ((i != 13) && (i != 10)) {
				irSerial.flush();
				anzdata = 0;
				break;
			}
		case 6:
			if ((i != 13) && (i != 10)) {
				irSerial.flush();
				anzdata = 0;
				break;
			}
			else {
				// read actual weigt
				weight = ad104;

				// actual weight-state overload
				weight_ovl_netto = (ad104_state && 0x0001);
				weight_ovl_brutto = (ad104_state && 0x0002);
				weight_ovl_adu = (ad104_state && 0x0004);
				// weight ok 
				weight_ok = (ad104_state && 0x0008);
				// limit-switch
				weight_liv_1 = (ad104_state && 0x0010);
				weight_liv_2 = (ad104_state && 0x0020);
				weight_conf = (ad104_state && 0x0040);

				// send MSV?; after x mSeconds
				timeout_ad104 = millis() - max_ad104 + rdy_ad104;
				break;
			}
		default:
			irSerial.flush();
			anzdata = 0;
			break;
	}
}

void read_CMD_AD104(int anzdata, int i) {
	switch (anzdata) {
	case 1:
		// response from Command
		rs104 = i;
		break;
	case 2:
		if ((i != 13) && (i != 10)) {
			irSerial.flush();
			anzdata = 0;
			break;
		}
	case 3:
		if ((i != 13) && (i != 10)) {
			irSerial.flush();
			anzdata = 0;
			break;
		}
		else {
			// get actual response OK ("0")
			responseOK = (rs104 == 0x30);
			// get actual response ERR ("?")
			responseERR = (rs104 == 0x3F);
			// send next command after x mSeconds
			timeout_ad104 = millis() - max_ad104 + rdy_ad104;
			break;
		}
	default:
		irSerial.flush();
		anzdata = 0;
		break;
	}
}


