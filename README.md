# DS1338_RTC
DS1338 RTC Arduino Library

This library is based on https://github.com/richard-clark/ds1337 project. Thanks.  
The main difference between ds1337 and ds1338:  
1. ds1338 has 56 byte battery backed ram with unlimited writes. 
2. ds1338 has no alarm.
3. ds1338 has VBAT Pin.

The library can be easily migrated to other platform because most of the code is standard C code.  
Note: If you want to use the square wave output, do not forget to add a pullup resistor, because the SQW pin is open drain.  
