#define MSV     0
#define COF8	2

#define SPW		10	// Passwort
#define CWT		11	// Abgleichwert in Prozent vom Endgewicht
#define LDW		12	// Abgleich Nullpunkt
#define LWT		13	// Abgleich Verstärkung
#define NOV		14	// Nominalwert der Waage
#define RSN		15	// Auflösung der Waage
#define TDD		16	// Werte im EEProm speichern


// GPIO14 - D5 - RxD
// GPIO12 - D6 - TxD
SoftwareSerial irSerial(14, 12, false, 512); // RX, TX, inverse, buffsize

uint16_t anzdata;
uint16_t maxdata;

uint16_t state = MSV;
uint16_t rs104 = 0;
sint32_t ad104 = 0;

bool responseOK = false;
bool responseERR = false;
bool ad104wait = false;

uint16_t ad104_state;			// Status AD104 (Waage Unter-/Überlast)
uint32_t max_ad104 = 1000;		// Timeout Antwort von AD104
uint32_t rdy_ad104 = 250;		// Pause zwischen zwei Abfragen AD104
uint32_t timeout_ad104 = 0;		// Timeout-Counter

String MaxGew;
String AbglGew;
String PrzNenn;
String AuflGew = "1";


void setupAD104() {
	// Verbindung zur AD104-Waage
	irSerial.begin(9600, 1, 'N');
	
	// Set state
	state = COF8;
}


void loopAD104() {

	// Read serial (6 digits)
	if (((irSerial.available() >= 6) && (state == MSV)) || ((irSerial.available() >= 3) && (state != MSV))) {
		anzdata = 0;
		// Reset Flag - wait for answer from ad104
		ad104wait = false;

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
			default:
				// Get response 
				read_CMD_AD104(anzdata, i);
				break;
			}

		}

		// statemachine AD104
		state_AD104();

		//Serial.print(weight_ok);
		//Serial.print(" ");
		//Serial.print(weight);
		//Serial.print(" ");
		//Serial.println(state);
	}
	else {
		// Send Command after Timeout/Received data
		if ((max_ad104 > 0) && ((millis() - timeout_ad104) > max_ad104)) {
			// no answer from ad104
			ad104Offline = ad104wait;
			// Set Flag - wait for answer from ad104
			ad104wait = true;

			switch (state) {
			case MSV:
				// Send MSV to ad104
				write_MSV_AD104();
				break;
			case COF8:
				write_COF8_AD104();
				break;

			// Abgleich Nullpunkt
			case SPW:
				// schreibe Passwort
				write_SPW_AD104();
				break;
			case CWT:
				// Abgleichwert in Prozent der Nennlast
				write_CWT_AD104();
				break;
			case LDW:
				// Nullpunkt abgleichen
				write_LDW_AD104();
				break;

			// Abgleich Verstärkung
			case LWT:
				// Abgleich Verstärkung
				write_LWT_AD104();
				break;
			case NOV:
				// Schreibe Nennlast 
				write_NOV_AD104();
				break;
			case RSN:
				// Schreibe Auflösung
				write_RSN_AD104();
				break;
			case TDD:
				// Speichere Werte in EEProm
				write_TDD_AD104();
				break;
			}

			// Start timer
			timeout_ad104 = millis();
		}
	}
}

void state_AD104() {
	if (ad104_null) {state = SPW;}
	if (ad104_span) {state = LWT;}

	switch (state)
	{
	case MSV:
		break;
	case COF8:
		if (responseOK) {
			state = MSV;
		}
		break;

	case SPW:
		if (responseOK) {
			state = CWT;
		}
		break;
	case CWT:
		if (responseOK) {
			state = LDW;
		}
		break;
	case LDW:
		if (responseOK) {
			state = MSV;
		}
		break;

	case LWT:
		if (responseOK) {
			state = NOV;
		}
		break;
	case NOV:
		if (responseOK) {
			state = RSN;
		}
		break;
	case RSN:
		if (responseOK) {
			state = TDD;
		}
		break;
	case TDD:
		if (responseOK) {
			state = MSV;
		}
		break;

	default:
		break;
	}

	// Reset Flags
	responseOK = false;
	responseERR = false;
	ad104_null = false;
	ad104_span = false;
}

void write_MSV_AD104() {
	Send_CMD_AD104("MSV?;");
	max_ad104 = 1000;
}

void write_COF8_AD104() {
	Send_CMD_AD104("COF8;");
}

// Passwort (Freigabe Abgleich)
void write_SPW_AD104() {
	Send_CMD_AD104("SPW\"AED\";");
	max_ad104 = 2000;
}

// Teil-Last-Abgleich - Abgleichgewicht in % der Nennlast
void write_CWT_AD104() {
	String cwt = "CWT";
	cwt.concat(PrzNenn);
	cwt.concat(";");
	Send_CMD_AD104(cwt);
	Serial.println(cwt);
	max_ad104 = 5000;
}

// Abgleich Nullpunkt
void write_LDW_AD104() {
	Send_CMD_AD104("LDW;");
	max_ad104 = 5000;
}

// Abgleich mit Abgleichgewicht
void write_LWT_AD104() {
	Send_CMD_AD104("LWT;");
	max_ad104 = 5000;
}

// Maximal-Last der Waage
void write_NOV_AD104() {
	String nov = "NOV";
	nov.concat(MaxGew);
	nov.concat(";");
	Send_CMD_AD104(nov);
}

// Auflösung
void write_RSN_AD104() {
	String rsn = "RSN";
	rsn.concat(AuflGew);
	rsn.concat(";");
	Send_CMD_AD104(rsn);
}

// Abgleichwerte speichern in EEProm
void write_TDD_AD104() {
	Send_CMD_AD104("TDD1;");
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
				// read actual weight in [gr]
				fWeight = ad104;
				fWeight = fWeight / 256;
				sWeight = String(fWeight/100);

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
			if (responseOK) { Serial.println("ResposeOK");}
			if (responseERR) { Serial.println("ResposeERR"); }
			break;
		}
	default:
		irSerial.flush();
		anzdata = 0;
		break;
	}
}

void Send_CMD_AD104(String cmd) {
	irSerial.print(cmd);

	//Serial.print("State ");
	//Serial.print(state);
	//Serial.print(" Command ");
	//Serial.println(cmd);
}


