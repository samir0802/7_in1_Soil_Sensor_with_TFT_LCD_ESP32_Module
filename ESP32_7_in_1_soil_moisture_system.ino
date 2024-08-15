/*
-----------------------------Real Time Soil Monitoring System---------------------------
-----------------------------NPK Sensor and ESP32--------------------------------------
*Code Developed by: Samir Thapa
*Date: 2024-06-17
*Version: 1.0
*----------------------------Device Functionality--------------------------------------
* Sends MOdbus commands to the (7 in 1)NPK,Temperature, Moisture, pH and Conductivity sensor to get the NPK values
* Sends the all seven values(N,P,K, Moisture, Temperature, pH and Conducivity) values to the ESP32
*ESP32 reads the values decodes them and displays them in the builtin LCD
*ESP32 also sends the values to MQTT broker using WiFi
*The device also has a button to activate the hotspot
*The device also uses it's hotspot to host a webpage to configure and control the device
*The device also has a LED to indicate the status of the device

*----------------------------Device Components-----------------------------------------
*ESP32
*(7 in 1)NPK Soil Moisture Temperature Conductivity Sensor(oprates in 5V)
*LCD
*Button
*LED

*----------------------------Device Pins-----------------------------------------------
*ESP32
*Button: 0
*LED: 9
*LCD: SCK:14, SDO: 12, SDI: 13, CS: 15, RS: 2, RST: rst_pin of esp32
*NPK Sensor: Serial of esp32
*----------------------------Device Libraries------------------------------------------
*OneButton
*WebServer
*Preferences
*WiFi
*FS
*SD
*SPI
*HardwareSerial
*PubSubClient
*TFT_eSPI
*WiFiClientSecure
*ArduinoJson


*----------------------------Hardware Features-----------------------------------------
*The device has a button to activate the hotspot
*The device has a LED to indicate the status of the device
*The device has a (7 in 1)NPK Soil Moisture Temperature Conductivity Sensor 
*The device has a LCD to display the NPK, moisture, temperature, pH and conductivity values
*The device has a ESP32 to control the device
*The device has a WiFi module within the ESP32 Chip to connect to the internet

*----------------------------Software Features-----------------------------------------
*The device sends MOdbus commands to the (7 in 1)NPK,Temperature, Moisture, pH and Conductivity sensor to get the NPK values
*esp32 reads the values decodes them and displays them in the builtin LCD
*esp32 also sends the values to MQTT broker using WiFi
*The device uses hotspot to host a webpage to configure and control the device
*The device also use SPIFFS to store the values in the MicroSD card

*----------------------------Serial AT Commands-----------------------------------------
Command: Hello, AT, Ramlaxman to get informations from serial terminal
Command: 'Set_moisture_threshold_min=', Set the minimum threshold value for the moisture sensor
Command: 'Set_moisture_threshold_max=', Set the maximum threshold value for the moisture sensor
Command: 'Set_sensor_test_timer=', Set the time interval to test the sensor
*/
#include <HardwareSerial.h>
#include <OneButton.h>    // Include the OneButton library
#include <Preferences.h>  //Include the Preferences library for storing the preferences

#include <WebServer.h>  // Include the WebServer library for hosting a webpage
#include <WiFi.h>  //Include the WiFi library for connecting to the internet
#include <WiFiClientSecure.h>// Include the WiFiClientSecure library for connecting to the internet
#include <PubSubClient.h>   // Include the PubSubClient library for sending and receiving messages from the MQTT broker


#include "FS.h"  //Include the FS library for reading and writing files
#include "SPIFFS.h" // Include the SPIFFS library fro reading and writing files
#include "ArduinoJson.h" // Include the ArduinoJson library for parsing JSON data
#include "SPI.h"  // Include the SPI library for SPI communication
#include "SD.h"   // Include the SD library for reading and writing files

#include <PNGdec.h> // PNG decoder Library
#include "ramlaxmann_logo.h" // RL Logo Image is stored in an 8-bit array
#include "soil_monitor_image.h" // Image of Soil MOnitor logo
#include "SD_Warning_image.h"   // Image of SD Card Not inserted warning
#include "Flash_Initialization_Info_Image.h" //Image of SPIFFS Initialization Info

#include <TFT_eSPI.h>  //Include the TFT_eSPI library for controlling the LCD
#include <JPEGDecoder.h> // JPEG decoder library
// #include <lvgl.h>   // Include the lvgl library for creating the GUI

//Include library for RTOS
// #include <FreeRTOS.h> // for running rask in dual core
#include <esp_system.h>
#include <esp_task_wdt.h>


// Define the pins for the MOSI, MISO, SCLK, and CS of the microSD card
#define TF_MISO 19
#define TF_MOSI 23
#define TF_SCLK 18
#define TF_CS   5
SPIClass TF_Card(VSPI); // Define the SPI class for the microSD card
File file;  // Create a file object


// define 3 pins for a single RGB LED
#define LED_R_PIN 4   // Pin for the LED
#define LED_G_PIN 16  // Pin for the LED
#define LED_B_PIN 17  // Pin for the LED

// values for color of LED for blink function
const int NO_LED = 0;
const int RED_LED = 1;
const int GREEN_LED = 2;
const int BLUE_LED = 3;

unsigned long previousMillis = 0;  // Variable to store the previous time
//const long interval = 1000;         // Interval at which to blink (milliseconds)
bool ledState = false;  // Current state of the LED

// arrays to hold multiple values from the sensor
const int num_sensor_values = 7;
String sensor_values[num_sensor_values];
uint8_t sensor_percent[num_sensor_values];
String sensor_names[num_sensor_values] = { "Moisture", "Temperature", "N", "P", "K", "pH", "Conductivity" };
String sensor_units[num_sensor_values] = {"%", "C", "mg/Kg", "mg/Kg","mg/Kg","","us/cm"};

uint threshold_min;
uint threshold_max;
String MQTT_broker_ID;  // MQTT broker ID
int MQTT_broker_port;   // MQTT broker port
const char* mqttUsername = "";
const char* mqttPassword = "";

String mqttClientId;
String chipIdString;
String mqttRootPublishTopic = "SS/NPK/";   // MQTT Main Directory
String mqttRootSubscribeTopic = "SS/NPK/"; // MQTT Main Directory
String mqttPublishTopic;                       // Will contain full Topic Desctiption
String mqttSubscribeTopic;

String device_location;

const char* mqttLastWillTopic;
String mqttLastWillMessage = ", Offline.";

String WiFi_SSID;  // WiFi SSID
String WiFi_PASS;  // WiFi password
WiFiClient mqtt_client;     // MQTT client
PubSubClient mqtt(mqtt_client); // MQTT client

// Set the timer for testing the sensors
long sensor_test_timer = 6000;   // Set the timer for testing the sensors
long serial_print_timer = 6000;  // Set the timer for printing the sensor values to the serial monitor
long current_test_timer;
long current_print_timer;

long SD_write_timer;
long log_duration_min = 30;
int log_count;
int current_log_count=0;
int last_log_count =0;

bool SD_Card_Inserted = false;
bool SPIFFS_Mounted = false;


long wifi_check_timer ;
long wifi_timeout_period = 15;
long MQTT_send_timer;
long MQTT_post_period = 15;
long wifi_disconnect_period_minutes = 25;
long WifiDisconnectiontimer;

// bool variables to store condition of wifi connection
bool WifiConnected = false;

const int btn = 0;  //boot button;              // User Switch, long press to activate hotspot

//Function Prototypes
void hotspot();
void handle_webserver();
void blink_led( bool led_active, const int led_color, int interval = 0);

bool hotspot_activated = false;

OneButton button(btn, true);            // [Active LOW]   //attach a button on pin 0 to the library
WebServer server(80);
Preferences preferences;  // Create a Preferences object to read and write preferences
HardwareSerial Soil_Modbus(1);  // Creating Modbus object for Soil sensor 



// #define ST7796_DRIVER
// #define TFT_WIDTH  320
// #define TFT_HEIGHT 480 // 
// #define TFT_BL   27            // LED back-light control pin
// #define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)
// #define TFT_MISO 12
// #define TFT_MOSI 13 // In some display driver board, it might be written as "SDA" and so on.
// #define TFT_SCLK 14
// #define TFT_CS   15  // Chip select control pin
// #define TFT_DC   2  // Data Command control pin
// #define TFT_RST  -1  // Reset pin (could connect to Arduino RESET pin)
// #define TFT_BL   27  // LED back-light

// #define TOUCH_CS 33     // Chip select pin (T_CS) of touch screen
TFT_eSPI tft = TFT_eSPI();      // Creating TFT object for display

// Creating a taskhandler for core 0
#define WDT_TIMEOUT 60
TaskHandle_t xButtonHandlingTask;
volatile bool buttonDoubleClickFlag = false; // bool for button double click indication
volatile bool SDLoggerEnableFlag = false;   // bool to check SD logger has been enabled or not
volatile bool buttonLongPressFlag = false;  // bool for button long press indication


void setup() {
  //nature of pins
  pinMode(LED_R_PIN, OUTPUT);       // Set the LED pin as an output
  pinMode(LED_G_PIN, OUTPUT);       // Set the LED pin as an output
  pinMode(LED_B_PIN, OUTPUT);       // Set the LED pin as an output

  blink_led(false, NO_LED);
  delay(1000);
  blink_led(true, GREEN_LED);       // Turn GREEN lED to indicate the device ON


  Soil_Modbus.begin(9600, SERIAL_8N1, 35, 22);
  Serial.begin(115200);  // Start the serial communication
  if (!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      SPIFFS_Mounted = false;
      // return;
  }else{
      Serial.println("SPIFFS mounted successfully");
      SPIFFS_Mounted = true;
  }

  xTaskCreatePinnedToCore(
  ButtonTASK_CORE0
  , "Button_Hanlder"      // Name of task
  , 3072                  // 3KB stack size
  , NULL                  //pvParameters
  , 2                     // Priority
  , &xButtonHandlingTask  //Task handling variable
  , 0);

  //Task stack needed for task
  Serial.print("Core 0 Task Stack size: ");
  Serial.println(uxTaskGetStackHighWaterMark(xButtonHandlingTask));

  // Attach interrupt to the button pin
  // attachInterrupt(digitalPinToInterrupt(btn), handleButtonInterrupt, FALLING);
 

  //tft initialization
  Serial.print("Printing on LCD");
  tft.init();
  // tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);

  int x = (tft.width() - 320)/ 2 - 1;
  int y = (tft.height() - 480)/ 2 - 1;

  drawArrayJpeg(RL_Logo, sizeof(RL_Logo), 0, 0); // Darw the RL logo jpeg image from memory
  delay(3000);
  drawArrayJpeg(Soil_Monitor_Logo, sizeof(Soil_Monitor_Logo), 0, 0); // Draw the Soil Monitor Image
  delay(2000);

  // Start the microSD card
  long SD_Load_or_Not_Timer = millis();
  while(!setup_SD() && millis() - SD_Load_or_Not_Timer < 5000){
  }
  if(!SD_Card_Inserted){
    if(SPIFFS_Mounted){
      drawArrayJpeg(Flash_Init_Info_Image, sizeof(Flash_Init_Info_Image), 0, 0); // Darw the RL logo jpeg image from memory
      delay(3000);
    }
  }
  delay(2000);

  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(4);
  updateTFTDisplay();

  pinMode(btn, INPUT_PULLUP);          // Set the button pin as an input with a pullup resistor
  button.attachClick(SD_Logger); // Attach the SD logging to the double click of button
  button.attachLongPressStart(hotspot);    // Attach the hotspot function to the button
  button.setPressMs(4000);          // Set the time for a long press

  MQTT_send_timer = 60000 * log_duration_min;  // Set the timer for sending the sensor values to the MQTT broker

  preferences.begin("moisture_config", false);  // Start the preferences object with the namespace "sensor_settings"
  // preferences.clear();                // Clear the preferences
  WiFi_SSID = preferences.getString("_WiFi_SSID", "evonelectric_2");   //TP-LINK_A5AFEA         // Get the WiFi SSID from the preferences
  WiFi_PASS = preferences.getString("_WiFi_PASS", "CLB2778B39");        //79641494          // Get the WiFi password from the preferences
  threshold_min = preferences.getInt("_threshold_min", 20);                     // Get the minimum threshold value from the preferences
  threshold_max = preferences.getInt("_threshold_max", 80);                     // Get the maximum threshold value from the preferences
  MQTT_broker_ID = preferences.getString("_MQTT_broker_ID", "202.52.240.148");  // Get the MQTT broker ID from the preferences
  MQTT_broker_port = preferences.getInt("_MQTT_broker_port", 5065);             // Get the MQTT broker port from the preferences
  device_location = preferences.getString("_device_location", "Somewhere");  // Get the device location from the preferences
  preferences.end();  // End the preferences object

  Serial.printf("\n\n---------------------------------Soil Monitoring Device Started-----------------------------------\n\n");                                                                                                                                                                                           // Print the message "Device Started" to the serial monitor
  Serial.printf("[INFO] Threshold Min : %d\n [INFO] Threshold Max : %d\n [INFO] WiFi SSID: %s\n [INFO] WiFi PASSWORD: %s\n", threshold_min, threshold_max, WiFi_SSID.c_str(), WiFi_PASS.c_str());  // Print the threshold values and MQTT broker ID to the serial monitor
  // Serial.printf("[INFO] MQTT Broker ID : %s\n [INFO] MQTT Broker Port : %d\n [INFO] WiFi SSID: %s\n [INFO] WiFi PASSWORD: %s\n", threshold_min, threshold_max, MQTT_broker_ID.c_str(), MQTT_broker_port, WiFi_SSID.c_str(), WiFi_PASS.c_str());  // Print the threshold values and MQTT broker ID to the serial monitor
  
  // Get the ESP32 Chip ID
  uint64_t chipId = ESP.getEfuseMac();
  chipIdString = String(chipId, HEX);

  //configure correct IDs
  mqttClientId = device_location + "_";
  mqttClientId += "_NPK";
  mqttClientId += chipIdString;


  mqttPublishTopic = mqttRootPublishTopic + device_location;
  mqttSubscribeTopic = mqttRootSubscribeTopic + device_location + "/#";

  Serial.printf("[INFO] MQTT Client ID : %s\n", mqttClientId.c_str());  // Print the MQTT client ID to the serial monitor
  Serial.printf("[INFO] MQTT Publish Topic : %s\n", mqttPublishTopic.c_str());  // Print the MQTT publish topic to the serial monitor
  Serial.printf("[INFO] MQTT Subscribe Topic : %s\n", mqttSubscribeTopic.c_str());  // Print the MQTT subscribe topic to the serial monitor
  Serial.print("[INFO] Device MQTT Client ID: "); Serial.println(mqttClientId);  // Print the device MQTT client ID to the serial monitor

  //Serial Configuration Instructions
  // String instruction = "\n\n\n---------------Soil MOnitoring System----------------\n";
  // instruction += "Commands: \n";  // Print the commands to the serial monitor
  // instruction += "1. 'Set_moisture_threshold_min=': Set the minimum threshold value for the moisture sensor\n";  // Print the command to set the minimum threshold value to the serial monitor
  // instruction += "2. 'Set_moisture_threshold_max=': Set the maximum threshold value for the moisture sensor\n";  // Print the command to set the maximum threshold value to the serial monitor
  // instruction += "3. 'Set_device_location=': Set the device location\n";  // Print the command to set the device location to the serial monitor
  // Serial.println(instruction);  // Print the instructions to the serial monitor

  // //--------------------------------------Wifi Login Part--------------------------------------------------------//
  WiFi.mode(WIFI_STA);
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
    connectToMQTT();
  }

  current_test_timer = millis();
  current_print_timer = millis();
  wifi_check_timer = millis();
  MQTT_send_timer  = millis();
  SD_write_timer = millis();
  delay(2000);
}
void loop() {

  mqtt.loop();  // Loop the MQTT client
  // button.tick();  // Check the button state
  handle_webserver();  // Handle the webserver


  // Check the time for testing the sensors
  if (millis() - current_test_timer >= sensor_test_timer) {
    current_test_timer = millis();  // Reset the timer
    blink_led(true, BLUE_LED);       // Turn Blue LED to indicate the sensor is reading the soil parameters
    test_moisture_sensors();          // Test the moisture sensors
    delay(100);
    print_sensor_values();            // Print the sensor values
    if(!buttonLongPressFlag && !buttonDoubleClickFlag){
      updateTFTDisplay();
    }
    blink_led(false, NO_LED);       // Turn off the LEDs to show end of sensor reading
  }
  delay(1000);
  if(millis() - wifi_check_timer >= wifi_timeout_period*1000){
    if (WiFi.status() != WL_CONNECTED) {
      WifiConnected = false;
      Serial.println("Reconnecting to WiFi...");
      WiFi.begin(WiFi_SSID.c_str(), WiFi_PASS.c_str());
      delay(2000);
    }else{
      Serial.println("Wifi Connected!!.");
      WifiConnected = true;
    }
    wifi_check_timer = millis();
  }
  if(millis() - WifiDisconnectiontimer >= wifi_disconnect_period_minutes*60*1000){
    Serial.println("Wifi connection Failed for too long. Restarting the device...");
    delay(100);
    blink_led(true, RED_LED, 2000);                                                       // Blink RED LED to show device is restarting 
    // ESP.restart();
  }

  if(WifiConnected && (millis()- MQTT_send_timer >= MQTT_post_period*1000)){
    // blink_led(true, BLUE_LED, 2000);                                                      // blink BLUE led to show MQTT publish has started
    String soil_message= "";
    for(int i = 0; i < num_sensor_values; i++){
      soil_message += sensor_names[i] +"="+sensor_values[i] + ",";
    }
    publishToMQTT(mqttPublishTopic.c_str(), soil_message.c_str());
    MQTT_send_timer = millis();
    // blink_led(false, NO_LED);
  }
  delay(2000);
}

// LED ON function
void blink_led( bool led_active, const int led_color, int interval){
    int current_LED;
    if (led_active) {
        if (led_color == RED_LED) {
          current_LED = LED_R_PIN;
            digitalWrite(LED_R_PIN, LOW); // Turn on the red LED
            digitalWrite(LED_G_PIN, HIGH); // Turn off the green LED
            digitalWrite(LED_B_PIN, HIGH); // Turn off the blue LED
        } else if (led_color == GREEN_LED) {
            current_LED = LED_G_PIN;
            digitalWrite(LED_R_PIN, HIGH); // Turn off the red LED
            digitalWrite(LED_G_PIN, LOW); // Turn on the green LED
            digitalWrite(LED_B_PIN, HIGH); // Turn off the blue LED
        } else if (led_color == BLUE_LED) {
          current_LED = LED_B_PIN;
            digitalWrite(LED_R_PIN, HIGH); // Turn off the red LED
            digitalWrite(LED_G_PIN, HIGH); // Turn off the green LED
            digitalWrite(LED_B_PIN, LOW); // Turn on the blue LED
        }
    } else {
        digitalWrite(LED_R_PIN, HIGH); // Turn off the red LED
        digitalWrite(LED_G_PIN, HIGH); // Turn off the green LED
        digitalWrite(LED_B_PIN, HIGH); // Turn off the blue LED
    }
    if(interval > 0){
      long current_millis = millis();
      yield();
      while(millis()-current_millis <= interval){
          digitalWrite(current_LED, LOW);
          delay(100);
          digitalWrite(current_LED, HIGH);
          delay(100);
      }
    }

}



// // Serial Configuration Functions, read serial if At commands match update the preferences
// void serial_configuration(){
//     String set_threshold_min = "Set_moisture_threshold_min=";
//     String set_threshold_max = "Set_moisture_threshold_max=";
//     String set_device_location = "Set_device_location=";

//     String response = Serial.readStringUntil('\n');  // Read the serial input until a new line character is received
//     if(response.startsWith("hello") || response.startsWith("AT") || response.startsWith("Ramlaxman")){
//         Serial.println("Hello, I am ESP32 NPK Soil Monitor");
//     } else if(response.startsWith(set_threshold_min)){
//         threshold_min = response.substring(set_threshold_min.length()).toInt();  // Get the threshold value from the serial input
//         preferences.begin("moisture_config", false);  // Start the preferences object with the namespace "sensor_settings"
//         preferences.putInt("_threshold_min", threshold_min);  // Put the threshold value in the preferences
//         preferences.end();  // End the preferences object
//         Serial.printf("[INFO] Threshold Min : %d\n", threshold_min);  // Print the threshold value to the serial monitor
//     } else if(response.startsWith(set_threshold_max)){
//         threshold_max = response.substring(set_threshold_max.length()).toInt();  // Get the threshold value from the serial input
//         preferences.begin("moisture_config", false);  // Start the preferences object with the namespace "sensor_settings"
//         preferences.putInt("_threshold_max", threshold_max);  // Put the threshold value in the preferences
//         preferences.end();  // End the preferences object
//         Serial.printf("[INFO] Threshold Max : %d\n", threshold_max);  // Print the threshold value to the serial monitor
//     }else if(response.startsWith(set_device_location)){
//         String location = "";
//         location = response.substring(set_device_location.length());  // Get the device location from the serial input
//         for(int i=0; i<location.length(); i++){
//             if (location[i] != '\r' && location[i] != '\n') {
//                 device_location += toupper(location[i]);  // Convert the device location to uppercase
//             }
//         }
//         preferences.begin("moisture_config", false);  // Start the preferences object with the namespace "sensor_settings"
//         preferences.putString("_device_location", device_location);  // Put the device location in the preferences
//         preferences.end();  // End the preferences object

//         mqttClientId  = device_location + "_NPK"+ chipIdString;
//         mqttPublishTopic = mqttRootPublishTopic + device_location;
//         mqttSubscribeTopic = mqttRootSubscribeTopic + device_location + "/#";

//         Serial.printf("[INFO] Device Location : %s\n", device_location.c_str());  // Print the device location to the serial monitor
//         Serial.printf("[INFO] MQTT Client ID : %s\n", mqttClientId.c_str());  // Print the MQTT client ID to the serial monitor
//         Serial.printf("[INFO] MQTT Publish Topic : %s\n", mqttPublishTopic.c_str());  // Print the MQTT publish topic to the serial monitor
//         Serial.printf("[INFO] MQTT Subscribe Topic : %s\n", mqttSubscribeTopic.c_str());  // Print the MQTT subscribe topic to the serial monitor
//         mqtt.disconnect();
//         connectToMQTT();  // Connect to the MQTT broker
//     }
// }

// String toUpperCase(String inputString) {
//   String outputString = "";
//   int strLength = inputString.length();

//   for (int i = 0; i < strLength; i++) {
//     char c = inputString.charAt(i);
//     c = toupper(c);
//     outputString += c;
//   }

//   return outputString;
// }
