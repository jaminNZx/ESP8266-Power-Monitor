/**************************************************************
 *
 *                          Settings
 *
 **************************************************************/
/*
     Blynk Auth Code
*/
  #define AUTH                  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
/*
   Wifi Credentials
*/
  #define WIFI_SSID             "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
  #define WIFI_PASS             "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
/*
   Over The Air Hostname
*/
  #define OTA_HOSTNAME          "POWER-MONITOR"
/*
   Local Server Settings (uncomment to use local server)
*/
#define USE_LOCAL_SERVER
#define SERVER                  IPAddress(192, 168, 1, 2)
/*
   Energy Cost per kWh in cents. (comment out to use API method)
*/
#define FIXED_ENERGY_PRICE      9.934
#define ENERGY_API              "http://192.168.1.2:3000/"
/*
   Virtual Pins - Base
*/
#define vPIN_VOLTAGE_REAL       V1 
#define vPIN_VOLTAGE_AVG        V2 
#define vPIN_VOLTAGE_PEAK       V3 

#define vPIN_CURRENT_REAL       V4
#define vPIN_CURRENT_AVG        V5 
#define vPIN_CURRENT_PEAK       V6
#define vPIN_CURRENT_GRAPH      V7

#define vPIN_POWER_REAL         V8
#define vPIN_POWER_AVG          V9 
#define vPIN_POWER_PEAK         V10

#define vPIN_ENERGY_USED        V11
#define vPIN_ENERGY_COST        V12 
#define vPIN_ENERGY_PRICE       V13
#define vPIN_ENERGY_API         V14
#define vPIN_ENERGY_TIME        V15

#define vPIN_BUTTON_AUTORANGE   V16
#define vPIN_BUTTON_HOLD        V17
#define vPIN_BUTTON_RESET_AVG   V18
#define vPIN_BUTTON_RESET_MAX   V19


