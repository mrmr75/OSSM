# OTA Update Implementation for OSSM

This document describes the implementation of Over-The-Air (OTA) firmware updates for the OSSM project.

## Overview

The OTA update system allows users to update the firmware wirelessly through WiFi without needing physical access to the device. The implementation uses ArduinoOTA library and integrates with the existing state machine architecture.

## Features

- **Wireless Firmware Updates**: Update firmware over WiFi without physical access
- **Progress Tracking**: Real-time progress display during update process
- **Error Handling**: Comprehensive error handling and user feedback
- **Security**: Configurable password protection for OTA updates
- **Integration**: Seamless integration with existing menu system and state machine

## Architecture

### Components

1. **OTA Service** (`src/services/ota.h/cpp`)
   - Manages ArduinoOTA library initialization and handling
   - Provides progress callbacks and status information
   - Handles OTA events (start, end, error, progress)

2. **State Machine Integration** (`src/ossm/Events.h`)
   - New states for OTA update process
   - Events to trigger state transitions
   - Integration with existing state machine architecture

3. **UI Integration** (`src/ossm/OSSM.cpp`)
   - Display functions for OTA update screens
   - Progress indication and status messages
   - Error handling and user feedback

4. **Configuration** (`src/constants/UserConfig.h`)
   - Configurable OTA password and hostname
   - Easy customization for different deployments

## Implementation Details

### 1. OTA Service

The OTA service is implemented as a singleton class that wraps the ArduinoOTA functionality:

```cpp
class OTAService {
public:
    static void init(const String& hostname, const String& password);
    static void handle();
    static bool isRunning();
    static String getHostname();
    static String getIPAddress();
    static void setProgressCallback(void (*callback)(unsigned int, unsigned int));

private:
    static bool initialized;
    static String hostname;
    static String password;

    static void onStart();
    static void onEnd();
    static void onError(ota_error_t error);
    static void onProgress(unsigned int progress, unsigned int total);
};
```

### 2. State Machine Integration

New states and events have been added to the state machine:

```cpp
// States
struct OTAUpdate {};
struct OTAProgress {};
struct OTAComplete {};
struct OTAFailed {};

// Events
struct StartOTA {};
struct OTAProgressEvent {};
struct OTACompleteEvent {};
struct OTAFailedEvent {};
```

### 3. Menu Integration

The OTA update option is integrated into the existing update menu:

```cpp
enum UpdateMenu {
    HTTP_Update,
    BackToCalibration,
    NUM_UPDATE_OPTIONS
};
```

### 4. Display Functions

The following display functions have been implemented:

- `drawOTA()`: Shows OTA setup information (hostname, IP, password)
- `drawOTAProgress()`: Shows update progress with progress bar
- `drawOTAComplete()`: Shows success message
- `drawOTAFailed()`: Shows error message with retry option

## Configuration

### User Configuration

OTA settings are configured in `src/constants/UserConfig.h`:

```cpp
namespace UserConfig {
    static const char* otaPassword = "ossm123";
    static const char* otaHostname = "OSSM";
}
```

### PlatformIO Configuration

The ArduinoOTA library is included in `platformio.ini`:

```ini
lib_deps =
    arduino-libraries/ArduinoOTA
```

## Usage

### 1. Accessing OTA Update

1. Navigate to the Update Menu from the main menu
2. Select "HTTP Update" option
3. Choose "OTA Update" from the submenu

### 2. Performing OTA Update

1. The device will display connection information:
   - Hostname: Configurable (default: "OSSM")
   - IP Address: Current WiFi IP
   - Password: Configurable (default: "ossm123")

2. Use Arduino IDE or other OTA tools to upload new firmware:
   - Select "Arduino OTA" as upload method
   - Enter the hostname or IP address
   - Enter the password
   - Upload the new firmware

3. Monitor progress on the device display
4. Device will restart automatically after successful update

## Security Considerations

- **Password Protection**: All OTA updates require authentication
- **WiFi Dependency**: Updates only work when WiFi is connected
- **Configuration**: Password and hostname can be customized per deployment
- **Access Control**: OTA menu only accessible through physical device interface

## Error Handling

The system handles various error conditions:

- **WiFi Disconnection**: OTA service only runs when WiFi is connected
- **Authentication Failure**: Invalid password prevents update
- **Corruption**: Failed updates trigger error state with retry option
- **Timeout**: Long-running operations have timeout protection

## Testing

To test the OTA implementation:

1. **Build and Flash**: Compile and flash the firmware to the device
2. **Connect WiFi**: Ensure device is connected to WiFi network
3. **Access Menu**: Navigate to Update Menu → HTTP Update → OTA Update
4. **Verify Display**: Check that connection information is displayed correctly
5. **Test Update**: Use Arduino IDE to perform OTA update
6. **Monitor Progress**: Verify progress display updates correctly
7. **Verify Completion**: Check that device restarts and functions normally

## Troubleshooting

### Common Issues

1. **WiFi Not Connected**
   - Ensure device is connected to WiFi
   - Check WiFi credentials in configuration

2. **Authentication Failed**
   - Verify password matches UserConfig::otaPassword
   - Check hostname configuration

3. **Update Failed**
   - Check WiFi signal strength
   - Verify firmware compatibility
   - Try manual flash if OTA continues to fail

4. **Progress Not Updating**
   - Check OTA service is running
   - Verify ArduinoOTA library is properly initialized

### Debug Information

Enable debug logging by setting appropriate log levels in `platformio.ini`:

```ini
build_flags =
    -DCORE_DEBUG_LEVEL=4
```

## Future Enhancements

Potential improvements for future versions:

1. **Progress Bar**: Implement actual progress bar instead of placeholder
2. **Multiple Networks**: Support for multiple WiFi networks
3. **Firmware Validation**: Verify firmware integrity before applying
4. **Rollback**: Automatic rollback on failed updates
5. **Remote Management**: Web interface for OTA management
6. **OTA Server**: Dedicated OTA server for enterprise deployments

## Dependencies

- **ESP32 Update Library**: Core OTA functionality (Update.h)
- **WiFi**: Network connectivity
- **State Machine**: Integration with existing architecture
- **Display System**: User interface for progress and status

## Files Modified

- `platformio.ini`: Added ArduinoOTA dependency
- `src/services/ota.h`: OTA service header
- `src/services/ota.cpp`: OTA service implementation
- `src/services/wm.h`: Added OTA service include
- `src/services/wm.cpp`: Initialize OTA service
- `src/ossm/Events.h`: Added OTA events and states
- `src/ossm/OSSM.h`: Added OTA function declarations
- `src/ossm/OSSM.cpp`: Implemented OTA display functions
- `src/main.cpp`: Added OTA service task
- `src/constants/UserConfig.h`: Added OTA configuration
- `src/constants/UpdateMenu.h`: Added OTA menu option

## Conclusion

The OTA implementation provides a robust and user-friendly way to update OSSM firmware wirelessly. The integration with the existing state machine and menu system ensures consistency with the overall application architecture while providing essential functionality for maintenance and updates.
