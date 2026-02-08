#ifndef OSSM_OTA_H
#define OSSM_OTA_H

#include <Arduino.h>
#include <WiFi.h>
#include <Update.h>

#include "constants/LogTags.h"

// Forward declaration to avoid circular dependency
class OSSM;

class OTAService {
  private:
    static bool initialized;
    static String hostname;
    static String password;

    static void onStart();
    static void onEnd();
    static void onError(int error);
    static void onProgress(unsigned int progress, unsigned int total);

  public:
    static void init(const String& deviceHostname = "OSSM",
                    const String& otaPassword = "ossm123");
    static void handle();
    static bool isRunning();
    static String getHostname();
    static String getIPAddress();
    static void setProgressCallback(void (*callback)(unsigned int, unsigned int));
};

#endif // OSSM_OTA_H
