#include <ESP32-TWAI-CAN.hpp>

// Custom error codes for Baja CAN operations
// ESP-IDF custom errors start at 0x6000 for user applications
#define ESP_ERR_BAJA_CAN_BASE           0x6000
#define ESP_ERR_BAJA_CAN_TX_FAILED      (ESP_ERR_BAJA_CAN_BASE + 0x01)
#define ESP_ERR_BAJA_CAN_RX_TIMEOUT     (ESP_ERR_BAJA_CAN_BASE + 0x02)
#define ESP_ERR_BAJA_CAN_INVALID_FRAME  (ESP_ERR_BAJA_CAN_BASE + 0x03)
#define ESP_ERR_BAJA_CAN_BUS_OFF        (ESP_ERR_BAJA_CAN_BASE + 0x04)
#define ESP_ERR_BAJA_CAN_ARB_LOST       (ESP_ERR_BAJA_CAN_BASE + 0x05)

// Default for ESP32
gpio_num_t CAN_TX = gpio_num_t::GPIO_NUM_5;
gpio_num_t CAN_RX = gpio_num_t::GPIO_NUM_4;


/**
 * @brief Convert Baja CAN error codes to human-readable names
 * Falls back to standard esp_err_to_name for non-Baja errors
 * 
 * @param error The error code to convert
 * @return const char* Error name string
 */
const char* baja_err_to_name(esp_err_t error) {
    switch(error) {
        case ESP_ERR_BAJA_CAN_TX_FAILED:     return "ESP_ERR_BAJA_CAN_TX_FAILED";
        case ESP_ERR_BAJA_CAN_RX_TIMEOUT:    return "ESP_ERR_BAJA_CAN_RX_TIMEOUT";
        case ESP_ERR_BAJA_CAN_INVALID_FRAME: return "ESP_ERR_BAJA_CAN_INVALID_FRAME";
        case ESP_ERR_BAJA_CAN_BUS_OFF:       return "ESP_ERR_BAJA_CAN_BUS_OFF";
        case ESP_ERR_BAJA_CAN_ARB_LOST:      return "ESP_ERR_BAJA_CAN_ARB_LOST";
        default:                             return esp_err_to_name(error);
    }
}


/**
 * @brief Installs twai driver and starts it. Use default config.
 * 
 * @param txPin
 * @param rxPin
 * 
 * @return esp_err_t from twai_driver_install and twai_start
 */
esp_err_t initBajaCan(gpio_num_t txPin, gpio_num_t rxPin) {
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        txPin,
        rxPin,
        TWAI_MODE_NORMAL
    );
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    esp_err_t ret = twai_driver_install(&g_config, &t_config, &f_config);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = twai_start();
    return ret;
}

esp_err_t bajaCanReadFrame(twai_message_t* frame, uint32_t timeoutMs) {
    if (!frame) {
        return ESP_ERR_BAJA_CAN_INVALID_FRAME;
    }
    esp_err_t ret = twai_receive(frame, pdMS_TO_TICKS(timeoutMs));
    if (ret == ESP_ERR_TIMEOUT) {
        return ESP_ERR_BAJA_CAN_RX_TIMEOUT;
    } else if (ret != ESP_OK) {
        return ret; // Propagate other errors
    }
    return ESP_OK;
}

esp_err_t bajaCanWriteFrame(const twai_message_t* frame, uint32_t timeoutMs) {
    if (!frame) {
        return ESP_ERR_BAJA_CAN_INVALID_FRAME;
    }
    esp_err_t ret = twai_transmit(frame, pdMS_TO_TICKS(timeoutMs));
    if (ret != ESP_OK) {
        return ESP_ERR_BAJA_CAN_TX_FAILED;
    }
    return ESP_OK;
}


void setup() {
    // Setup serial for debbuging.
    Serial.begin(115200);

    // Initialize CAN bus at default pins
    esp_err_t ret = initBajaCan(CAN_TX, CAN_RX);
    if (ret == ESP_OK) {
        Serial.println("TWAI-CAN initialized");
    } else {
        Serial.print("Failed to initialize TWAI-CAN with error: "); 
        Serial.println(baja_err_to_name(ret));
    }
}

void loop() {
    twai_message_t frame;
    esp_err_t ret = bajaCanReadFrame(&frame, 1000); // 1 second timeout
    if (ret == ESP_OK) {
        Serial.print("Received CAN frame with ID: ");
        Serial.println(frame.identifier, HEX);
    } else if (ret == ESP_ERR_BAJA_CAN_RX_TIMEOUT) {
        Serial.println("No CAN frame received within timeout period.");
    } else {
        Serial.print("Error receiving CAN frame: ");
        Serial.println(baja_err_to_name(ret));
    }

    // Example of sending a CAN frame
    twai_message_t txFrame;
    txFrame.identifier = 0x123;
    txFrame.data_length_code = 2;
    txFrame.data[0] = 0xAB;
    txFrame.data[1] = 0xCD;

    ret = bajaCanWriteFrame(&txFrame, 100); // 100 ms timeout
    if (ret == ESP_OK) {
        Serial.println("CAN frame sent successfully.");
    } else {
        Serial.print("Error sending CAN frame: ");
        Serial.println(baja_err_to_name(ret));
    }

    delay(2000); // Wait for 2 seconds before next loop
   
}