#include "wm.h"

#include "WiFi.h"
#include "constants/UserConfig.h"

WiFiManager wm;

void initWM() {
    WiFi.useStaticBuffers(true);

#if defined(WIFI_SSID) && defined(WIFI_PASSWORD)
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#else
    WiFi.begin();
#endif

    // Initialize OTA service after WiFi is set up
    OTAService::init(UserConfig::otaHostname, UserConfig::otaPassword);
}
