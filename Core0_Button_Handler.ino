// button handling task

void ButtonTASK_CORE0 (void *pvParameters){
  
  //Creating the watchdog for the Core 0
  #if defined ESP32
    esp_task_wdt_deinit();                  // ensure a watchdog is not already configured
    #if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR == 3  
      // v3 board manager detected
      // Create and initialize the watchdog timer(WDT) configuration structure
        esp_task_wdt_config_t wdt_config = {
            .timeout_ms = WDT_TIMEOUT * 1000,              // Convert seconds to milliseconds
            .idle_core_mask = (1 << 0) | (1 << 1),         // Monitor core 1 & Core 2 only
            .trigger_panic = false                         // Enable panic
        };
      // Initialize the WDT with the configuration structure
        esp_task_wdt_init(&wdt_config);       // Pass the pointer to the configuration structure
        esp_task_wdt_add(NULL);               // Add current thread to WDT watch    
        esp_task_wdt_reset();                 // reset Watchdog timer
    #else
      // pre v3 board manager assumed
        esp_task_wdt_init(WDT_TIMEOUT, false);                      //enable panic so ESP32 restarts
        esp_task_wdt_add(NULL);                                    //add current thread to WDT watch   
        esp_task_wdt_reset();
    #endif
  #endif
  // Run Loop Core 0
  long LoopTimer = millis();  // timer to check loop runtime
  while(true){
    button.tick();  // Check the button state
    vTaskDelay(pdMS_TO_TICKS(100)); // Add a small delay to yield control and prevent blocking
    if(millis() - LoopTimer >= (WDT_TIMEOUT -5)*1000){
      esp_task_wdt_reset();
      LoopTimer = millis();
    }
  }
}
