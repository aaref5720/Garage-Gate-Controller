# Garage Gate Controller - HTTP Implementation

A complete garage door control system using CC3200 microcontroller, Python HTTP server, and Android mobile app.

## 🏗️ System Architecture

This project consists of three main components:

1. **CC3200 Microcontroller** (`cc3200_http_garage_fixed.ino`) - HTTP server for garage control
2. **Python HTTP Server** (`garage_http_server_fixed.py`) - Web interface and logging server  
3. **Android App** (`app/`) - Mobile interface for garage control

## 🚀 Features

- **HTTP-based Communication**: Reliable HTTP protocol for garage control
- **Real-time Status Monitoring**: Live garage door status updates
- **Web Interface**: Beautiful web dashboard for monitoring and control
- **Mobile App**: Android app for remote garage control
- **Hardware Integration**: Direct relay control for garage door mechanism
- **Status Logging**: Complete activity logging and monitoring

## 📋 Hardware Requirements

### CC3200 Setup
- **Microcontroller**: CC3200 LaunchPad
- **Relay Module**: For garage door control
- **LED Indicators**: Status and relay status LEDs
- **WiFi Connection**: For network communication

### Pin Configuration
```cpp
const int relayPin = 39;           // Relay control pin
const int RelaystatusLedPin = 1;   // Relay status LED
const int statusLedPin = 2;        // General status LED
```

## 🛠️ Setup Instructions

### 1. CC3200 Microcontroller Setup

1. **Install Energia IDE**
   - Download Energia IDE for CC3200 development
   - Install required libraries: WiFi, WiFiClient, WiFiServer

2. **Configure WiFi Settings**
   ```cpp
   char ssid[] = "YOUR_WIFI_SSID";
   char password[] = "YOUR_WIFI_PASSWORD";
   ```

3. **Upload the Code**
   - Open `cc3200_http_garage_fixed.ino` in Energia IDE
   - Select CC3200 LaunchPad as target board
   - Upload the code to your CC3200

4. **Get CC3200 IP Address**
   - Check Serial Monitor for the assigned IP address
   - Update the IP in Android app and Python server

### 2. Python HTTP Server Setup

1. **Install Python Dependencies**
   ```bash
   pip install http.server json threading datetime
   ```

2. **Configure Server Settings**
   ```python
   HOST = "0.0.0.0"  # Listen on all interfaces
   PORT = 8080        # Server port
   ```

3. **Run the Server**
   ```bash
   python garage_http_server_fixed.py
   ```

4. **Access Web Interface**
   - Open browser to `http://localhost:8080`
   - Use the web interface to test garage control

### 3. Android App Setup

1. **Open in Android Studio**
   ```bash
   # Open Android Studio and import the project
   ```

2. **Update IP Addresses**
   ```kotlin
   private val cc3200BaseUrl = "http://192.168.0.105"  // Your CC3200 IP
   private val pcServerUrl = "http://192.168.0.103:8080"  // Your PC IP
   ```

3. **Build and Run**
   - Connect Android device or start emulator
   - Click "Run" in Android Studio
   - Grant internet permissions when prompted

4. **APK File Location**
   - The compiled APK file is located at:
   ```
   C:\Users\aaref\AndroidStudioProjects\AutomotiveDashboard\app\build\outputs\apk\release
   ```
   - You can install this APK directly on Android devices for testing

## 🔧 Configuration

### Network Configuration

**CC3200 Settings:**
- WiFi SSID and password in the Arduino code
- Default HTTP server port: 80

**Python Server Settings:**
- Host: 0.0.0.0 (all interfaces)
- Port: 8080
- Web interface available at `http://localhost:8080`

**Android App Settings:**
- CC3200 IP: Update in `MainActivity.kt`
- PC Server IP: Update in `MainActivity.kt`

### Hardware Configuration

**Relay Connection:**
- Connect relay module to pin 39
- Connect garage door motor to relay
- Ensure proper power supply for relay

**LED Indicators:**
- Status LED on pin 2
- Relay status LED on pin 1

## 📱 Usage

### Web Interface
1. **Access Dashboard**: Open `http://localhost:8080`
2. **Monitor Status**: Real-time garage door status
3. **Send Commands**: Use "OPEN GARAGE" and "CLOSE GARAGE" buttons
4. **View Logs**: Check recent activity in the log section

### Android App
1. **Launch App**: Open the Android app
2. **Check Connection**: Verify HTTP status indicator
3. **Control Garage**: Use the garage control buttons
4. **Monitor Status**: Real-time connection and command status

### CC3200 Serial Monitor
1. **Open Serial Monitor**: In Energia IDE
2. **Monitor Logs**: View connection status and commands
3. **Debug Issues**: Check for WiFi connection and HTTP requests

## 🔌 API Endpoints

### CC3200 HTTP Server (Port 80)
- `GET /` - Get garage status
- `POST /open` - Open garage door
- `POST /close` - Close garage door

### Python Server (Port 8080)
- `GET /` - Web interface
- `POST /open` - Open garage door
- `POST /close` - Close garage door
- `POST /api/garage/status` - CC3200 status updates

## 🐛 Troubleshooting

### Common Issues

1. **CC3200 Not Connecting**
   - Check WiFi credentials
   - Verify CC3200 is powered correctly
   - Check Serial Monitor for connection logs

2. **Android App Connection Failed**
   - Verify CC3200 IP address is correct
   - Ensure both devices are on same network
   - Check firewall settings

3. **Python Server Not Starting**
   - Check if port 8080 is available
   - Verify Python dependencies are installed
   - Check firewall/antivirus settings

4. **Garage Door Not Responding**
   - Check relay connections
   - Verify power supply to relay
   - Test relay manually

### Debug Information

- **CC3200**: Check Serial Monitor for detailed logs
- **Python Server**: Console output shows all requests
- **Android App**: Check Logcat for HTTP request logs
- **Web Interface**: Browser developer tools for network requests

## 🔒 Security Notes

- **Network Security**: Use WPA2/WPA3 WiFi encryption
- **Local Network**: Keep all devices on secure local network
- **Access Control**: Consider adding authentication for production use
- **Firewall**: Configure firewall rules appropriately

## 📁 Project Structure

```
Garage-Gate-Controller/
├── cc3200_http_garage_fixed.ino    # CC3200 Arduino code
├── garage_http_server_fixed.py      # Python HTTP server
├── app/                             # Android application
│   ├── src/main/java/
│   │   └── com/example/automotivedashboard/
│   │       └── MainActivity.kt      # Main Android activity
│   └── src/main/res/
│       └── layout/
│           └── activity_main.xml    # Android UI layout
└── README.md                        # This file
```

## 🛡️ Safety Considerations

- **Electrical Safety**: Ensure proper electrical isolation
- **Mechanical Safety**: Install proper safety sensors
- **Emergency Stop**: Consider emergency stop functionality
- **Manual Override**: Ensure manual operation is always possible

## 👨‍💻 Author

**Abdelrahman Mohamed** - Embedded Software Engineer

## 📄 License

This project is for educational and testing purposes. Use at your own risk.

## 🚨 Disclaimer

This project involves electrical and mechanical components. Ensure proper safety measures are in place before deployment. The authors are not responsible for any damage or injury resulting from the use of this system. 