# ESP8266-Power-Monitor
An Arduino sketch to connect an [INA219](https://www.aliexpress.com/item/Free-shipping-Zero-drift-CJMCU-219-INA219-I2C-interface-Bi-directional-current-power-monitoring-sensor/32688147341.html) module to an [ESP Dev Board](https://www.aliexpress.com/wholesale?SearchText=esp8266+mini) and measure Voltage, Current, Power, Energy Consumption and Costs via the Blynk mobile app.

Official Blynk Community forum thread: http://community.blynk.cc/t/power-monitor-ina219-blynk/10297

#Installation

##Wifi Credentials

Create a new folder in your library dir (arduino\libraries\wifi_credentials) called 'wifi_credentials' and create a new file called 'wifi_credentials.h' and copy the example below:

```cpp
/*
   Wifi Credentials
*/
#ifndef WIFI_CREDS_H
#define WIFI_CREDS_H
#define WIFI_SSID               "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define WIFI_PASS               "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#endif
```

Replace "xxxxxxx" with your own Wifi network credentials.

##Hardware

* [INA219](https://www.aliexpress.com/item/Free-shipping-Zero-drift-CJMCU-219-INA219-I2C-interface-Bi-directional-current-power-monitoring-sensor/32688147341.html)
* ESP8266 (WeMos/NodeMCU Dev Board)

Diagrams to come...

##Software

* Download and install the Blynk Mobile App for iOS or Android.
* Scan the QR code at the bottom of this page to clone the screenshot or create a new project yourself and manually arrange and setup the widgets. 
* Email yourself the Auth code. 
* Download this repo and copy the files in to your sketches directory. Open the sketch in Arduino IDE. 
* Go to the settings.h tab. This is where all the customisable settings are. You should be able to change almost everything from there before compiling. 
* Enter the Auth code you were provided in to settings.h. 

###Settings.h

* Replace AUTH "xxxxxxxxxxx" with your own Auth code.
* If you are using the Blynk Cloud server (most beginners do) then comment out the line below

From
```cpp
#define USE_LOCAL_SERVER
```
to
```cpp
//#define USE_LOCAL_SERVER
```




![INA219 Power Monitor running on Blynk](http://community.blynk.cc/uploads/default/original/2X/d/d8380acfb7de92c7fef6b4c939a5e46bf0166eac.PNG)
