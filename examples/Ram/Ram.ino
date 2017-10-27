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
  Serial.println("Maxim DS1338 RAM Example");

  //read ram
  uint32_t m = 0;
  if(ds1338_read_ram(DS1338_REG_RAM_BEGIN,(uint8_t *)&m,sizeof(m)) != 0) {
    Serial.println("read ram failed.");
  }
  Serial.print("Last Run Ram data:");
  Serial.println(m);
}

void loop()
{

  uint32_t m = millis();
  if(ds1338_write_ram(DS1338_REG_RAM_BEGIN,(uint8_t *)&m,sizeof(m)) != 0) {
    Serial.println("write ram failed.");
  }

  Serial.println(m);
  
  delay(1000);
}
