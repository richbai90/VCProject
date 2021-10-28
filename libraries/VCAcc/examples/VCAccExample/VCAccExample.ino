//Include statement to reference library to be used.
#include <VCAcc.h>

//Creating an instance of the accelerometer named a
VCAcc a;


void setup() {
  //Set baud rate
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  //Indicate you want the sensor connections status printed
  a.printStatus(true);

  //Try connecting to the sensor
  a.connectSensor();

  //Initialize sensor settings to project default
  a.initSettings();
}

void loop() { 
  //Read and store current accelerometer values, must do this every time you want to get data.
  a.readValues();

  //Print all axes in serial plotter friendly format.
  Serial.print("X:");             
  Serial.print(a.getAccX());
  Serial.print(","); 
  Serial.print("Y:");
  Serial.print(a.getAccY());
  Serial.print("Z:");
  Serial.print(a.getAccZ());
  Serial.println();
  delayMicroseconds(10000);

}
