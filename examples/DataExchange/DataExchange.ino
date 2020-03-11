#include <SPI.h>
#include <Ethernet2.h>
#include <IntlerCloud.h>

void printValue(double val) {
  Serial.println("Print value " + String(val));
}

int luminosity, temperature;

Cloud cloud("demo_user", "qwerty", "myArduino");

void setup() {
  Serial.begin(9600);
  
  cloud.connect();
  cloud.setInterval(8000);
  
  cloud.addCommand("motor", printValue);
}

void loop()
{
  cloud.run();
  
  luminosity = analogRead(A0);
  temperature = analogRead(A1);
  
  cloud.sendValue("temperature", temperature);
  cloud.sendValue("light", luminosity);
}
