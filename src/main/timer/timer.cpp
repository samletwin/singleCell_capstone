#include "timer.h"
#include "Arduino.h"

// Function to create and start a timer
esp_timer_handle_t create_timer(int timer_num, int64_t interval_us, esp_timer_cb_t callback, bool continuous) {
    esp_timer_handle_t timer_handle;

    // Define timer arguments
    esp_timer_create_args_t timer_args = {
        .callback = callback,
        .arg = (void*)timer_num,
        .name = "esp32_timer"
    };

    // Create the timer
    esp_err_t err = esp_timer_create(&timer_args, &timer_handle);
    if (err != ESP_OK) {
        // Handle error
        Serial.printf("Failed to create timer %d\n", timer_num);
        return nullptr;
    }

    // Start the timer
    if (continuous) {
        err = esp_timer_start_periodic(timer_handle, interval_us);
    } else {
        err = esp_timer_start_once(timer_handle, interval_us);
    }

    if (err != ESP_OK) {
        // Handle error
        Serial.printf("Failed to start timer %d\n", timer_num);
        esp_timer_delete(timer_handle);
        return nullptr;
    }

    return timer_handle;
}

// Function to stop and delete a timer
void delete_timer(esp_timer_handle_t timer_handle) {
    if (timer_handle != nullptr) {
        esp_timer_stop(timer_handle);
        esp_timer_delete(timer_handle);
    }
}
