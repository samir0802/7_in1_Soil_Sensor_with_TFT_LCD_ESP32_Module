
const uint8_t Temp_Mois_queryData[] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x02, 0x24, 0x0A};    // Query data to get the temperature and moisture values "01 03 00 06 00 02 24 0A"
const uint8_t NPK_queryData[] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x03, 0x65, 0xCD};          // Query data to get the NPK values                      "01 03 00 1E 00 03 65 CD"
const uint8_t pH_queryData[] = {0x01, 0x03, 0x00, 0x09, 0x00, 0x01, 0x54, 0x08};           // Query data to get the pH value                        "01 03 00 09 00 01 54 08"
const uint8_t Conductivity_queryData[] = {0x01, 0x03, 0x00, 0x08, 0x00, 0x01, 0x05, 0xC8}; // Query data to get the conductivity value              "01 03 00 08 00 01 05 C8"
String receivedData;    


void test_moisture_sensors() {
    // Send the command to the sensor
    test_temperture_moisture();  // Test the temperature and moisture sensor
    test_NPK();  // Test the NPK sensor
    test_pH();  // Test the pH sensor
    test_conductivity();  // Test the conductivity sensor
    if(SDLoggerEnableFlag){
      // check if new values are added
      for(int i = 0; i < num_sensor_values; i++){
        if(sensor_values[i] == ""){
          current_log_count = last_log_count;
          return;
        }
      }
      if (sensor_values[0] != "" && sensor_values[1] != "" && sensor_values[2] != "" && sensor_values[3] != "" && sensor_values[4] != "" && sensor_values[5] != "" &&  sensor_values[6] != ""){
        log_count ++;
        current_log_count = log_count;
      }
  }
}

void test_temperture_moisture() {
    // Send the query data to the sensor
    for (int i = 0; i < sizeof(Temp_Mois_queryData); i++) {
        Soil_Modbus.write(Temp_Mois_queryData[i]); // Write each element of the query data array to the sensor
    }
    read_serial_data("Temp_Mois");
    delay(500);  // Delay for the sensor to respond
}

void test_NPK() {
    // Send the query data to the sensor
    for (int i = 0; i < sizeof(NPK_queryData); i++) {
        Soil_Modbus.write(NPK_queryData[i]); // Write each element of the query data array to the sensor
    }
    read_serial_data("NPK");  // Read 11 bytes of data from the sensor
    delay(500);  // Delay for the sensor to respond

}

void test_pH() {
    // Send the query data to the sensor
    for (int i = 0; i < sizeof(pH_queryData); i++) {
        Soil_Modbus.write(pH_queryData[i]); // Write each element of the query data array to the sensor
    }
    read_serial_data("pH");  // Read 7 bytes of data from the sensor
    delay(500);  // Delay for the sensor to respond
}

void test_conductivity() {
    // Send the query data to the sensor
    for (int i = 0; i < sizeof(Conductivity_queryData); i++) {
        Soil_Modbus.write(Conductivity_queryData[i]); // Write each element of the query data array to the sensor
    }
    read_serial_data("Conductivity");  // Read 7 bytes of data from the sensor
    delay(500);  // Delay for the sensor to respond
}

void read_serial_data(String name) {
  receivedData = "";
    String received_Data = "";
  unsigned long startTime = millis();
      while(!Soil_Modbus.available() && millis() - startTime <2000 ){
        delay(500);
        yield();
      }
      if (!Soil_Modbus.available())  //If no data received, exit function
      {
        Serial.println("\n[Info] Timeout Waiting for data...\n");
        received_Data = "";
      }
        while(Soil_Modbus.available()){
          received_Data = Soil_Modbus.readString();
        }
        for(int i = 0; i < received_Data.length(); i ++){
          if(received_Data[i] < 0x10){
            receivedData += "0"; 
          }
          receivedData = receivedData + String(received_Data[i], HEX);
        }
      Serial.println(receivedData);
      parse_sensor_value(receivedData, name);
}

void parse_sensor_value(String recv_data, String name) {

  if(name == "Temp_Mois"){
    float calc_value_Temp;
    float calc_value_Mois;
    String temp = recv_data.substring(6,10);
    String mois = recv_data.substring(10,14);

    float T = strtoul(temp.c_str(), NULL,16);
    float M = strtoul(mois.c_str(), NULL,16);

    memcpy(&calc_value_Temp , &T, sizeof(float));
    memcpy(&calc_value_Mois, &M, sizeof(float));

    sensor_values[1] = calc_value_Temp/100.0;
    sensor_values[0] = calc_value_Mois/100.0;
    
    // Serial.print("Temp value: "); Serial.println(sensor_values[1]);
    // Serial.print("Mois value: "); Serial.println(sensor_values[0]);
  }
  else if(name == "pH" || name == "Conductivity"){
    float calc_value;
    String data = recv_data.substring(6,10);

    float pH_or_Conduct = strtoul(data.c_str(), NULL,16);

    memcpy(&calc_value , &pH_or_Conduct, sizeof(float));
    if(name == "pH"){
      sensor_values[5] = calc_value/100.0;
    // Serial.print("pH value: "); Serial.println(sensor_values[5]);
    }else{
      sensor_values[6] = calc_value;
    // Serial.print("Conductiviy value: "); Serial.println(sensor_values[6]);
    }
  }
  else if (name == "NPK"){
    int calc_value_Nitro;
    int calc_value_Phos;
    int calc_value_Potas;

    String Nitro = recv_data.substring(6,10);
    String Phos = recv_data.substring(10,14);
    String Potas = recv_data.substring(14,18);

    float N = strtoul(Nitro.c_str(), NULL,16);
    float P = strtoul(Phos.c_str(), NULL,16);
    float K = strtoul(Phos.c_str(), NULL,16);
    
    int N_ = (int)N;
    int P_ = (int)P;
    int K_ = (int)K;

    memcpy(&calc_value_Nitro , &N_, sizeof(int));
    memcpy(&calc_value_Phos, &P_, sizeof(int));
    memcpy(&calc_value_Potas, &K_, sizeof(int));
    

    sensor_values[2] = calc_value_Nitro;
    sensor_values[3] = calc_value_Phos;
    sensor_values[4] = calc_value_Potas;
    // Serial.print("Nitroge value: "); Serial.println(sensor_values[1]);
    // Serial.print("Phosphorus value: "); Serial.println(sensor_values[1]);
    // Serial.print("Potassium value: "); Serial.println(sensor_values[1]);
  }
  else{
    if(receivedData == ""){
      Serial.println("[WARNING] No data received.");
    }else{
      Serial.print("[WARNING] Unknown data received.");
    }
  }
} 

void print_sensor_values(){
    // Print the sensor values to the serial monitor
    String units;
    for(int i = 0; i < num_sensor_values; i++){  // Loop through the sensor values
        if(i == 0){
          units = "%";
        }
        else if(i == 1){
          units = "°C";
        }
        else if (i == 2 || i == 3 || i == 4) {
          units = "mg/kg";
        }
        else if (i == 6) {
          units = "μs/cm";
        }
        else{
          units = "";
        }
        // Serial.printf("%s : %.3f %s\n", sensor_names[i].c_str(), sensor_values[i], units.c_str());  // Print the sensor name and value to the serial monitor
        Serial.print(sensor_names[i]); 
        Serial.print(": "); 
        Serial.print(sensor_values[i]); 
        Serial.print("  ");
        Serial.println(units);\
    }
}
