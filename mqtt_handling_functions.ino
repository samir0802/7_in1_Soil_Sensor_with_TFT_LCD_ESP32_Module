//------------------------------------MQTT Functions-------------------------------------
// connect to the MQTT broker
int mqtt_conn_err_count = 0;

// ------------------- MQTT communication init, data receiving processing and data publishing ---------------------------------//
bool connectToMQTT(){ 

    // Ensure any previous connection is disconnected safely
    if (mqtt.connected()) {
        Serial.println("Old MQTT client was connected, disconnecting...");
        mqtt.disconnect();
        delay(1000); // Small delay to ensure disconnection
        Serial.println("Old MQTT client disconnected");
    }

    if (WiFi.status() == WL_CONNECTED){
        Serial.println("[Info] Connecting to MQTT.");
    } else {
        Serial.println("[Warning] No Connection to MQTT, WiFi not connected...");
        return false;
    }

    mqtt.setServer(MQTT_broker_ID.c_str(), MQTT_broker_port);
    mqtt.setCallback(mqttCallback);
    mqtt.setKeepAlive(60);
    mqtt.setSocketTimeout(15); 

    Serial.println("."); 
    String online_stat; 
    online_stat = "Soil Monitor:" + mqttClientId + ", Online..";
    mqttLastWillMessage = "Device: " + mqttClientId + mqttLastWillMessage; 

    const char* mqttID = mqttClientId.c_str();
    const char* mqttLastWillMESSAGE = mqttLastWillMessage.c_str();
    const char* sub_topic = mqttSubscribeTopic.c_str();
    const char* pub_topic = mqttPublishTopic.c_str();
    mqttLastWillTopic = pub_topic;

    Serial.print("[Info] Connecting to MQTT Broker....\n");
    Serial.print("[Info] MQTT Server IP: "); Serial.println(MQTT_broker_ID);
    Serial.print("[Info] MQTT Server Port: "); Serial.println(MQTT_broker_port);
    Serial.print("[Info] MQTT Client ID: "); Serial.println(mqttClientId);

    if (!mqtt.connect(mqttID, mqttUsername, mqttPassword, mqttLastWillTopic, /*QOS1 */0, /*lastWill Retain*/true, mqttLastWillMESSAGE) ) {
        Serial.print("[Warning] MQTT connection failed, MQTT State: ");
        Serial.println(mqtt.state());
        mqtt_conn_err_count++;
        return false;         
    }

    mqtt_conn_err_count = 0; //reset counter
    online_stat = String(mqttClientId) + "Online.."; 
    Serial.println("[Info] Connected to broker.");

    publishToMQTT(mqttPublishTopic, online_stat);
    mqtt.subscribe(sub_topic);
    return true; 
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("[Info] Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}
void publishToMQTT(String topic, String message) {
    message = mqttClientId + ", " + message;
    const char* pubtopic = topic.c_str();
    const char* pubmessage = message.c_str();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi Connected!! Trying to publish to MQTT");
        if (mqtt.publish(pubtopic, pubmessage, true)) {
            Serial.println("\n[Info] WiFi MQTT Message Published successfully");
        } else {
            Serial.println("\n[Warning] WiFi MQTT Message publication failed");
        }
    } else {
        Serial.println("[Warning] Publishing Failed, No WiFi Connection...");
    }
}