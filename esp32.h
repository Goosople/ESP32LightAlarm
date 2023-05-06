#include <WiFi.h>
#include <SPIFFS.h>

#define logger Serial

inline String tmf(struct tm _t) {
    char c_str[5];
    sprintf(c_str, "%02d%02d", _t.tm_hour, _t.tm_min);
    return c_str;
}

inline void wifiSetup() {
    logger.print("\033[3;36m");
    // Init WiFi as Station, start SmartConfig
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();

    // Wait for SmartConfig packet from mobile
    logger.println("Waiting for SmartConfig.");
    while (!WiFi.smartConfigDone()) {
        delay(500);
        logger.print(".");
    }

    logger.println("");
    logger.println("SmartConfig received.");

    // Wait for WiFi to connect to AP
    logger.println("Waiting for WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        logger.print(".");
    }

    logger.print("WiFi Connected.\n\rIP Address: ");
    logger.println(WiFi.localIP());
    logger.print("\033[0m");
}
