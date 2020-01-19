#include "debAnalogInput.h"

// Jan 19, 2020 Notes:
// Raw ADC readings when suspended in open air: 634 to 644 (debounced to 637 to 638 w/ this program).
// Raw ADC readings when submerged in tap water: settles down to between 300 and 304.
// From suspended in air to submerged in water: readings stabilize after a minute or so.
// From submerged in water to suspended in air: readings stabilize after a few seconds.
#define VERSION_DEBOUNCED

#ifdef VERSION_DEBOUNCED
const uint8_t SOILMOISTURE_DEB_VAL_SIGNIFCHANGE = 1;
const uint8_t SOILMOISTURE_DEB_PERIOD_SIGNIFCHANGE = 50;
const int16_t SOILMOISTURE_DEB_VAL_MAX = 1023;
const int16_t SOILMOISTURE_DEB_VAL_MIN = 0;
const uint16_t SOILMOISTURE_DEB_QTY_SAMPLES = 10;
#endif

#define DBG_ALL

#define PLOT_ALL
#ifdef PLOT_ALL
#undef DBG_ALL
#define PLOT_PORT Serial
#define PLOT_BAUD 9600

#define PLOT_SOILMOISTURE_RAW
// #define PLOT_SOILMOISTURE_RAW_ISLAST
// #define PLOT_SOILMOISTURE_DEB
#define PLOT_SOILMOISTURE_DEB_ISLAST
#endif

#ifdef DBG_ALL
#define DBG_PORT Serial
#define DBG_BAUD 9600

#define DBG_SOILMOISTURE_RAW
#define DBG_SOILMOISTURE_DEB
#endif


void setup() {
  // put your setup code here, to run once:
  #ifdef DBG_ALL
  DBG_PORT.begin(DBG_BAUD);
  DBG_PORT.println(F("Capacitive Soil Moisture Sensor Test!"));
  #elif defined PLOT_ALL
  PLOT_PORT.begin(PLOT_BAUD);
  #endif
}

void loop() {
  // put your main code here, to run repeatedly:

  #ifdef VERSION_DEBOUNCED
  static int16_t soil_moisture_deb_valPrev{};
  static uint8_t soil_moisture_deb_st{};
  static float soil_moisture_deb_sum{};
  static int16_t soilMoistureDeb{};
  static uint8_t soil_moisture_deb_ctr{};
  static uint32_t soil_moisture_deb_tRef{};
  int16_t soilMoistureRaw = analogRead(A0);
  if (debAnalogInput(
          &soilMoistureRaw,
          &soil_moisture_deb_valPrev,
          &soil_moisture_deb_st,
          &soil_moisture_deb_sum,
          &soilMoistureDeb,
          &soil_moisture_deb_ctr,
          &soil_moisture_deb_tRef,
          SOILMOISTURE_DEB_VAL_SIGNIFCHANGE,
          SOILMOISTURE_DEB_PERIOD_SIGNIFCHANGE,
          SOILMOISTURE_DEB_VAL_MAX,
          SOILMOISTURE_DEB_VAL_MIN,
          SOILMOISTURE_DEB_QTY_SAMPLES))
  {
    #ifdef DBG_SOILMOISTURE_RAW
    DBG_PORT.print(F("soilMoistureRaw: "));
    DBG_PORT.println(soilMoistureRaw);
    #endif
    #ifdef DBG_SOILMOISTURE_DEB
    DBG_PORT.print(F("soilMoistureDeb: "));
    DBG_PORT.println(soilMoistureDeb);
    #endif
  }
  #else
  static int16_t soilMoistureDeb{};
  int16_t soilMoistureRaw = analogRead(A0);
  if(soilMoistureDeb != soilMoistureRaw){
    soilMoistureDeb = soilMoistureRaw;
    #ifdef DBG_SOILMOISTURE_RAW
    DBG_PORT.print(F("soilMoistureRaw: "));
    DBG_PORT.println(soilMoistureRaw);
    #endif
    #ifdef DBG_SOILMOISTURE_DEB
    DBG_PORT.print(F("soilMoistureDeb: "));
    DBG_PORT.println(soilMoistureDeb);
    #endif
  }
  #endif

  #ifdef PLOT_SOILMOISTURE_RAW
  PLOT_PORT.print(soilMoistureRaw);
  PLOT_PORT.print(" ");
  #elif defined PLOT_SOILMOISTURE_RAW_ISLAST
  PLOT_PORT.println(soilMoistureRaw);
  #endif

  #ifdef PLOT_SOILMOISTURE_DEB
  PLOT_PORT.print(soilMoistureDeb);
  PLOT_PORT.print(" ");
  #elif defined PLOT_SOILMOISTURE_DEB_ISLAST
  PLOT_PORT.println(soilMoistureDeb);
  #endif
}
