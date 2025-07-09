/**
 * CC3200 Garage Door Controller - HTTP Version
 * 
 * This version uses HTTP instead of MQTT for better Energia compatibility
 * 
 * @author Abdelrahman Mohamed
 */

 #include <WiFi.h>
 #include <WiFiClient.h>
 #include <WiFiServer.h>
 
 // WiFi Configuration
 char ssid[] = "TP-LINK_ED14";
 char password[] = "68550837";
 
 // HTTP Server Configuration
 WiFiServer server(80);
 
 // Hardware Configuration
 const int relayPin = 39;
 const int RelaystatusLedPin = 1;
 const int statusLedPin = 2;
 
 // Global Variables
 bool garageDoorOpen = false;
 bool wifiConnected = false;
 unsigned long lastStatusUpdate = 0;
 const unsigned long statusUpdateInterval = 10000; // 10 seconds
 
 /**
  * Connects to WiFi
  */
 void connectToWiFi() {
   Serial.println("Connecting to WiFi...");
   WiFi.begin(ssid, password);
   
   int attempts = 0;
   while (WiFi.status() != WL_CONNECTED && attempts < 20) {
     delay(500);
     Serial.print(".");
     attempts++;
   }
   
   if (WiFi.status() == WL_CONNECTED) {
     Serial.println("\nWiFi connected!");
     Serial.print("IP address: ");
     Serial.println(WiFi.localIP());
     Serial.print("Signal strength: ");
     Serial.println(WiFi.RSSI());
     wifiConnected = true;
   } else {
     Serial.println("\nWiFi connection failed!");
     wifiConnected = false;
   }
 }
 
 /**
  * Sends HTTP status update to your server
  */
 void sendStatusUpdate() {
   if (!wifiConnected) return;
   
   WiFiClient client;
   String serverIP = "192.168.0.103";  // Your computer's IP
   int serverPort = 8080;  // HTTP server port
   
   Serial.println("Sending status update to server...");
   
   if (client.connect(serverIP.c_str(), serverPort)) {
     Serial.println("Connected to server");
     
     // Create JSON payload
     String jsonPayload = "{";
     jsonPayload += "\"device\":\"garage_door\",";
     jsonPayload += "\"status\":\"";
     jsonPayload += (garageDoorOpen ? "open" : "closed");
     jsonPayload += "\",";
     jsonPayload += "\"timestamp\":";
     jsonPayload += millis();
     jsonPayload += "}";
     
     // Send HTTP POST request
     client.println("POST /api/garage/status HTTP/1.1");
     client.println("Host: " + serverIP + ":" + String(serverPort));
     client.println("Content-Type: application/json");
     client.println("Content-Length: " + String(jsonPayload.length()));
     client.println("Connection: close");
     client.println();
     client.println(jsonPayload);
     
     // Wait for response
     unsigned long timeout = millis();
     while (client.available() == 0) {
       if (millis() - timeout > 5000) {
         Serial.println("HTTP request timeout");
         client.stop();
         return;
       }
     }
     
     // Read response
     String response = "";
     while (client.available()) {
       response += client.readString();
     }
     
     Serial.println("Status update sent successfully");
     Serial.println("Response: " + response);
     client.stop();
   } else {
     Serial.println("Failed to connect to server for status update");
   }
 }
 
 /**
  * Handles HTTP requests from clients
  */
 void handleHttpRequests() {
   WiFiClient client = server.available();
   if (!client) return;
   
   Serial.println("New client connected");
   
   // Read the HTTP request
   String request = "";
   while (client.connected() && client.available()) {
     String line = client.readStringUntil('\n');
     if (line == "\r") break;
     if (request.length() == 0) {
       request = line;
     }
   }
   
   Serial.println("HTTP Request: " + request);
   
   String response = "";
   int statusCode = 200;
   
   // Parse the request
   if (request.indexOf("GET /") >= 0) {
     // Status request
     response = "{";
     response += "\"device\":\"garage_door\",";
     response += "\"status\":\"";
     response += (garageDoorOpen ? "open" : "closed");
     response += "\",";
     response += "\"timestamp\":";
     response += millis();
     response += "}";
   } else if (request.indexOf("POST /open") >= 0) {
     // Open garage door
     digitalWrite(relayPin, HIGH);
     digitalWrite(RelaystatusLedPin, HIGH);
     garageDoorOpen = true;
     Serial.println("Garage door OPENED via HTTP");
     
     response = "{";
     response += "\"result\":\"success\",";
     response += "\"action\":\"open\",";
     response += "\"status\":\"open\"";
     response += "}";
     
     // Send status update
     sendStatusUpdate();
   } else if (request.indexOf("POST /close") >= 0) {
     // Close garage door
     digitalWrite(relayPin, LOW);
     digitalWrite(RelaystatusLedPin, LOW);
     garageDoorOpen = false;
     Serial.println("Garage door CLOSED via HTTP");
     
     response = "{";
     response += "\"result\":\"success\",";
     response += "\"action\":\"close\",";
     response += "\"status\":\"closed\"";
     response += "}";
     
     // Send status update
     sendStatusUpdate();
   } else {
     // Invalid request
     statusCode = 404;
     response = "{\"error\":\"Invalid endpoint\"}";
   }
   
   // Send HTTP response
   client.println("HTTP/1.1 " + String(statusCode) + " " + (statusCode == 200 ? "OK" : "Not Found"));
   client.println("Content-Type: application/json");
   client.println("Access-Control-Allow-Origin: *");
   client.println("Access-Control-Allow-Methods: GET, POST");
   client.println("Access-Control-Allow-Headers: Content-Type");
   client.println("Connection: close");
   client.println();
   client.println(response);
   
   client.stop();
   Serial.println("HTTP response sent");
 }
 
 /**
  * Setup function
  */
 void setup() {
   Serial.begin(115200);
   delay(1000);
   Serial.println("\n=== CC3200 HTTP Garage Controller ===");
   
   // Initialize pins
   pinMode(relayPin, OUTPUT);
   pinMode(statusLedPin, OUTPUT);
   digitalWrite(relayPin, LOW);
   digitalWrite(statusLedPin, LOW);
   
   // Connect to WiFi
   connectToWiFi();
   
   if (wifiConnected) {
     // Start HTTP server
     server.begin();
     Serial.println("HTTP server started");
     Serial.println("Available endpoints:");
     Serial.println("  GET  /        - Get status");
     Serial.println("  POST /open    - Open garage door");
     Serial.println("  POST /close   - Close garage door");
     Serial.println();
     Serial.println("Example usage:");
     Serial.print("  curl http://");
     Serial.print(WiFi.localIP());
     Serial.println("/");
     Serial.print("  curl -X POST http://");
     Serial.print(WiFi.localIP());
     Serial.println("/open");
     Serial.print("  curl -X POST http://");
     Serial.print(WiFi.localIP());
     Serial.println("/close");
     Serial.println();
     Serial.println("Web interface: http://localhost:8080");
   }
   
   Serial.println("Setup complete!");
 }
 
 /**
  * Main loop
  */
 void loop() {
   // Check WiFi connection
   if (WiFi.status() != WL_CONNECTED) {
     Serial.println("WiFi lost. Reconnecting...");
     wifiConnected = false;
     connectToWiFi();
   }
   
   // Handle HTTP requests
   if (wifiConnected) {
     handleHttpRequests();
   }
   
   // Update status LED
   digitalWrite(statusLedPin, wifiConnected ? HIGH : LOW);
   
   // Send periodic status updates
   if (wifiConnected && (millis() - lastStatusUpdate > statusUpdateInterval)) {
     sendStatusUpdate();
     lastStatusUpdate = millis();
   }
   
   delay(10); // Small delay to prevent overwhelming the system
 } 