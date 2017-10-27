#include <Wire.h>
#include <DS1338.h>

void setup()
{

  // Initialize the I2C bus
  Wire.begin();

  // Initialize the serial interface
  Serial.begin(74880);
  while (!Serial)
    ;

  // Once serial has been inialized, print a welcome message
  Serial.println("Maxim DS1338 Square Wave Example");
  ds1338_clean_osf();
  ds1338_disable_sqw();
}

void loop()
{
  Serial.println("DS1338_SQW_32768HZ");
  ds1338_enable_sqw(DS1338_SQW_32768HZ);
  delay(10000);
  Serial.println("DS1338_SQW_8192HZ");
  ds1338_enable_sqw(DS1338_SQW_8192HZ);
  delay(10000);
  Serial.println("DS1338_SQW_4096HZ");
  ds1338_enable_sqw(DS1338_SQW_4096HZ);
  delay(10000);
  Serial.println("DS1338_SQW_1HZ");
  ds1338_enable_sqw(DS1338_SQW_1HZ);
  delay(10000);
  Serial.println("DS1338_SQW_0HZ");
  ds1338_disable_sqw();
  delay(5000);
}
