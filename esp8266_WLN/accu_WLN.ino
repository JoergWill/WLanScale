int16_t cnt = 0;
int32_t ain;
float accu = 0;
float level = 1;

void loopAkku() {
	cnt += 1;
	ain += analogRead(A0);

	if (cnt >= 10000) {
		accu = ain / cnt;

		level = (accu * 14) / 1024;
		if (level > 14) level = 14;
		if (level < 1) level = 1;
		Serial.print("Akku ");
		Serial.print(level);
		Serial.print(" ");
		Serial.println(accu);
		// reset counter
		ain = 0;
		cnt = 0;
	}
}