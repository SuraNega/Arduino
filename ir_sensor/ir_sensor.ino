// I2C address finding
#include <Wire.h>

void setup()
{
  //Initializing wire
  Wire.begin();
  //Initializing seraial monitor at the baudrate of 9600
  Serial.begin(9600);
}

void loop()
{
  byte err, addr;
  //Declaring variable to detect and count no. of I2C device found
  int devices = 0;
  
  // For loop to try multiple combinations of Address
  for (addr = 1; addr < 127; addr++) 
  {
    Wire.beginTransmission(addr);
    err = Wire.endTransmission();

    if (!err) 
    {
      Serial.print("Address 0x");
      if (addr < 16)
      {
      Serial.print("0");
      }
      Serial.println(addr, HEX);
      devices++;
    }
    else if (err == 4) 
    {
      Serial.print("Error at address 0x");
      if (addr < 16)
      {
      Serial.print("0");
      }
      Serial.println(addr, HEX);
    }
  }
  
  //Exception, when there is no I2C device found
  if (!devices)
  {
  Serial.println("Please connect your I2C device");
  }
  
  //Waiting for 2 seconds
  delay(2000);
}
