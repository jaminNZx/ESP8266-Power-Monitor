#define BLYNK_PRINT Serial
#define BLYNK_MAX_READBYTES 512
/****************************************************************************/
#include "settings.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <SimpleTimer.h>
#ifdef OTA_UPDATES
#include <ArduinoOTA.h>
#endif
/****************************************************************************/
SimpleTimer timer;
Adafruit_INA219 ina219;
float shuntvoltage = 0.00;
float busvoltage = 0.00;
float current_mA = 0.00, current_mA_Max;
float loadvoltage = 0.00, loadvoltageMax;
float energy = 0.00, energyPrice = 0.000, energyCost, energyPrevious, energyDifference;
float power = 0.00, powerMax, powerAvg;
int sendTimer, pollingTimer, priceTimer, graphTimer, autoRange, countdownResetCon, countdownResetClock, counter2, secret, stopwatchTimer;
long stopwatch;
int splitTimer1, splitTimer2, splitTimer3, splitTimer4, splitTimer5;
int sendTimer1, sendTimer2, sendTimer3, sendTimer4, sendTimer5;
int loadvoltage_AVG_cycle = 0, current_AVG_cycle = 0, power_AVG_cycle = 0;
float loadvoltage_AVG[AVG_DEPTH_VOLTAGE + 1], current_AVG[AVG_DEPTH_CURRENT + 1], power_AVG[AVG_DEPTH_POWER + 1];
float loadvoltage_AVG_total, current_AVG_total, power_AVG_total;
/****************************************************************************/
void getINA219values() {

  // get the INA219 values and throw some basic math at them
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000); // V
  power = (current_mA / 1000) * loadvoltage * 1000; // mW
  energy = energy + (power / 1000 / 1000);

  // nothing connected? set all to 0, otherwise they float around 0.
  if (loadvoltage < 1.1 && loadvoltage > 1 && current_mA < 2 && power < 2) {
    loadvoltage = 0;
    current_mA = 0;
    power = 0;
    energy = 0;
  }

  // gather voltage averages
  loadvoltage_AVG[loadvoltage_AVG_cycle] = loadvoltage;
  loadvoltage_AVG_cycle++;
  if (loadvoltage_AVG_cycle == AVG_DEPTH_VOLTAGE) loadvoltage_AVG_cycle = 0;

  // gather current averages
  current_AVG[current_AVG_cycle] = current_mA;
  current_AVG_cycle++;
  if (current_AVG_cycle == AVG_DEPTH_CURRENT) current_AVG_cycle = 0;

  // gather power averages
  power_AVG[power_AVG_cycle] = power;
  power_AVG_cycle++;
  if (power_AVG_cycle == AVG_DEPTH_POWER) power_AVG_cycle = 0;
}

// this function is for updaing the REAL TIME values and is on a timer
void sendINA219valuesREAL() {
  // VOLTAGE
  Blynk.virtualWrite(vPIN_VOLTAGE_REAL, String(loadvoltage, 4) + String(" V") );
  // POWER
  if (power > 1000 && autoRange == 1) {
    Blynk.virtualWrite(vPIN_POWER_REAL, String((power / 1000), 3) + String(" W") );
  } else {
    Blynk.virtualWrite(vPIN_POWER_REAL, String(power, 3) + String(" mW") );
  }
  // CURRENT (REAL TIME)
  if (current_mA > 1000 && autoRange == 1) {
    Blynk.virtualWrite(vPIN_CURRENT_REAL, String((current_mA / 1000), 3) + String(" A") );
  } else {
    Blynk.virtualWrite(vPIN_CURRENT_REAL, String(current_mA, 3) + String(" mA"));
  }
}

// this function is for updaing the AVERGE values and is on a timer
void sendINA219valuesAVG() {
  // VOLTAGE
  for (int i = 0; i < (AVG_DEPTH_VOLTAGE - 1); i++) loadvoltage_AVG_total += loadvoltage_AVG[i];
  loadvoltage_AVG_total = loadvoltage_AVG_total / AVG_DEPTH_VOLTAGE;
  Blynk.virtualWrite(vPIN_VOLTAGE_AVG, String(loadvoltage_AVG_total, 3) + String(" V"));

  // CURRENT
  for (int i = 0; i < (AVG_DEPTH_CURRENT - 1); i++) current_AVG_total += current_AVG[i];
  current_AVG_total = current_AVG_total / AVG_DEPTH_CURRENT;
  if (current_AVG_total > 1000 && autoRange == 1) {
    Blynk.virtualWrite(vPIN_CURRENT_AVG, String((current_AVG_total / 1000), 2) + String(" A"));
  } else {
    Blynk.virtualWrite(vPIN_CURRENT_AVG, String(current_AVG_total, 2) + String(" mA"));
  }

  // POWER
  for (int i = 0; i < (AVG_DEPTH_POWER - 1); i++) power_AVG_total += power_AVG[i];
  power_AVG_total = power_AVG_total / AVG_DEPTH_POWER;
  if (power_AVG_total > 1000 && autoRange == 1) {
    Blynk.virtualWrite(vPIN_POWER_AVG, String((power_AVG_total / 1000), 2) + String(" W"));
  } else {
    Blynk.virtualWrite(vPIN_POWER_AVG, String(power_AVG_total, 2) + String(" mW"));
  }
}

// this function is for updaing the MAX values and is on a timer
void sendINA219valuesMAX() {
  // LOAD VOLTAGE (HIGH)
  if (loadvoltage > loadvoltageMax) {
    loadvoltageMax = loadvoltage;
    Blynk.virtualWrite(vPIN_VOLTAGE_PEAK, String(loadvoltageMax, 3) + String(" V") );
  }
  // LOAD CURRENT (HIGH)
  if (current_mA > current_mA_Max) {
    current_mA_Max = current_mA;
    if (current_mA_Max > 1000 && autoRange == 1) {
      Blynk.virtualWrite(vPIN_CURRENT_PEAK, String((current_mA_Max / 1000), 2) + String(" A") );
    } else {
      Blynk.virtualWrite(vPIN_CURRENT_PEAK, String(current_mA_Max, 2) + String(" mA"));
    }
  }
  // LOAD POWER (HIGH)
  if (power > powerMax) {
    powerMax = power;
    if (powerMax > 1000 && autoRange == 1) {
      Blynk.virtualWrite(vPIN_POWER_PEAK, String((powerMax / 1000), 2) + String(" W") );
    } else {
      Blynk.virtualWrite(vPIN_POWER_PEAK, String(powerMax, 2) + String(" mW"));
    }
  }
}

// this function is for updaing the ENERGY values and is on a timer
void sendINA219valuesENERGY() {
  energyDifference = energy - energyPrevious;
  // ENERGY CONSUMPTION
  if (energy > 1000 && autoRange == 1) {
    Blynk.virtualWrite(vPIN_ENERGY_USED, String((energy / 1000), 5) + String(" kWh"));
  } else {
    Blynk.virtualWrite(vPIN_ENERGY_USED, String(energy, 5) + String(" mWh"));
  }
  energyPrevious = energy;
  // ENERGY COST
  energyCost = energyCost + ((energyPrice / 1000 / 100) * energyDifference);
  if (energyCost > 9.999) {
    Blynk.virtualWrite(vPIN_ENERGY_COST, String((energyCost), 7));
  } else {
    Blynk.virtualWrite(vPIN_ENERGY_COST, String((energyCost), 8));
  }
}

// HOLD BUTTON
BLYNK_WRITE(vPIN_BUTTON_HOLD) {
  if (param.asInt()) {
    timer.disable(sendTimer1);
    timer.disable(sendTimer2);
    timer.disable(sendTimer3);
    timer.disable(sendTimer4);
  } else {
    timer.enable(sendTimer1);
    timer.enable(sendTimer2);
    timer.enable(sendTimer3);
    timer.enable(sendTimer4);
  }
}

// this function only runs when in HOLD mode and select AUTO-RANGE. just shows latest values on demand
void updateINA219eXtraValues() {
  Blynk.virtualWrite(vPIN_VOLTAGE_PEAK, String(loadvoltageMax, 3) + String(" V") );
  if (current_AVG_total > 1000 && autoRange == 1) {
    Blynk.virtualWrite(vPIN_CURRENT_PEAK, String((current_AVG_total / 1000), 2) + String(" A") );
  } else {
    Blynk.virtualWrite(vPIN_CURRENT_PEAK, String(current_AVG_total, 2) + String(" mA"));
  }
  if (powerMax > 1000 && autoRange == 1) {
    Blynk.virtualWrite(vPIN_POWER_PEAK, String((powerMax / 1000), 2) + String(" W") );
  } else {
    Blynk.virtualWrite(vPIN_POWER_PEAK, String(powerMax, 2) + String(" mW"));
  }
}

// AUTO RANGE BUTTON
BLYNK_WRITE(vPIN_BUTTON_AUTORANGE) {
  autoRange = param.asInt();
  updateINA219eXtraValues();
}

// RESET AVERAGES (short) & RESET STOPWATCH (long)
BLYNK_WRITE(vPIN_BUTTON_RESET_AVG) {
  if (param.asInt()) {
    Blynk.virtualWrite(vPIN_VOLTAGE_AVG,  "--- V");
    Blynk.virtualWrite(vPIN_CURRENT_AVG, "--- mA");
    Blynk.virtualWrite(vPIN_POWER_AVG, "--- mW");
    for (int i = 0; i < (AVG_DEPTH_VOLTAGE - 1); i++) loadvoltage_AVG[i] = loadvoltage;
    for (int i = 0; i < (AVG_DEPTH_CURRENT - 1); i++) current_AVG[i] = current_mA;
    for (int i = 0; i < (AVG_DEPTH_POWER   - 1); i++) power_AVG[i] = power;
    delay(50);
    updateINA219eXtraValues();
    countdownResetCon = timer.setTimeout(1000, []() {
      Blynk.virtualWrite(vPIN_ENERGY_USED, "0.00000 mWh");
      Blynk.virtualWrite(vPIN_ENERGY_COST, "0.000000");
      energy = 0;
      energyCost = 0;
      energyPrevious = 0;
    });
  } else {
    timer.disable(countdownResetCon);
  }

}

// RESET PEAKS (short) & RESET CONSUMTION (long)
BLYNK_WRITE(vPIN_BUTTON_RESET_MAX) {
  if (param.asInt()) {
    Blynk.virtualWrite(vPIN_VOLTAGE_PEAK, "--- V");
    Blynk.virtualWrite(vPIN_CURRENT_PEAK, "--- mA");
    Blynk.virtualWrite(vPIN_POWER_PEAK, "--- mW");
    loadvoltageMax = loadvoltage;
    current_mA_Max = current_mA;
    powerMax = power;
    delay(50);
    updateINA219eXtraValues();
    countdownResetClock = timer.setTimeout(1000, []() {
      Blynk.virtualWrite(vPIN_ENERGY_TIME, "--:--:--:--");
      stopwatch = 0;
    });
  } else {
    timer.disable(countdownResetClock);
  }
}

// the stopwatch counter which is run on a timer
void stopwatchCounter() {
  stopwatch++;
  long days = 0, hours = 0, mins = 0, secs = 0;
  String secs_o = ":", mins_o = ":", hours_o = ":";
  secs = stopwatch; //convect milliseconds to seconds
  mins = secs / 60; //convert seconds to minutes
  hours = mins / 60; //convert minutes to hours
  days = hours / 24; //convert hours to days
  secs = secs - (mins * 60); //subtract the coverted seconds to minutes in order to display 59 secs max
  mins = mins - (hours * 60); //subtract the coverted minutes to hours in order to display 59 minutes max
  hours = hours - (days * 24); //subtract the coverted hours to days in order to display 23 hours max
  if (secs < 10) secs_o = ":0";
  if (mins < 10) mins_o = ":0";
  if (hours < 10) hours_o = ":0";
  Blynk.virtualWrite(vPIN_ENERGY_TIME, days + hours_o + hours + mins_o + mins + secs_o + secs);
}

#ifdef FIXED_ENERGY_PRICE
BLYNK_WRITE(vPIN_ENERGY_API) {
  energyPrice = param.asFloat();
  Blynk.virtualWrite(vPIN_ENERGY_PRICE, String(energyPrice, 4) + String('c') );
}
#endif

/****************************************************************************/
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  // CONNECT TO BLYNK
#ifdef USE_LOCAL_SERVER
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS, SERVER);
#else
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS);
#endif
  while (Blynk.connect() == false) {}
#ifdef OTA_UPDATES
  // SETUP OVER THE AIR UPDATES
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.begin();
#endif
  // START INA219
  ina219.begin();

  // TIMERS
  pollingTimer = timer.setInterval(1000, getINA219values);
  graphTimer = timer.setInterval(2000, []() {
    Blynk.virtualWrite(vPIN_GRAPH, current_mA);
  });
  stopwatchTimer = timer.setInterval(1000, stopwatchCounter);

  // setup split-task timers so we dont overload ESP
  // with too many virtualWrites per second
  timer.setTimeout(200, []() {
    sendTimer1 = timer.setInterval(1000, sendINA219valuesREAL);
  });
  timer.setTimeout(400, []() {
    sendTimer2 = timer.setInterval(1000, sendINA219valuesAVG);
  });
  timer.setTimeout(600, []() {
    sendTimer3 = timer.setInterval(1000, sendINA219valuesMAX);
  });
  timer.setTimeout(800, []() {
    sendTimer4 = timer.setInterval(2000, sendINA219valuesENERGY);
  });

  // start in auto-range mode & sync widget to hardware state
  autoRange = 1;
  Blynk.virtualWrite(vPIN_BUTTON_AUTORANGE, 1);

  // Check for fixed energy price and update global 'energyPrice'
#ifdef FIXED_ENERGY_PRICE
  // else set fixed price with configured price
  energyPrice = FIXED_ENERGY_PRICE;
  Blynk.virtualWrite(vPIN_ENERGY_PRICE, String(FIXED_ENERGY_PRICE, 4) + String('c') );
#else
  // No fixed price set, so pull from local API
  Blynk.virtualWrite(vPIN_ENERGY_API, ENERGY_API);
  priceTimer = timer.setInterval(20000, []() {
    Blynk.virtualWrite(vPIN_ENERGY_API, ENERGY_API);
  });
#endif
}
/****************************************************************************/
void loop() {
  // the loop... dont touch or add to this!
  Blynk.run();
#ifdef OTA_UPDATES
  ArduinoOTA.handle();
#endif
  timer.run();
}
