#include "BajaCan.h"
#include "CanDatabase.h"
#include "Arduino.h"


// Default for ESP32
gpio_num_t CAN_TX = gpio_num_t::GPIO_NUM_5;
gpio_num_t CAN_RX = gpio_num_t::GPIO_NUM_4;


BajaCan CANBus(CAN_TX, CAN_RX);


void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF version is: " + String(esp_get_idf_version())); // v4.4.4, see docs here https://docs.espressif.com/projects/esp-idf/en/v4.4.4/esp32/api-reference/peripherals/twai.html
    
    esp_err_t ret = CANBus.begin();
    if (ret == ESP_OK) {
        Serial.println("CAN bus initialized successfully.");
    } else {
        Serial.printf("Failed to initialize CAN bus. Error code: %d\n", ret);
    }
}

void loop() {
    // // send a CAN frame every second
    // CanMessage frame(ENGINE_RPM.id, 42.0f); // Example CAN message with ID 0x123 and float data 42.0
    // esp_err_t ret = CANBus.writeMessage(frame, 1000);

    // if (ret == ESP_OK) {
    //     Serial.println("CAN frame sent successfully.");
    // } else {
    //     Serial.printf("Failed to send CAN frame. Error code: %d\n", ret);
    // }
    // delay(1000);
    // CanMessage intFrame(INT_DATA.id, 12345); // Example CAN message with ID 0x123 and int data 12345
    // ret = CANBus.writeMessage(intFrame, 1000);

    // CanMessage int8Frame(INT8_DATA.id, (uint8_t) 55); 
    // ret = CANBus.writeMessage(int8Frame, 1000);


    // recieve a CAN frame every second
    CanMessage receivedFrame; // Placeholder for received message
    esp_err_t ret = CANBus.readMessage(receivedFrame, 1000);
    
    if (ret == ESP_OK) {
        if (receivedFrame.getId() == ENGINE_RPM.id) {
            Serial.printf("Received ENGINE_RPM frame: %.2f\n", receivedFrame.getFloat());
        } else if (receivedFrame.getId() == INT_DATA.id) {
            Serial.printf("Received INT_DATA frame: %d\n", receivedFrame.getInt32());
        } else if (receivedFrame.getId() == INT8_DATA.id) {
            Serial.printf("Received INT8_DATA frame: %d\n", receivedFrame.getUInt8());
        } else {
            Serial.printf("Received unknown CAN frame with ID: 0x%X\n", receivedFrame.getId());
        }
    } else if (ret == ESP_ERR_TIMEOUT) {
        Serial.println("No CAN frame received within timeout.");
    } else {
        Serial.printf("Failed to read CAN frame. Error code: %d\n", ret);
    }

    // delay(1000);
}