/*
 * Project: Vinicunca - Industrial monitoring system
 * Updated code from Si4463 Radio Library for AVR and Arduino (Ping client example)
 * Ping client example author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2017 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/
 *
 * Updated by Emma Dingman and Isabella Gilman, 8/21
 */

/*
 * Ping client
 *
 * Time how long it takes to send some data and get a reply
 * Should be around 5-6ms with default settings
 */

unsigned long timer; // Times when to send accelerometer data
unsigned long current;
#include <Si446x.h>
#include "RTClib.h" // Real time clock library
#include "VCAcc.h" // Accelerometer
#include "Adafruit_ZeroI2S.h" // Microphone

RTC_DS3231 rtc; 
VCAcc a;  
Adafruit_ZeroI2S i2s; //Microphone object



#define CHANNEL 20
#define MAX_PACKET_SIZE 64
#define TIMEOUT 1000

#define PACKET_NONE		0
#define PACKET_OK		1
#define PACKET_INVALID	2
#define SAMPLERATE_HZ 44000

#define DECIMATION 64 
// a windowed sinc filter for 44 khz, 64 samples
uint16_t sincfilter[DECIMATION] = {0, 2, 9, 21, 39, 63, 94, 132, 179, 236, 302, 379, 467, 565, 674, 792, 920, 1055, 1196, 1341, 1487, 1633, 1776, 1913, 2042, 2159, 2263, 2352, 2422, 2474, 2506, 2516, 2506, 2474, 2422, 2352, 2263, 2159, 2042, 1913, 1776, 1633, 1487, 1341, 1196, 1055, 920, 792, 674, 565, 467, 379, 302, 236, 179, 132, 94, 63, 39, 21, 9, 2, 0, 0};
// a manual loop-unroller!
#define ADAPDM_REPEAT_LOOP_16(X) X X X X X X X X X X X X X X X X

int32_t left; // Left channel of microphone
int32_t right; // Right channel of microphone (currently unused, 
	       //could be used in future for dual microphones)



typedef struct{
	uint8_t ready;
	uint32_t timestamp;
	int16_t rssi;
	uint8_t length;
	uint8_t buffer[MAX_PACKET_SIZE];
} pingInfo_t;

static volatile pingInfo_t pingInfo;

void SI446X_CB_RXCOMPLETE(uint8_t length, int16_t rssi)
{
	if(length > MAX_PACKET_SIZE)
		length = MAX_PACKET_SIZE;

	pingInfo.ready = PACKET_OK;
	pingInfo.timestamp = millis();
	pingInfo.rssi = rssi;
	pingInfo.length = length;

	Si446x_read((uint8_t*)pingInfo.buffer, length);

	// Radio will now be in idle mode
}

void SI446X_CB_RXINVALID(int16_t rssi)
{
	pingInfo.ready = PACKET_INVALID;
	pingInfo.rssi = rssi;
}

void setup()
{
  Serial.begin(57600);
  while(!Serial)
  delay(10);
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  //RTC setup
  if(! rtc.begin()) {
   Serial.println("Couldn't find RTC");
   Serial.flush();
   //abort();
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  //Accelorometer setup
  //Indicate you want the sensor connections status printed
  a.printStatus(true);
  //Try connecting to the sensor
  Serial.println("Connecting Sensor");
  a.connectSensor();
  Serial.println("Sensor Connected");
  //Initialize sensor settings to project default
  a.initSettings();
  pinMode(A5, OUTPUT); // LED

  // Microphone setup
  i2s.begin(I2S_24_BIT, SAMPLERATE_HZ * DECIMATION / 16);
  i2s.enableRx();

	// Transceiver start up
	Si446x_init();
	Si446x_setTxPower(SI446X_MAX_TX_POWER);


 timer = millis(); //Start Timer
}

void loop()
{
	static uint8_t counter;
	static uint32_t sent;
	static uint32_t replies;
	static uint32_t timeouts;
	static uint32_t invalids;

  Serial.println("In the loop");
 
 current = millis();

 
	// Make data
	char data[MAX_PACKET_SIZE]; //sends RTC
  memset(data, '\0', sizeof(data));
 
  //Timestamp setup
  DateTime now = rtc.now(); //timestamp is 19 bytes
  String TimeStamp;
  now.timestamp(TimeStamp);
  int TimeStampLength = TimeStamp.length();
  char TimeCharArray[TimeStampLength];
  TimeStamp.toCharArray(TimeCharArray, TimeStampLength+1);
  for(int i = 0; i < TimeStamp.length(); i++)
    {
      data[i] = TimeStamp.charAt(i);
    }

  //Choose to send either Microphone or Accelerometer data
  if(current - timer > 5000) {
    // Send Accelerometer
    data[19] = 'A'; // Tag data type
    a.readValues();
    String accXYZ = a.getAccXYZ();
    for(int i = 0; i < accXYZ.length(); i++)
      {
        data[i + 20] = accXYZ.charAt(i);
      }
  
    //Reset Timer
    timer = millis();
    Serial.println("Accelerometer sending");
  }
  else {
    // Send Microphone
    uint16_t runningsum = 0;
    uint16_t *sinc_ptr = sincfilter;
    data[TimeStampLength + 1] = 'M';
    data[TimeStampLength + 2] = '_';
    i2s.read(&left, &right);
    
    for (uint8_t samplenum=0; samplenum < (DECIMATION/16) ; samplenum++) {
        uint16_t sample = left & 0xFFFF;    // we read 16 bits at a time, by default the low half
        
        ADAPDM_REPEAT_LOOP_16(      // manually unroll loop: for (int8_t b=0; b<16; b++) 
       {
            // start at the LSB which is the 'first' bit to come down the line, chronologically 
            if (sample & 0x1) {
                runningsum += *sinc_ptr;     // do the convolution
            }
               sinc_ptr++;
               sample >>= 1;
            }
          )
        }
    runningsum /= 64 ;
    Serial.println("Microphone sending");

    String strLeft =  String(runningsum);
    for(int i = 0; i < strLeft.length(); i++)
      {
        data[i + TimeStampLength + 3] = strLeft.charAt(i);
      }
  }

	sprintf_P(data, PSTR("test %hhu"), counter);
	counter++;
	Serial.print(F("Sending data"));

	
	uint32_t startTime = millis();

	// Send the data
	uint8_t tx_success = Si446x_TX(data, sizeof(data), CHANNEL, SI446X_STATE_RX);
	sent++;
	
	Serial.println(tx_success ? F("Data sent, waiting for reply...") : F("Failed to send"));
	
	uint8_t success;

	// Wait for reply with timeout
	uint32_t sendStartTime = millis();
	while(1)
	{
		success = pingInfo.ready;
		if(success != PACKET_NONE)
			break;
		else if(millis() - sendStartTime > TIMEOUT) // Timeout // TODO typecast to uint16_t
			break;
	}
		
	pingInfo.ready = PACKET_NONE;

	if(success == PACKET_NONE)
	{
		Serial.println(F("Ping timed out"));
		timeouts++;
	}
	else if(success == PACKET_INVALID)
	{
		Serial.print(F("Invalid packet! Signal: "));
		Serial.print(pingInfo.rssi);
		Serial.println(F("dBm"));
		invalids++;
	}
	else
	{
		// If success toggle LED and send ping time over UART
		uint16_t totalTime = pingInfo.timestamp - startTime;

		static uint8_t ledState;
		digitalWrite(A5, ledState ? HIGH : LOW);
		ledState = !ledState;

		replies++;

		Serial.print(F("Ping time: "));
		Serial.print(totalTime);
		Serial.println(F("ms"));

		Serial.print(F("Signal strength: "));
		Serial.print(pingInfo.rssi);
		Serial.println(F("dBm"));

		// Print out ping contents
		Serial.print(F("Data from server: "));
		Serial.write((uint8_t*)pingInfo.buffer, sizeof(pingInfo.buffer));
		Serial.println();
	}

	Serial.print(F("Totals: "));
	Serial.print(sent);
	Serial.print(F(" Sent, "));
	Serial.print(replies);
	Serial.print(F(" Replies, "));
	Serial.print(timeouts);
	Serial.print(F(" Timeouts, "));
	Serial.print(invalids);
	Serial.println(F(" Invalid"));
	Serial.println(F("------"));

	
}
