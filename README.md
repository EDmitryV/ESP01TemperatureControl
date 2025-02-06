# ESP01TemperatureControl
Simple implementation of remote temperature controller on ESP01 relay module and ESP01 temperature module DS18B20. Communication between modules using espnow and the MAC address of the relay module. For the task of maintaining the temperature on the temperature sensor module, a server is launched that is accessible from the local wifi network.
## Required
-ESP-01 Programmer;
-ESP01 Relay module;
-ESP01 DS18B20 temperature module;
-Power supply module 5v 2A ;
-Arduino IDE (about: https://cyberblogspot.com/how-to-program-esp-01-with-arduino-ide/) with necessary libraries (DallasTemperature, OneWire);
-Wifi scaner (for example: https://www.softperfect.com/products/networkscanner/).
## Preparation
In temperature:
1)Change ssid to the name of your wifi network;
2)Change password to the password of your wifi network;
3)Change relayMacAddress using scaner and wifi server sketch (for temperature module or base example).
