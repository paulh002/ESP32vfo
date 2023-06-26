/*
 Name:		ESP32vfo.ino
 Created:	11/14/2021 1:09:05 PM
 Author:	paulh
*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <TCA9548.h>
#include <RotaryEncoder.h>
#include "gui.h"
//#include "credentials.h"
#include "measurement.h"
#include "network.h"
#include "io.h"
#include "vfo.h"
#include "_EEPROMAnything.h"
#include "FT891_CAT.h"

#define NUMBR_SSID 1
const char* _ssid[] = { "Network" };
const char* _password[] = { "password" };

SemaphoreHandle_t swrBinarySemaphore = NULL;
SemaphoreHandle_t NetSemaphore = NULL;

#define COLDSTART_REF          0x20 // When started, the firmware examines this "Serial Number
// and enforces factory reset if there is a mismatch.
// Rolling this value is useful if the EEPROM structure has been modified

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

extern var_t	R;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200); /* prepare for possible serial debug */
	swrBinarySemaphore = xSemaphoreCreateMutex();
	if (swrBinarySemaphore == NULL) {
		Serial.println("Error creating the swrBinarySemaphore");
	}
	net_queue = xQueueCreate(1, sizeof(uint8_t));
	if (net_queue == NULL) {
		Serial.println("Error creating the queue");
	}

	pinMode(TXRX_SWITCH, INPUT);
	pinMode(S_METER, ANALOG);
	pinMode(FWD_METER, ANALOG);
	pinMode(REV_METER, ANALOG);
	adc_init();
	memset(&R, 0, sizeof(R));
	init_io();
	init_vfo(); // load default values for R
	CAT.begin(true);
	start_vfo();
	//LoadEEPROM();
	gui.gui_setup();

	R.wifi_onoff = false;
	delay(10);
	if (strlen(R.ssid[0]) == 0) // If no ssid configured take from credentials.h
	{
		for (int i = 0; i < NUMBR_SSID; i++)
		{
			strcpy(R.ssid[i], _ssid[i]);
			strcpy(R.password[i], _password[i]);
		}
	}
	if (R.wifi_onoff)
	{
		for (int i = 0; i < NUMBR_SSID; i++)
		{
			if (strlen(R.ssid[i]))
			{
				Serial.println("Start Wifi");
				if (vfo_network.begin(TRXNET_SERVER))
				{
					return;
				}
			}
		}
	}
}

// the loop function runs over and over again until power down or reset
void loop() {

	if (net_queue)
	{ // Check if we need to start or stop wifi
		uint8_t message;
		if (xQueueReceive(net_queue, &message, 0))
		{
			if (message == 0)
			{
				// stopwifi
				vfo_network.network_stop();
			}
			if (message == 1)
			{
				// startwifi
				vfo_network.begin(TRXNET_SERVER);
			}
		}
	}
	vfo_network.network_loop();
	delay(5);
}


void LoadEEPROM()
{
	uint8_t coldstart;

	EEPROM.begin(sizeof(var_t) + sizeof(uint8_t));

	coldstart = EEPROM.read(0);               // Grab the coldstart byte indicator in EEPROM for
											   // comparison with the COLDSTART_REFERENCE

	Serial.println(String("Coldstart : ") + String(coldstart));

	// Initialize all memories if first upload or if COLDSTART_REF has been modified
	// either through PSWR_A.h or through Menu functions

	if (coldstart == COLDSTART_REF - 1)
	{

		EEPROM_readAnything(1, R);               // Read the stored data
		long  current_frq1[] = { 1800000,3500000,5350000,7000000,10100000,14000000,18068000,21000000,28000000 };
		long  current_frq2[] = { 1800000,3500000,5350000,7000000,10100000,14000000,18068000,21000000,28000000 };

		memcpy(R.current_frq1, current_frq1, sizeof(current_frq1));
		memcpy(R.current_frq2, current_frq2, sizeof(current_frq2));
		return;
	}

	if (coldstart != COLDSTART_REF)
	{
		EEPROM.write(0, COLDSTART_REF);          // COLDSTART_REF in first byte indicates all initialized
		EEPROM_writeAnything(1, R);              // Write default settings into EEPROM
		EEPROM.commit();
	}
	else                                      // EEPROM contains stored data, retrieve the data
	{
		EEPROM_readAnything(1, R);               // Read the stored data
	}
}

void SaveEEPROM()
{
	EEPROM.write(0, COLDSTART_REF);          // COLDSTART_REF in first byte indicates all initialized
	EEPROM_writeAnything(1, R);              // Write default settings into  
	EEPROM.commit();
}
