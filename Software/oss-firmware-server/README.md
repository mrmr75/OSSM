# OSSM Firmware Server

A Docker-based firmware server for OSSM (Open Source Sex Machine) over-the-air updates.

## Quick Start

1. **Create directory structure:**
```
oss-firmware-server/
├── docker-compose.yml
└── firmware/
    └── firmware.bin
```

2. **Place your compiled firmware.bin in the firmware/ directory**

3. **Start the server:**
```bash
docker-compose up -d
```

4. **Your firmware URL will be:**
```
http://YOUR_SERVER_IP/firmware.bin
```

## Directory Structure

```
oss-firmware-server/
├── docker-compose.yml    # Docker configuration
├── README.md            # This file
└── firmware/            # Firmware files directory
    └── firmware.bin     # Your compiled OSSM firmware
```

## Usage

### Starting the Server
```bash
# Start in background
docker-compose up -d

# View logs
docker-compose logs -f

# Stop the server
docker-compose down
```

### Updating Firmware

1. **Stop the container:**
```bash
docker-compose down
```

2. **Replace firmware.bin in the firmware/ directory with your new firmware**

3. **Restart:**
```bash
docker-compose up -d
```

## Configuration

### Port Configuration
The server runs on port 80 by default. To change the port, modify the `ports` section in `docker-compose.yml`:

```yaml
ports:
  - "8080:80"  # Change 8080 to your desired port
```

### Security Notes
- The firmware directory is mounted as read-only for security
- Consider using HTTPS in production (requires SSL certificate setup)
- Firewall should allow traffic on the configured port

## Testing

### Local Testing
For testing on the same machine:
- URL: `http://localhost/firmware.bin`
- Start Docker on the same machine as your OSSM device

### Network Testing
- Ensure your OSSM device can reach the server IP
- Test with: `curl http://YOUR_SERVER_IP/firmware.bin`
- File should download without errors

## Troubleshooting

### Container Won't Start
- Check Docker is running: `docker ps`
- Check logs: `docker-compose logs`
- Ensure port 80 is not in use by another service

### Firmware Not Found
- Verify `firmware.bin` exists in the `firmware/` directory
- Check file permissions
- Verify URL path is correct

### Network Issues
- Ensure firewall allows traffic on port 80
- Test connectivity from OSSM device to server
- Check server IP address is correct

## Integration with OSSM

This server is designed to work with the modified OSSM firmware that uses custom firmware URLs. See the main OSSM repository for integration instructions.
