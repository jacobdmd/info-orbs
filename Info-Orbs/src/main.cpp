#include <Arduino.h>
#include <Button.h>
#include <config.h>
#include <globalTime.h>
#include <widgets/stockWidget.h>

#include "core/wifiManager/wifiManager.h"
#include "core/wifiWidget.h"
#include "core/fileManager.h"
#include "widgetSet.h"
#include "screenManager.h"
#include "widgetSet.h"
#include "widgets/clockWidget.h"
#include "widgets/weatherWidget.h"
#include "widgets/webDataWidget.h"
#include <Arduino.h>
#include <Button.h>
#include <globalTime.h>
#include <config.h>
#include <widgets/stockWidget.h>

TFT_eSPI tft = TFT_eSPI();

// Button states
bool lastButtonOKState = HIGH;
bool lastButtonLeftState = HIGH;
bool lastButtonRightState = HIGH;

Button buttonOK(BUTTON_OK);
Button buttonLeft(BUTTON_LEFT);
Button buttonRight(BUTTON_RIGHT);

GlobalTime *globalTime;  // Initialize the global time

String connectingString{""};

WifiWidget *wifiWidget{nullptr};
WifiManager *wifiManager;

int connectionTimer{0};
const int connectionTimeout{10000};
bool isConnected{true};

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    if (y >= tft.height())
        return 0;
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

ScreenManager* sm;
WidgetSet* widgetSet;
FileManager* fm;
JsonDocument localJson;

bool createDefaultSettings()
{
  //localJson["ssid"] = WIFI_SSID;
  //localJson["pass"] = WIFI_PASS;
  localJson["TimeZoneLocation"] = TIMEZONE_API_LOCATION;
  localJson["Format24h"] = FORMAT_24_HOUR;
  localJson["ShowAMPMIndicator"] = SHOW_AM_PM_INDICATOR;
  localJson["ShowSecondTicks"] = SHOW_SECOND_TICKS;
  localJson["WeatherLocation"] = WEATHER_LOCAION;
  localJson["StockTickerList"] = STOCK_TICKER_LIST;
  localJson["InvertOrbs"] = INVERTED_ORBS;
  return true;
}

bool readConfig()
{
  String file_content = fm->readFile("/config.json");
  
  if(file_content == "") // If the file is empty/doesn't exists, should create the file and load in defaults.
  {
    Serial.println("Config file doesn't exist or is empty. Creating...");
    createDefaultSettings();
    serializeJson(localJson, file_content);
    Serial.println(file_content);
    fm->writeFile("/config.json", file_content);
    return true;
  }

  int config_file_size = file_content.length();

  Serial.println("Config file size: " + String(config_file_size));
  Serial.println(file_content);
  if(config_file_size > 2048) {
    Serial.println("Config file too large");
    return false;
  }
  
  DeserializationError error = deserializeJson(localJson, file_content);
  if (error) { 
    Serial.println("Error interpreting config file");
    return false;
  }
  
  return true;
} 


void setup() {
    buttonLeft.begin();
    buttonOK.begin();
    buttonRight.begin();

    Serial.begin(115200);
    Serial.println();
    Serial.println("Starting up...");

    sm = new ScreenManager(tft);
    sm->selectAllScreens();
    sm->getDisplay().fillScreen(TFT_WHITE);
    sm->reset();
    widgetSet = new WidgetSet(sm);

    TJpgDec.setSwapBytes(true);  // jpeg rendering setup
    TJpgDec.setCallback(tft_output);

  fm = new FileManager();
  if(fm->mountFS())
  {
    Serial.println("Reading config.");
    if(!readConfig())
      Serial.println("Can not read config file.");
  }

#ifdef GC9A01_DRIVER
    Serial.println("GC9A01 Driver");
#endif
#ifdef ILI9341_DRIVER
    Serial.println("ILI9341 Driver");
#endif
#if HARDWARE == WOKWI
    Serial.println("Wokwi Build");
#endif

    pinMode(BUSY_PIN, OUTPUT);
    // Serial.println("Connecting to: " + String(WIFI_SSID));

    wifiWidget = new WifiWidget(*sm);
    //wifiWidget->config(localJson);
    // wifiWidget->setup();

    wifiManager = new WifiManager(*sm);
    wifiManager->setup(localJson);

    globalTime = GlobalTime::getInstance();

  widgetSet->add(new ClockWidget(*sm));
  widgetSet->add(new StockWidget(*sm));
  widgetSet->add(new WeatherWidget(*sm));
#ifdef WEB_DATA_WIDGET_URL
  widgetSet->add(new WebDataWidget(*sm, WEB_DATA_WIDGET_URL));
#endif
#ifdef WEB_DATA_STOCK_WIDGET_URL
  widgetSet->add(new WebDataWidget(*sm, WEB_DATA_STOCK_WIDGET_URL));
#endif
}

void loop() {
    if (!wifiManager->isConnected()) {
        wifiManager->draw();
        delay(100);
    } else {
        if (!widgetSet->initialUpdateDone()) {
            widgetSet->initializeAllWidgetsData();
        }
        globalTime->updateTime();

        if (buttonLeft.pressed()) {
            Serial.println("Left button pressed");
            widgetSet->prev();
        }
        if (buttonOK.pressed()) {
            Serial.println("OK button pressed");
            widgetSet->changeMode();
        }
        if (buttonRight.pressed()) {
            Serial.println("Right button pressed");
            widgetSet->next();
        }

        widgetSet->updateCurrent();
        widgetSet->drawCurrent();
    }
}