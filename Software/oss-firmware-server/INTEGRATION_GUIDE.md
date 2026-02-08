# OSSM Custom Firmware Update Integration Guide

This guide explains how to set up and use the custom firmware update system for OSSM.

## Overview

The custom firmware update system allows you to:
- Host firmware updates on your own server
- Skip the standard update check process
- Use Docker for easy server deployment
- Maintain compatibility with existing OSSM environments

## Files Created

### 1. Docker Server Files
- `oss-firmware-server/docker-compose.yml` - Docker configuration
- `oss-firmware-server/README.md` - Server setup instructions
- `oss-firmware-server/INTEGRATION_GUIDE.md` - This file

### 2. Modified OSSM Files
- `src/utils/update.h` - Updated to support custom firmware URLs
- `platformio.ini` - Added CUSTOM_FIRMWARE_URL flag for production-s3

## Setup Instructions

### Step 1: Set Up Your Firmware Server

1. **Create the server directory:**
```bash
mkdir oss-firmware-server
cd oss-firmware-server
```

2. **Copy the Docker files:**
```bash
# Copy docker-compose.yml and README.md to the directory
```

3. **Create firmware directory:**
```bash
mkdir firmware
```

4. **Place your firmware:**
```bash
# Copy your compiled firmware.bin to the firmware directory
cp path/to/your/firmware.bin firmware/
```

5. **Start the server:**
```bash
docker-compose up -d
```

### Step 2: Configure Your Server IP

1. **Edit platformio.ini:**
   - Open `platformio.ini` in your OSSM project
   - Find the `production-s3` environment section
   - Replace `192.168.1.100` with your actual server IP:
   ```ini
   -D CUSTOM_FIRMWARE_URL="http://YOUR_SERVER_IP/firmware.bin"
   ```

2. **Alternative: Use environment variable:**
   ```bash
   export CUSTOM_FIRMWARE_URL="http://YOUR_SERVER_IP/firmware.bin"
   ```

### Step 3: Build and Flash

1. **Build for production-s3:**
```bash
# Using PlatformIO
pio run -e production-s3

# Or using VS Code PlatformIO extension
# Select "production-s3" environment and build
```

2. **Flash to your device:**
```bash
pio run -e production-s3 -t upload
```

## Usage

### Updating Firmware

1. **Access OSSM Menu:**
   - Navigate to "Calibration → Update OSSM"

2. **Update Process:**
   - The system will skip the standard update check
   - Download firmware directly from your custom server
   - Apply the update automatically

### Testing

1. **Local Testing:**
   - Use `http://localhost/firmware.bin` for testing
   - Run Docker on the same machine as your OSSM device

2. **Network Testing:**
   - Ensure your OSSM device can reach the server IP
   - Test with: `curl http://YOUR_SERVER_IP/firmware.bin`

## Configuration Options

### Docker Configuration

**Change Port:**
```yaml
ports:
  - "8080:80"  # Change 8080 to your desired port
```

**Add SSL (Advanced):**
```yaml
ports:
  - "443:443"
volumes:
  - ./ssl:/etc/nginx/ssl:ro
```

### PlatformIO Configuration

**Different Environments:**
- `development` - For development builds
- `staging` - For staging builds
- `production` - For standard production builds
- `production-s3` - For ESP32-S3 with custom firmware URL

**Custom Build Flags:**
```ini
build_flags =
    -D CUSTOM_FIRMWARE_URL="http://your-server/firmware.bin"
    -D CUSTOM_UPDATE_CHECK=false  # Skip update check entirely
```

## Troubleshooting

### Common Issues

**1. Firmware Not Found**
- Check `firmware.bin` exists in the `firmware/` directory
- Verify file permissions
- Check URL path is correct

**2. Container Won't Start**
- Ensure Docker is running
- Check port 80 is not in use
- View logs: `docker-compose logs`

**3. Network Connectivity**
- Test connectivity from OSSM device to server
- Check firewall settings
- Verify server IP address

**4. Compilation Errors**
- Ensure PlatformIO is installed
- Check for syntax errors in modified files
- Verify all dependencies are installed

### Debug Logs

**Enable Debug Logging:**
```ini
build_flags =
    -D CORE_DEBUG_LEVEL=4  # Enable debug logs
```

**Log Messages:**
- "Custom firmware URL configured, skipping update check"
- "Using custom firmware URL: http://..."
- "HTTP_UPDATE_OK" - Update successful

## Security Considerations

### Production Deployment

1. **Use HTTPS:**
   - Configure SSL certificates
   - Use HTTPS URLs in platformio.ini

2. **Firewall Configuration:**
   - Allow only necessary ports
   - Restrict access to known IP ranges

3. **File Permissions:**
   - Keep firmware directory read-only
   - Secure Docker container access

4. **Backup Strategy:**
   - Keep backup of working firmware
   - Test updates in staging environment first

## Advanced Configuration

### Multiple Firmware Versions

**Directory Structure:**
```
firmware/
├── firmware-v1.0.0.bin
├── firmware-v1.1.0.bin
└── latest.bin
```

**URL Configuration:**
```ini
-D CUSTOM_FIRMWARE_URL="http://server/firmware/latest.bin"
```

### Custom Update Logic

**Modify update.h for advanced logic:**
```cpp
#ifdef CUSTOM_FIRMWARE_URL
    // Custom update logic here
    if (shouldUpdate()) {
        return true;
    }
#endif
```

## Support

For issues or questions:
1. Check the troubleshooting section
2. Review Docker logs: `docker-compose logs`
3. Enable debug logging in platformio.ini
4. Test with local server first

## File Structure Summary

```
oss-firmware-server/
├── docker-compose.yml    # Docker configuration
├── README.md            # Basic setup instructions
├── INTEGRATION_GUIDE.md # This comprehensive guide
└── firmware/            # Firmware files directory
    └── firmware.bin     # Your compiled OSSM firmware

OSSM-Software/
├── src/utils/update.h          # Modified for custom URLs
├── platformio.ini              # Updated with CUSTOM_FIRMWARE_URL
└── ...                         # Rest of OSSM codebase
```

This setup provides a complete, production-ready firmware update system for OSSM with your custom server.
