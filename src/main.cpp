#include "BajaCan.h"
#include "Arduino.h"


// Default for ESP32
gpio_num_t CAN_TX = gpio_num_t::GPIO_NUM_5;
gpio_num_t CAN_RX = gpio_num_t::GPIO_NUM_4;


BajaCan CANBus(CAN_TX, CAN_RX);


void setup() {
    Serial.begin(115200);
    esp_err_t ret = CANBus.begin();
    if (ret == ESP_OK) {
        Serial.println("CAN bus initialized successfully.");
    } else {
        Serial.printf("Failed to initialize CAN bus. Error code: %d\n", ret);
    }
}

void loop() {
    // send a CAN frame every second

    CanMessage frame(0x123, 42.0f); // Example CAN message with ID 0x123 and float data 42.0
    esp_err_t ret = CANBus.writeMessage(frame, 1000);

    if (ret == ESP_OK) {
        Serial.println("CAN frame sent successfully.");
    } else {
        Serial.printf("Failed to send CAN frame. Error code: %d\n", ret);
    }
    delay(1000);
}