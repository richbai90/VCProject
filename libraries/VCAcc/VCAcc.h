/*
VCAcc.h - Library for ISM330DHCX accelerometer in the VC project.
Wrapper library for Adafruit ISM330DHCX set up to VC project specs.

*/

#ifndef VCAcc_h
#define VCAcc_h

#include <Adafruit_ISM330DHCX.h>
#include "Arduino.h"
#include <Wire.h>

class VCAcc {

public:
	//Constructor
	VCAcc();
	//Utility functions
	void printStatus(bool p);
	bool connectSensor();
	void initSettings();
	void changeAccRange(int accRange);
	void changeGyroRange(int gyroRange);
	void changeAccRate(int accRate);
	void changeGyroRate(int gyroRate);
	void readValues();
	double getAccX();
	double getAccY();
	double getAccZ();
	double getGyroX();
	double getGyroY();
	double getGyroZ();
	double getTemp();
	String getAccXYZ();
private:
	//Internal variables
	Adafruit_ISM330DHCX a;
	sensors_event_t accData;
	sensors_event_t gyroData;
	sensors_event_t temp;
	bool printMessage;
};

#endif