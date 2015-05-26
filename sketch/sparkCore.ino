// what pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11		// DHT 11 
#define DHTTYPE DHT22		// DHT 22 (AM2302)
//#define DHTTYPE DHT21		// DHT 21 (AM2301)


//Attribution des pins E/S
#define DHTPIN 5    
int ledPin = D7;
int input433 = D1;
int inputIR = D0;
int sendPinIR = D3;
int sendPin433 = D6;

unsigned int sendIRDelay = 500; // Delais entre deux codes IR
unsigned long valueTime = 0;
unsigned long value = 0;
int valueBitCount = 0;
int sendIR(String args);
unsigned long previousMillis = 0;  
const long interval = 5000;
double h = 0.00;
float t = 0.00;
char temperature[10];
char humidite[10];

IRsend irsend(sendPinIR); // hardwired to pin 3; use a transistor to drive the IR LED for maximal range
RCSwitch mySwitch = RCSwitch();
DHT dht(DHTPIN, DHTTYPE);

void setup() {
	Spark.function("sendIR", sendIR);
	Spark.function("send433", send433);
	Spark.variable("temperature", temperature, STRING);
	Spark.variable("humidite", humidite, STRING);
	
	pinMode(input433, INPUT_PULLDOWN);
	pinMode(ledPin, OUTPUT);
	pinMode(inputIR, INPUT);
	
	mySwitch.enableReceive(input433);
	mySwitch.enableTransmit(sendPin433);

	attachInterrupt(inputIR, isr, CHANGE);
		
	resetValue();
	dht.begin();    
}

void loop() {
	unsigned long currentMillis = millis();
	
	if(currentMillis - previousMillis >= interval) {
		// save the last time you blinked the LED 
		previousMillis = currentMillis;   
		
		h = dht.getHumidity();
		t = dht.getTempCelcius();
		sprintf(temperature, "%2.1f°C", t);
		sprintf(humidite, "%2.1f%\%", h);

		if (isnan(h) || isnan(t)) {
			t = -99;
			h = -99;
			return;
		}
	}
	//Reception 433Mhz
	int inputPinState = digitalRead(input433);
	digitalWrite(ledPin, inputPinState);

	if (mySwitch.available()) {
		output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(), mySwitch.getReceivedProtocol());
		mySwitch.resetAvailable();
	}
	
	//Reception infrarouge
	if (valueBitCount == 32) {
		Spark.publish("ReceptionIR", String(value), 5, PRIVATE);
        resetValue();
    }
    else if (deltaTime() > 10000L && valueBitCount != 0) {
		resetValue();
    }   
}




//--------------------------------------------------Fonctions--------------------------------------------------
static char *bin2tristate(char *bin) {
	char returnValue[50];
	for (int i=0; i<50; i++) {
		returnValue[i] = '\0';
	}
	int pos = 0;
	int pos2 = 0;
	while (bin[pos]!='\0' && bin[pos+1]!='\0') {
		if (bin[pos]=='0' && bin[pos+1]=='0') {
			returnValue[pos2] = '0';
		} else if (bin[pos]=='1' && bin[pos+1]=='1') {
			returnValue[pos2] = '1';
		} else if (bin[pos]=='0' && bin[pos+1]=='1') {
			returnValue[pos2] = 'F';
		} else {
			return "not applicable";
		}
		pos = pos+2;
		pos2++;
	}
	returnValue[pos2] = '\0';
	return returnValue;
}

void output(unsigned long decimal, unsigned int length, unsigned int delay, unsigned int* raw, unsigned int protocol) {
	if (decimal == 0) {
		Serial.print("Unknown encoding.");
	} else {
		char* b = mySwitch.dec2binWzerofill(decimal, length);
		char* tristate = bin2tristate(b);
		Spark.publish("Reception433Mhz", String(delay) + " " + String(tristate));
	}
}

//---------------------fonction reception IR----------------------
void resetValue() {
	// called from ISR, be careful what you put in this function
	value = 0;
	valueBitCount = 0;
}

unsigned long deltaTime() {
	// called from ISR, be careful what you put in this function
	// Since the microsecond timer wraps around once per minute, we need to
	// take into account the wrap case
	unsigned long newTime = micros();
	unsigned long delta;
	if (newTime > valueTime) {
		// Normal case
		delta = newTime - valueTime;
	}
	else {
		// Wraparound case
		delta = 0xffffffffu - valueTime + newTime + 1;
	}
	return delta;    
}

// Called whenever D0 changes state. This is an ISR; don't do much in this function!
void isr() {
    // called from ISR, be careful what you put in this function
    bool inputValue = (digitalRead(inputIR) == HIGH);
    
    if (inputValue) {
        // Rising
        valueTime = micros();    
    }
    else {
		// Falling
        unsigned long delta = deltaTime();
        if (delta < 800) {
            // 0 bit, 562.5 us
            value <<= 1;
            valueBitCount++;
        }
        else
        if (delta < 1900) {
            // 1 bit, 1687.5 us
            value <<= 1;
            value |= 1;
            valueBitCount++;
        }
        else 
        if (delta < 2600) {
            // Repeat bit, 2250 us
            value = 0xffffffffu;
            valueBitCount = 32;
        }
        else {
            // header, 4300 us, reset value
            resetValue();
        }
    }
}

//Fonction envoi IR
int sendIR(String args){
	String code1 = getValue(args, '-', 0);
	String code2 = getValue(args, '-', 1);
	String code3 = getValue(args, '-', 2);

	if (code2 == "" && code3 == ""){
		irsend.sendNEC(args.toInt(), 32);
		return code1.toInt();
	}
	else if (code2 != "" && code3 ==""){
		irsend.sendNEC(code1.toInt(), 32);
		delay(sendIRDelay);
		irsend.sendNEC(code2.toInt(), 32);
		return code2.toInt();
	}
	else if (code3 != ""){
		irsend.sendNEC(code1.toInt(), 32);
		delay(sendIRDelay);
		irsend.sendNEC(code2.toInt(), 32);
		delay(sendIRDelay);
		irsend.sendNEC(code3.toInt(), 32);
		return code3.toInt();
	}
	return 1;   
}

int send433(String args){
	char paramChar[args.length()+1];
	args.toCharArray(paramChar,args.length()+1);
	mySwitch.sendTriState(paramChar);
	return 1;
}

String getValue(String data, char separator, int index){
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length()-1;

	for(int i=0; i<=maxIndex && found<=index; i++){
		if(data.charAt(i)==separator || i==maxIndex){
			found++;
			strIndex[0] = strIndex[1]+1;
			strIndex[1] = (i == maxIndex) ? i+1 : i;
		}
	}
	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

