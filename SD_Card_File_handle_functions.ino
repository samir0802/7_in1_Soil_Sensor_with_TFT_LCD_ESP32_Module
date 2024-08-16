const char baseFilename[] = "/Soil_parameters_log_";  // Define the base filename for the data file
// define the microSD card and check if SD card is inserted or not
bool setup_SD(){
    // Check if the microSD card is available
    if(SD.begin(TF_CS, TF_Card)){
        Serial.println("SD Card Inserted");  // Print the message "SD Card Initialized" to the serial monitor
        display_Card_type();  // Identify the type of the microSD card

        Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
        Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
        listDir(SD, "/", 2);

        char directoryname[40] = "/Soil_Monitor_";
        char filename[120] = "/Soil_Monitor_/Soil_Parameters.xlsx";
      
        Serial.println("\n\n-----------------------------------------------------");
        Serial.print("[Info] Directory name: "); Serial.println(directoryname);
        Serial.print("[Info] File name: "); Serial.println(filename);

        // Create a filename with the current date and time
        // sprintf(filename, "%s%s",directoryname, filename);
        Serial.print("Directory path: "); Serial.println(directoryname);
        Serial.print("[Info] Filename: "); Serial.println(filename);
        createDir(SD, directoryname);

        // Create a file for writing the sensor values
        file = SD.open(filename, FILE_WRITE);
        if(!file){
            Serial.println("[Warning] Failed to open file for appending");
            // Serial.println("[Info] Press Reset to Restart the Process");     //Uncomment this when SD-card is inserted
            // while(true);
        } else {
          Serial.println("[Info] File Successfully Created!");
        }
        file.printf("Soil Parameters Log as of %s\n", directoryname);
        file.printf("Log Duration: %d minutes\n", log_duration_min);
        file.printf("Log, Moisture(%) , Temperature(C)  ,   N(mg/kg)   ,   P(mg/kg)   ,   K(mg/kg)   ,   pH   , Conductivity(us/cm) ,\n");
        file.close();
        SD_write_timer = millis();  // Set the timer for writing to the SD card
        SD_Card_Inserted = true;
        return true;
    }else{
        Serial.println("SD Card not inserted.");  // Print the message "SD Card Initialization Failed" to the serial monitor
        drawArrayJpeg(SD_Warning_Image, sizeof(SD_Warning_Image), 0, 0); // Darw the jpeg image from memory
        delay(3000);
        SD_Card_Inserted = false;
        return false;
    }
}

//SD card functions
void display_Card_type(){
     uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
}

void SD_Logger(){
  if(!SD_Card_Inserted){
    Serial.println("No SD card present. Data wont be logged.");
    displayNotification("           NO SD Card.,,   Data won't be logged.,,  Please insert SD Card.", true);
    delay(2000);
    esp_task_wdt_reset();
    tft.fillScreen(TFT_BLACK);
    updateTFTDisplay();
    return;
  }
  esp_task_wdt_reset();
  Serial.printf("[Info] Beginning SD card Logging for %d minutes.\n", log_duration_min);
  buttonDoubleClickFlag = true;
  SDLoggerEnableFlag = true;
    // Check the time for writing to the SD card
  update_TFT_SD_countdown();
  long Logging_timer = millis();
  SD_write_timer = millis();
  current_log_count = log_count;
  while(millis() - Logging_timer < log_duration_min*60000){
    esp_task_wdt_reset();
      if(current_log_count > last_log_count){
        String sensor_values_str=String(log_count)+",";
        for(int i = 0; i < num_sensor_values; i++){
          sensor_values_str +=sensor_values[i] + ",";
        }
        sensor_values_str += "\n";

        writeFile(SD, "/Soil_Monitor_/Soil_Parameters.xlsx", sensor_values_str.c_str()); // Write the sensor values to the SD card
        String read_data = readFile(SD, "/Soil_Monitor_/Soil_Parameters.xlsx");
        if(read_data != ""){
          Serial.println("Log Count, Moisture, Temperature,   N,   P,   K,   pH, Conductivity");
          Serial.println(read_data);
          Serial.print("\n\n");
        }
        last_log_count = current_log_count;
        esp_task_wdt_reset();
      }
    // Reset the watchdog timer periodically

  }// while loop
  SDLoggerEnableFlag = false;
  log_count = 0;
  current_log_count = 0;  // reset the log count after the SD logging completed.
  last_log_count =0;
  tft.fillScreen(TFT_CYAN);
  tft.setTextColor(TFT_BLACK, TFT_CYAN);
  tft.setTextSize(2);  // Larger text size for the heading
  tft.setTextDatum(MC_DATUM);  // Set datum to middle center
  tft.drawString("Data Logging", tft.width() / 2,tft.height()/2);
  tft.drawString("Completed!", tft.width() / 2, tft.height()/2 + 50);
  delay(4000);
  tft.fillScreen(TFT_BLACK);
  esp_task_wdt_reset();
  updateTFTDisplay();
}


void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){

    if(fs.exists(path)){
    Serial.printf("This Directry: %s Already Exists!!!\n", path);
    return;
    } else Serial.printf("Creating Dir: %s\n", path);
    
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

String readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return "";
    }

    String content = "";
    while(file.available()){
        char c = file.read();
        if(c == '|'){
            content += " , ";
        } else if(c == '\n'){
            content += "\n";
        } else {
            content += c;
        }
    }

    file.close();
    return content;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    if(fs.exists(path)){
      Serial.println("[Info] File Already Exists!!!");
      // return; 
    } else  Serial.printf("[Info] Writing file: %s\n", path);
    
    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    
    // Check the time for writing to the SD card
    // if(file.printf("| %s | %s | %s | %s | %s | %s | %s |\n",sensor_values[0], sensor_values[1], sensor_values[2], sensor_values[3], sensor_values[4], sensor_values[5], sensor_values[6])){
    //   Serial.println("File written");
    // }  // Write the sensor values to the file
    // else{
    //   Serial.println("Write failed");
    // }
    SD_write_timer = millis();  // Reset the timer
    
    if(file.print(message)){
       Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

