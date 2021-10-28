/*
VCAcc.cpp - Library for ISM330DHCX accelerometer in the VC project.
Wrapper library for Adafruit ISM330DHCX set up to VC project specs.

*/

#include <Adafruit_ISM330DHCX.h>
#include "Arduino.h"
#include <Wire.h>
#include <VCAcc.h>

/* Default constructor*/
VCAcc::VCAcc() { }

/* Turns serial monitor printing on or off*/
void VCAcc::printStatus(bool p) {
	printMessage = p;
}

/* Attempts to connect to sensor. If print is active, will print status to serial monitor.
	Returns true for successful connection. False otherwise.*/
bool VCAcc::connectSensor() {
	bool connect = a.begin_I2C();

	if (printMessage) {
		if (connect)
			Serial.println("ISM330DHCX Found!");
		else
			Serial.println("No ISM330DHCX chip detected. Check connection.");
	}

	return connect;
}

/* Initializes the sensor settings to the desired values. */
void VCAcc::initSettings() {
	a.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
	a.setAccelDataRate(LSM6DS_RATE_104_HZ);
}

/* Changes the accelerometer g-range. Parameter should be 2, 4, 8, or 16.*/
void VCAcc::changeAccRange(int accRange) {
	switch (accRange) {
		case 2:
			a.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
			break;
		case 4:
			a.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
			break;
		case 8:
			a.setAccelRange(LSM6DS_ACCEL_RANGE_8_G);
			break;
		case 16:
			a.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
			break;
		default:
			if (printMessage)
				Serial.println("Entered range setting is invalid, previous settings retained.");
			break;
	}
}

/* Changes the gyroscope range. Parameter is DPS and should be 125, 250, 500, 1000, 2000, or 4000. */
void VCAcc::changeGyroRange(int gyroRange) {
	switch (gyroRange) {
		case 125:
			a.setGyroRange(LSM6DS_GYRO_RANGE_125_DPS);
			break;
		case 250:
			a.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
			break;
		case 500:
			a.setGyroRange(LSM6DS_GYRO_RANGE_500_DPS);
			break;
		case 1000:
			a.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS);
			break;
		case 2000:
			a.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
			break;
		case 4000:
			a.setGyroRange(ISM330DHCX_GYRO_RANGE_4000_DPS);
			break;
		default:
			if (printMessage)
				Serial.println("Entered range setting is invalid, previous settings retained.");
			break;
	}
}
/* Changes the data rate of the accelerometer. Valid rates are: 0 (off), 12 (for 12.5 since int type), 
	26, 52, 104, 208, 416, 833, 1660, 3330, and 6660 */
void VCAcc::changeAccRate(int accRate) {
	switch (accRate) {
		case 0:
			a.setAccelDataRate(LSM6DS_RATE_SHUTDOWN);
			break;
		case 12:
			a.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
			break;
		case 26:
			a.setAccelDataRate(LSM6DS_RATE_26_HZ);
			break;
		case 52:
			a.setAccelDataRate(LSM6DS_RATE_52_HZ);
			break;
		case 104:
			a.setAccelDataRate(LSM6DS_RATE_104_HZ);
			break;
		case 208:
			a.setAccelDataRate(LSM6DS_RATE_208_HZ);
			break;
		case 416:
			a.setAccelDataRate(LSM6DS_RATE_416_HZ);
			break;
		case 833:
			a.setAccelDataRate(LSM6DS_RATE_833_HZ);
			break;
		case 1660:
			a.setAccelDataRate(LSM6DS_RATE_1_66K_HZ);
			break;
		case 3330:
			a.setAccelDataRate(LSM6DS_RATE_3_33K_HZ);
			break;
		case 6660:
			a.setAccelDataRate(LSM6DS_RATE_6_66K_HZ);
			break;
		default:
			if (printMessage)
				Serial.println("Entered data rate is invalid, previous settings retained.");
			break;
	}

}
/* Changes the data rate of the gyroscope. Valid rates are: 0 (off), 12 (for 12.5 since int type),
	26, 52, 104, 208, 416, 833, 1660, 3330, and 6660 */
void VCAcc::changeGyroRate(int gyroRate) {
	switch (gyroRate) {
	case 0:
		a.setGyroDataRate(LSM6DS_RATE_SHUTDOWN);
		break;
	case 12:
		a.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
		break;
	case 26:
		a.setGyroDataRate(LSM6DS_RATE_26_HZ);
		break;
	case 52:
		a.setGyroDataRate(LSM6DS_RATE_52_HZ);
		break;
	case 104:
		a.setGyroDataRate(LSM6DS_RATE_104_HZ);
		break;
	case 208:
		a.setGyroDataRate(LSM6DS_RATE_208_HZ);
		break;
	case 416:
		a.setGyroDataRate(LSM6DS_RATE_416_HZ);
		break;
	case 833:
		a.setGyroDataRate(LSM6DS_RATE_833_HZ);
		break;
	case 1660:
		a.setGyroDataRate(LSM6DS_RATE_1_66K_HZ);
		break;
	case 3330:
		a.setGyroDataRate(LSM6DS_RATE_3_33K_HZ);
		break;
	case 6660:
		a.setGyroDataRate(LSM6DS_RATE_6_66K_HZ);
		break;
	default:
		if (printMessage)
			Serial.println("Entered data rate is invalid, previous settings retained.");
		break;
	}
}

/* Records the current values of the IMU */
void VCAcc::readValues() {
	a.getEvent(&accData, &gyroData, &temp);
}

/* Returns X value of Accelerometer */
double VCAcc::getAccX() {
	return accData.acceleration.x;
}

/* Returns Y value of Accelerometer */
double VCAcc::getAccY() {
	return accData.acceleration.y;
}

/* Returns Z value of Accelerometer */
double VCAcc::getAccZ() {
	return accData.acceleration.z;
}

/* Returns X value of gyroscope */
double VCAcc::getGyroX() {
	return gyroData.gyro.x;
}

/* Returns Y value of gyroscope */
double VCAcc::getGyroY() {
	return gyroData.gyro.y;
}

/* Returns Z value of gyroscope */
double VCAcc::getGyroZ() {
	return gyroData.gyro.z;
}

/* Returns the temperature */
double VCAcc::getTemp() {
	return temp.temperature;
}

/* Returns X, Y, and Z value of Accelerometer as a String */
String VCAcc::getAccXYZ() {
	String result = "";
	result.concat(getAccX());
	result.concat(",");
	result.concat(getAccY());
	result.concat(",");
	result.concat(getAccZ());
	return result;
}