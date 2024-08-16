const char* save_screen_html = R"save_screen( 
  <!DOCTYPE html>
  <html>
  <head>
      <title>Save Success</title>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <style>
          body {
              text-align: center;
              background-color: #f2f2f2;
              font-family: Arial, sans-serif;
              margin: 0;
              padding: 0;
              height: 100vh;
              display: flex;
              justify-content: center;
              align-items: center;
              font-size: 12px; /* Smaller base font size */
          }

          .message {
              padding: 20px;
              border-radius: 10px;
              background-color: #4CAF50;
              color: white;
              box-shadow: 0px 0px 10px 0px rgba(0, 0, 0, 0.2);
              width: 80%;
              max-width: 300px;
          }

          .message h1 {
              margin-bottom: 10px;
              font-size: 18px; /* Smaller heading font size */
          }

          .message p {
              margin-top: 0;
              font-size: 14px; /* Smaller paragraph font size */
          }

          .home-link {
              color: white;
              text-decoration: none;
              font-weight: bold;
              border: 2px solid white;
              padding: 10px 20px;
              border-radius: 5px;
              transition: background-color 0.3s ease;
              font-size: 14px; /* Smaller link font size */
          }

          .home-link:hover {
              background-color: rgba(255, 255, 255, 0.2);
          }

          input[type="submit"] {
              background-color: green;
              color: white;
              border: none;
              padding: 10px 20px;
              border-radius: 20px;
              cursor: pointer;
              font-size: 14px; /* Smaller button font size */
              transition: background-color 0.3s ease;
          }

          input[type="submit"]:hover {
              background-color: darkgreen;
          }
      </style>
  </head>
  <body>
      <div class="message">
          <h1>Save Successful!</h1>
          <p>Your configuration has been saved.</p>
          <form action="/" method="POST">
              <input type="submit" value="Go back to home">
          </form>
      </div>
  </body>
  </html>
  )save_screen";

  const char* htmlPage = R"page(
  <!DOCTYPE html>
  <html>
  <head>
      <title>Soil Monitoring Sensor Configuration</title>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <style>
          body {
              text-align: center;
              background-color: #f2f2f2;
              font-family: Arial, sans-serif;
              margin: 0;
              padding: 0;
              font-size: 12px;
          }
          .form:before {
              content: '';
              position: absolute;
              top: -20px;
              left: 0px;
              width: 100%;
              height: 40px;
              background-color: #4CAF50;
              border-top-left-radius: 10px;
              border-top-right-radius: 10px;
          }

          .container {
              display: flex;
              flex-direction: column;
              align-items: center;
              justify-content: center;
              height: 100vh;
              padding: 10px;
          }

          .form {
              width: 80%;
              max-width: 300px;
              background-color: #f2f2f2;
              padding: 20px;
              border-radius: 10px;
              box-shadow: 0px 0px 10px 0px rgba(0, 0, 0, 0.2);
              position: relative;
              color: #555;
          }

          .form h1 {
              margin-bottom: 20px;
              color: #4CAF50;
              font-size: 18px;
          }

          .form label {
              display: block;
              margin-bottom: 5px;
              color: #555;
              font-size: 14px;
              font-weight: bold;
          }

          .form input[type="text"],
          .form input[type="password"] {
              width: 90%;
              padding: 8px;
              margin-bottom: 10px;
              border-radius: 5px;
              border: none;
              background-color: #fff;
              font-size: 12px;
              color: #555;
          }

          .form input[type="submit"] {
              padding: 8px;
              margin-top: 10px;
              width: 90%;
              border-radius: 5px;
              border: none;
              background-color: #4CAF50;
              color: white;
              font-weight: bold;
              font-size: 14px;
              cursor: pointer;
              transition: background-color 0.3s ease;
          }

          .form input[type="submit"]:hover {
              background-color: #45a049;
          }

          .sensor-values {
              margin-top: 20px;
              text-align: left;
          }

          .sensor-values h2 {
              margin-bottom: 10px;
              color: #4CAF50;
              font-size: 16px;
          }

          .sensor-values p {
              margin-bottom: 5px;
              font-size: 12px;
          }

          .sensor-values span {
              font-weight: bold;
              color: #555;
          }

          .company-info {
              font-size: 10px;
              margin-top: 20px;
              color: #777;
          }

          .company-info a {
              color: #777;
              text-decoration: none;
          }

          .company-info a:hover {
              text-decoration: underline;
          }
      </style>
  </head>
  <body>
  <div class="container">
      <div class="form">
          <h1>Moisture Sensor Configuration</h1>
          <!-- Form for setting WiFi, and thresholds -->
          <form action="/save" method="POST">
              <label for="wifi_ssid">WiFi SSID:</label>
              <input type="text" id="wifi_ssid" name="wifi_ssid" placeholder="Enter WiFi SSID">
              <label for="wifi_pass">WiFi Password:</label>
              <input type="password" id="wifi_pass" name="wifi_pass" placeholder="Enter WiFi Password">
              <label for="moisture_min">Minimum Moisture Threshold:</label>
              <input type="text" id="moisture_min" name="moisture_min" placeholder="Input Number from 10-100" pattern="[0-9]*" title="Please enter only numbers">
              <label for="moisture_max">Maximum Moisture Threshold:</label>
              <input type="text" id="moisture_max" placeholder="Input Number Greater Than Threshold Min" name="moisture_max" pattern="[0-9]*" title="Please enter only numbers">
              <label for="device_location">Device Location:</label>
              <input type="text" id="device_location" name="device_location" placeholder="Enter Device Location">
              <input type="submit" value="Save">
          </form>
          <!-- Display current sensor values and thresholds -->
          <div class="sensor-values">
              <h2>Current Sensor Values</h2>
              <p>Soil Moisture: <span id="current_moisture"></span></p>
              <p>Soil Temperature: <span id="soil_temperature"></span></p>
              <p>Soil Nitrogen (N): <span id="soil_nitrogen"></span></p>
              <p>Soil Phosphorus (P): <span id="soil_phosphorus"></span></p>
              <p>Soil Potassium (K): <span id="soil_potassium"></span></p>
              <p>Soil pH: <span id="soil_ph"></span></p>
              <p>E-conductivity: <span id="e_conductivity"></span></p>
              <h2>Threshold Settings</h2>
              <p>Minimum Threshold: <span id="min_threshold">_min_threshold</span></p>
              <p>Maximum Threshold: <span id="max_threshold">_max_threshold</span></p>
          </div>
          <!-- Company information -->
          <div class="company-info">
              <p>&copy; 2024 Ramlaxman Innovations. All rights reserved.</p>
          </div>
      </div>
  </div>
  <script>
    // Function to update sensor values
    function updateSensorValues(data) {
        document.getElementById("current_moisture").textContent = data.Moisture + " %RH";
        document.getElementById("soil_temperature").textContent = data.Temperature + " °C";
        document.getElementById("soil_nitrogen").textContent = data.N + " mg/kg";
        document.getElementById("soil_phosphorus").textContent = data.P + " mg/kg";
        document.getElementById("soil_potassium").textContent = data.K + " mg/kg";
        document.getElementById("soil_ph").textContent = data.pH;
        document.getElementById("e_conductivity").textContent = data.Conductivity + " µS/cm";
    }

    // Function to parse the received JSON data
    function parseSensorData(jsonData) {
        var data = JSON.parse(jsonData);
        updateSensorValues(data);
    }

    var receivedData = '_received_data';
    parseSensorData(receivedData);
  </script>
  </body>
  </html>
)page"; 

// Hotspot Local Server
const char* ssid = "RL_Soil_Monitor"; // SSID of the Hotspot
const char* password = "ramlaxman"; // Password of the network

// Function prototypes
// void blink_led(bool led_active);
void handleRoot();
void handleSave();

// ----------------------------- Start the hotspot and start the web-server
void hotspot() {
  esp_task_wdt_reset();
    buttonLongPressFlag = true;
    
    mqtt.disconnect();
    if(!mqtt.connected()) Serial.println("MQTT client disconnected.");
    delay(1000);
    mqtt_client.stop();
    WiFi.disconnect();
    if(WiFi.status() != WL_CONNECTED) Serial.println("WiFi Disconnected.");
    delay(1000);
    
    WiFi.mode(WIFI_AP);
    Serial.println("\n[Info] Starting Hotspot...");
    if (WiFi.softAP(ssid, password)) {
        esp_task_wdt_reset();
        IPAddress ipAddress = WiFi.softAPIP();
        
        String ip_address = ipAddress.toString();
        server.on("/", handleRoot); // Send the actual html page to display
        server.on("/save", handleSave); // Save the data into preferences once save is pressed.
        server.begin();

        Serial.println("\n\n---------------- Hotspot Access Point Activated ------------------");
        Serial.println("[Info] IP Address: " + ip_address);
        hotspot_activated = true;
        blink_led(true, GREEN_LED);                                       // turn on green led to show hotspot is ON
        esp_task_wdt_reset();
        // displayHotspotInfo();
        drawArrayJpeg(Hotspot_ON_Info_Image, sizeof(Hotspot_ON_Info_Image), 0, 0);
        delay(30000);
        esp_task_wdt_reset();
        blink_led(false, NO_LED);                                         // turn off the lED
    } else {
      esp_task_wdt_reset();
        Serial.println("[Error] Failed to start Hotspot!");
        hotspot_activated = false;
    }
    esp_task_wdt_reset();
} // End this function

void handle_webserver() {
    static long hotspot_activation_timer = millis();
    static long start_time;

    if (hotspot_activated != true) {
        // blink_led(false);
        return; // close if hotspot not activated. 
    }else{
      // blink_led(true);

      if (millis() - hotspot_activation_timer >= 2 * 60 * 1000 && WiFi.softAPgetStationNum() == 0) {
          Serial.println("No clients connected for too long. Disabling the hotspot...");
          hotspot_activated = false; 
          blink_led(true, RED_LED, 2000);                                   // Blink RED LED to show device is restarting
          WiFi.softAPdisconnect();
          WiFi.mode(WIFI_STA);                                              // Reassigning wifi more for connecting to Wifi after hotspot deactivation
          delay(100);
          Serial.println("Enabling WiFi...");
            long wifi_timeout = millis();
            Serial.print("Connecting to:" + WiFi_SSID);
            Serial.println("\tPass: " + WiFi_PASS);

            WiFi.begin(WiFi_SSID.c_str(), WiFi_PASS.c_str());
            while (WiFi.status() != WL_CONNECTED && (millis() - wifi_timeout <= wifi_timeout_period * 1000)) {
              blink_led(true, GREEN_LED, 200);       // Blink green led to indicate WiFi is being connected
              Serial.print(".");
              delay(10);
            }
            if (WiFi.status() != WL_CONNECTED) {
              Serial.println("\nWiFi Connection Failed");
              WifiDisconnectiontimer = millis();
            } else {
              Serial.println("\nWiFi Connected");
              Serial.print("IP Address: ");
              Serial.println(WiFi.localIP());
              WifiConnected = true;
              connectToMQTT();                                  // Connecting back the MQTT
              if(!mqtt.connected()){
                Serial.println("MQTT connection failed. Restarting the device.");
                displayNotification("   MQTT Connection failed!!,,      Restarting the device.", true);
                delay(2000);
                ESP.restart();
              }
            } 
          
          // ESP.restart();
      } else if (WiFi.softAPgetStationNum() > 0) {
          hotspot_activation_timer = millis(); 
      }

      if (millis() - start_time > 2000 && hotspot_activated == true) {
          start_time = millis(); 
          Serial.println("[Info] Number of Clients: " + (String)WiFi.softAPgetStationNum());
          Serial.println(".   ");
      }
      server.handleClient();
    }
    yield();
}

void handleRoot() {  // Home Screen
  String response = String(htmlPage);
  
  // Build JSON data for sensor values
  String receivedData = "{";
  for(int i = 0; i < num_sensor_values; i++){
    receivedData += "\"" + sensor_names[i] + "\": \"" + sensor_values[i] + "\"";
    if (i < num_sensor_values - 1) 
      receivedData += ", ";
  }
  receivedData += "}";

  response.replace("_received_data", receivedData);
  response.replace("_min_threshold", String(threshold_min));
  response.replace("_max_threshold", String(threshold_max));
  response.replace("_wifi_ssid", WiFi_SSID);
  response.replace("_wifi_pass", WiFi_PASS);
  response.replace("_test_location", device_location);
  
  // Replace sensor values in the HTML template
  response.replace("id=\"current_moisture\"></span>", "id=\"current_moisture\">" + sensor_values[0] + "</span>");
  response.replace("id=\"soil_temperature\"></span>", "id=\"soil_temperature\">" + sensor_values[1] + "</span>");
  response.replace("id=\"soil_nitrogen\"></span>", "id=\"soil_nitrogen\">" + sensor_values[2] + "</span>");
  response.replace("id=\"soil_phosphorus\"></span>", "id=\"soil_phosphorus\">" + sensor_values[3] + "</span>");
  response.replace("id=\"soil_potassium\"></span>", "id=\"soil_potassium\">" + sensor_values[4] + "</span>");
  response.replace("id=\"soil_ph\"></span>", "id=\"soil_ph\">" + sensor_values[5] + "</span>");
  response.replace("id=\"e_conductivity\"></span>", "id=\"e_conductivity\">" + sensor_values[6] + "</span>");
  
  server.send(200, "text/html", response);
}


void handleSave() {
    String value_min = server.arg("moisture_min");
    String value_max = server.arg("moisture_max");
    String wifi_ssid = server.arg("wifi_ssid");
    String wifi_pass = server.arg("wifi_pass");
    String location = server.arg("device_location");

    int moisture_min = value_min.toInt();
    int moisture_max = value_max.toInt();

    Serial.printf("\nReceived Data RAW: Threshold Min: %d, Threshold Max: %d", moisture_min, moisture_max);
    Serial.printf("\nReceived Data RAW: WiFi SSID: %s, WiFi PASS: %s", wifi_ssid.c_str(), wifi_pass.c_str());
    Serial.printf("\nReceived Data RAW: Device Location: %s", location.c_str());

    moisture_min = constrain(moisture_min, 6, 90);
    moisture_max = constrain(moisture_max, moisture_min + 10, 100);
    
    // make changes apply instantly
    // Only update threshold_min if the value is provided
    if (!value_min.isEmpty()) {
        moisture_min = constrain(moisture_min, 6, 90);
        threshold_min = moisture_min;
    } else {
        moisture_min = threshold_min; // Retain the old value if not provided
    }

    // Only update threshold_max if the value is provided
    if (!value_max.isEmpty()) {
        moisture_max = constrain(moisture_max, moisture_min + 10, 100);
        threshold_max = moisture_max;
    } else {
        moisture_max = threshold_max; // Retain the old value if not provided
    }

    // Only update WiFi_SSID if the value is provided
    if (!wifi_ssid.isEmpty()) {
        WiFi_SSID = wifi_ssid;
    } else {
        wifi_ssid = WiFi_SSID; // Retain the old value if not provided
    }

    // Only update WiFi_PASS if the value is provided
    if (!wifi_pass.isEmpty()) {
        WiFi_PASS = wifi_pass;
    } else {
        wifi_pass = WiFi_PASS; // Retain the old value if not provided
    }

    // Only update device_location if the value is provided
    if (!location.isEmpty()) {
        device_location = location;
    } else {
        location = device_location; // Retain the old value if not provided
    }

    Serial.printf("\nReceived Data Constrained\n[Info] Threshold Min: %d\n[Info] Threshold Max: %d", moisture_min, moisture_max);
    Serial.printf("\n[Info] WiFi SSID: %s\n[Info] WiFi PASS: %s", wifi_ssid.c_str(), wifi_pass.c_str());
    Serial.printf("\n[Info] Device Location: %s \n", location.c_str());

    preferences.begin("moisture_config", false);
    preferences.putInt("_threshold_min", moisture_min);
    preferences.putInt("_threshold_max", moisture_max);
    preferences.putString("_WiFi_SSID", wifi_ssid);
    preferences.putString("_WiFi_PASS", wifi_pass);
    preferences.putString("_test_location", location);
    WiFi_SSID = preferences.getString("_WiFi_SSID", WiFi_SSID);
    WiFi_PASS = preferences.getString("_WiFi_PASS", WiFi_PASS);
    device_location = preferences.getString("_test_location", device_location);
    preferences.end();
    Serial.println("[Info] Values Saved Successfully!!!\n"); 
    server.send(200, "text/html", save_screen_html);
    delay(1000);
    Serial.println("Restarting the Device.");
    delay(100);
    displayNotification("       Settings saved.,      Restarting the device.", true);
    delay(2000);
    ESP.restart();
}
