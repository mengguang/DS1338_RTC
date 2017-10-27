#include <Wire.h>
#include <DS1338.h>

// A structure to hold the time read from the device
struct rtctime_t time;

// A character array to hold the string representation of the time
char time_str[20];

void setup()
{

  // Initialize the I2C bus
  Wire.begin();

  // Initialize the serial interface
  Serial.begin(74880);
  while (!Serial)
    ;

  // Once serial has been inialized, print a welcome message
  Serial.println("Maxim DS1338 RTC Example");

  ds1338_clean_osf();
  ds1338_disable_sqw();
  
  /*
  // Set the date of the time structure to June 27, 2014 and
  // the time to 18:45:00 (6:45:00 pm)
  make_time(&time, 017, 10, 27, 18, 28, 06);

  // Set the clock
  
  if (ds1338_write_time(&time))
  {
    Serial.println("Unable to set the time--an I2C error has occured.");
  }
  else
  {
    Serial.println("Clock set successfully!");
  }
  */
}

void loop()
{

  // Attempt to read the time.
  if (ds1338_read_time(&time))
  {
    Serial.println("Unable to read time--an I2C error has occured.");
  }
  else
  {
    // The time has been read successfully--print it.
    format_time_str(&time, time_str);
    Serial.println(time_str);
  }

  delay(1000);
}
