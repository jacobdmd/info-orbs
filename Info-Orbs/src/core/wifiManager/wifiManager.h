#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <arduinoJson.h>
#include "core/fileManager.h"
#include "screenManager.h"

class WifiManager {
   public:
    WifiManager(ScreenManager& manager);

    bool isConnected();
    void setup(JsonDocument& doc);
    void draw();

   private:
    AsyncWebServer m_server{80};
    ScreenManager& m_manager;
    FileManager *fm;
    JsonDocument localJson;

    IPAddress m_localIP;
    IPAddress m_localGateway;
    IPAddress m_subnet{255, 255, 0, 0};

    // Search for parameter in HTTP POST request
    const char* SSID_INPUT_NAME = "ssid";
    const char* PASS_INPUT_NAME = "pass";
    const char* IP_INPUT_NAME = "ip";
    const char* GATEWAY_INPUT_NAME = "gateway";

    // Variables to save values from HTML form
    String m_ssid;
    String m_pass;
    String m_ip;
    String m_gateway;

    String m_dotsString{"."};

    unsigned long m_previousMillis = 0;
    const long m_interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

    bool initWifi();
    void configureAccessPoint();
    void configureWebServer();
};