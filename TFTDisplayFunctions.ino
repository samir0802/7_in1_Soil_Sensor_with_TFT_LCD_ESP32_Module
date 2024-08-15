
// Function to update the TFT display
void updateTFTDisplay() {
  // Define block dimensions and positions
  int gap = 10;  // Gap between blocks
  int radius = 10;  // Radius for rounded corners
  int blockHeight = 95;  // Height of each block (increased to 95 pixels)
  int blockWidth = (tft.width() - 30) / 2;  // Half width of the screen minus gap
  // Draw the heading with rounded rectangle
  int headingHeight = 52;
  int headingRadius = 10;
  
  // Define colors for the blocks
  uint16_t colors[num_sensor_values] = {
      TFT_CYAN, TFT_RED, TFT_SKYBLUE, TFT_YELLOW, TFT_DARKGREY, TFT_CYAN, TFT_ORANGE
  };

  tft.fillRoundRect(0, 0, tft.width(), headingHeight, headingRadius, TFT_DARKGREY);  // Background color for the heading
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setTextSize(2);           // Larger text size for the heading
  tft.setTextDatum(MC_DATUM);  // Set datum to middle center
  tft.drawString("Properties", tft.width() / 2, headingHeight / 2);

  // Draw sensor values in blocks
  for (int i = 0; i < num_sensor_values; i++) {
    int row = i / 2;  // Calculate the row number
    int col = i % 2;  // Calculate the column number

    int xPos = (col == 0) ? gap : blockWidth + 2 * gap;  // Calculate the x position
    int yPos = headingHeight + gap + row * (blockHeight + gap);  // Calculate the y position

    if (i == num_sensor_values - 1) {  // Last value uses the full width
      xPos = gap;
      blockWidth = tft.width() - 2 * gap;
    }

    // Draw a rounded rectangle for the block
    tft.fillRoundRect(xPos, yPos, blockWidth, blockHeight, radius, colors[i]);

    // Draw the sensor name inside the block
    tft.setTextColor(TFT_BLACK, colors[i]);
    tft.setTextDatum(MC_DATUM);  // Set datum to middle center
    tft.setTextSize(1);           // Smaller text size for the sensor name
    tft.drawString(sensor_names[i], xPos + blockWidth / 2, yPos + 15);  // Sensor name at y = 10

    // Draw the sensor value inside the block
    tft.setTextSize(2);  // Larger text size for the sensor value
    tft.drawString(sensor_values[i], xPos + blockWidth / 2, yPos + 52);  // Sensor value

    // Draw the sensor unit below the sensor value (80 pixels from the top of the block)
    tft.setTextSize(1);  // Same size as sensor name
    tft.drawString(sensor_units[i], xPos + blockWidth / 2, yPos + 82);  // Sensor unit at y = 80
  }
  if(SDLoggerEnableFlag && current_log_count >= last_log_count){
    // Display the counter in the lower right side
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(BR_DATUM);  // Bottom right datum
    tft.drawString(String(log_count), tft.width() - gap, tft.height() - gap);
  }
}

// Function to display the countdown
void update_TFT_SD_countdown() {
  esp_task_wdt_reset();
  tft.fillScreen(TFT_CYAN);  // Clear the screen with white
  for (int i = 3; i > 0; i--) {
    esp_task_wdt_reset();
    // Draw the heading
    tft.fillRect(0, 0, tft.width(), 50, TFT_CYAN);  // Background color for the heading
    tft.setTextColor(TFT_BLACK, TFT_CYAN);
    tft.setTextSize(2);  // Larger text size for the heading
    tft.setTextDatum(MC_DATUM);  // Set datum to middle center
    tft.drawString("SD Card Log", tft.width() / 2, 30);
    tft.drawString("Starting in", tft.width() / 2, 80);

    // Draw the countdown number in the middle of the screen
    tft.setTextSize(4);  // Large text size for the countdown number
    tft.setTextColor(TFT_BLACK, TFT_CYAN);
    tft.setTextDatum(MC_DATUM);  // Set datum to middle center
    tft.drawString(String(i), tft.width() / 2, tft.height() / 2);
    esp_task_wdt_reset();
    delay(1000);  // Wait for 1 second
  }

  // Display "Ready!" after countdown
  tft.fillScreen(TFT_CYAN);  // Clear the screen with white
  esp_task_wdt_reset();
  // Draw the heading
  tft.fillRect(0, 0, tft.width(), 50, TFT_CYAN);  // Background color for the heading
  tft.setTextColor(TFT_BLACK, TFT_CYAN);
  tft.setTextSize(2);  // Larger text size for the heading
  tft.setTextDatum(MC_DATUM);  // Set datum to middle center
  tft.drawString("SD Card Log", tft.width() / 2, 30);
  tft.drawString("Starting in", tft.width() / 2, 80);
  esp_task_wdt_reset();
  // Draw "Ready!" in the middle of the screen
  tft.setTextSize(3);  // Large text size for "Ready!"
  tft.setTextColor(TFT_BLACK, TFT_CYAN);
  tft.setTextDatum(MC_DATUM);  // Set datum to middle center
  tft.drawString("Ready!", tft.width() / 2, tft.height() / 2);
  esp_task_wdt_reset();
  delay(1000);  // Wait for 1 second
  buttonDoubleClickFlag = false;
  tft.fillScreen(TFT_BLACK); // fill the screen with black
  esp_task_wdt_reset();
  updateTFTDisplay();
  esp_task_wdt_reset();
}

// Function to display Hotspot information
void displayHotspotInfo() {
  esp_task_wdt_reset();
  tft.fillScreen(TFT_CYAN);  // Clear the screen with white

  // Display "Hotspot ON!" - center aligned, font 2
  tft.setTextColor(TFT_BLACK, TFT_CYAN);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);  // Middle center datum
  tft.drawString("Hotspot ON!", tft.width() / 2, tft.height() / 6);
  esp_task_wdt_reset();

  // Display "SSID: RL_Soil_Moisture" - left aligned, font 1
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);  // Top left datum
  tft.drawString("SSID: RL_Soil_Moisture", 10, tft.height() / 3);
  esp_task_wdt_reset();

  // Display "Pass: ramlaxman" - left aligned, font 1
  tft.drawString("Pass: ramlaxman", 10, tft.height() / 3 + 22);

  // Display "Go to 192.168.4.1 to change settings." - center aligned, font 2
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);  // Middle center datum
  tft.drawString("Go to", tft.width() / 2, tft.height() / 2 + 30);
  tft.drawString("192.168.4.1", tft.width()/2, tft.height()/2 + 50);
  tft.drawString("to change settings.", tft.width() / 2, tft.height() / 2 + 70);
  esp_task_wdt_reset();
  delay(15000);
  buttonLongPressFlag = false;
  // buttonFlag = false;
  tft.fillScreen(TFT_BLACK);
  esp_task_wdt_reset();
  updateTFTDisplay();
  esp_task_wdt_reset();
}