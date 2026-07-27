#include <stdint.h>             // Defines strict integer sizes (int32_t)
#include <stddef.h>             // Defines size_t for memory allocation
#include <math.h>               // For the asin() and atan2() trigonometry
#include <stdbool.h>            // Standard true/false boolean logic
#include <stdio.h>              // For printing to the serial monitor
#include <driver/i2s.h>         // The Espressif hardware drivers for the microphones
#include <freertos/FreeRTOS.h>  // The real-time operating system 
#include <freertos/task.h>      // Allows us to pin tasks to specific CPU cores

// --- HARDWARE CONSTANTS ---
#define SAMPLE_RATE 16000 
#define READ_LEN 1024           // Read 1024 bytes of audio at a time
#define THRESHOLD 15000         // The amplitude spike required to trigger the math
#define SOUND_SPEED 347.0       // Speed of sound in warm air (m/s)
#define MIC_DISTANCE 0.08       // Physical distance between microphones in meters

// --- I2S PINOUT ROUTING ---
#define I2S0_WS 15
#define I2S0_SD 32
#define I2S0_SCK 14

#define I2S1_WS 25
#define I2S1_SD 33
#define I2S1_SCK 26

// --- MEMORY ARRAYS ---
int32_t raw_i2s0[READ_LEN / 4]; 
int32_t raw_i2s1[READ_LEN / 4]; 
int32_t mic_A[READ_LEN / 8];
int32_t mic_B[READ_LEN / 8];
int32_t mic_C[READ_LEN / 8];
int32_t mic_D[READ_LEN / 8];

void init_microphone_arrays() {
    i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), 
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, 
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, 
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, 
        .dma_buf_count = 8, 
        .dma_buf_len = 512, 
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config_0 = {
        .bck_io_num = I2S0_SCK,
        .ws_io_num = I2S0_WS,
        .data_out_num = I2S_PIN_NO_CHANGE, 
        .data_in_num = I2S0_SD 
    };

    i2s_pin_config_t pin_config_1 = {
        .bck_io_num = I2S1_SCK,
        .ws_io_num = I2S1_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S1_SD
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config_0);

    i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_1, &pin_config_1);
}

void grab_and_split_audio_frame() {
    size_t bytes_read_0 = 0;
    size_t bytes_read_1 = 0;

    // Halt the processor until 1024 bytes of audio are actually ready
    i2s_read(I2S_NUM_0, &raw_i2s0, READ_LEN, &bytes_read_0, portMAX_DELAY);
    i2s_read(I2S_NUM_1, &raw_i2s1, READ_LEN, &bytes_read_1, portMAX_DELAY);

    int sample_count = bytes_read_0 / 4; 

    // Unweave the Left and Right channels into separate arrays
    for (int i = 0; i < sample_count; i += 2) {
        mic_A[i/2] = raw_i2s0[i] >> 8;       
        mic_B[i/2] = raw_i2s0[i + 1] >> 8;   
        mic_C[i/2] = raw_i2s1[i] >> 8;       
        mic_D[i/2] = raw_i2s1[i + 1] >> 8;   
    }
}

bool detect_transient(int32_t* primary_mic, int len) {
    for (int i = 0; i < len; i++) {
        if (abs(primary_mic[i]) > THRESHOLD) return true; 
    }
    return false;
}

int calculate_sample_delay(int32_t* sig1, int32_t* sig2, int len) {
    int best_lag = 0;
    int64_t max_correlation = 0;
    int search_window = 20; 

    for (int lag = -search_window; lag <= search_window; lag++) {
        int64_t current_correlation = 0;
        for (int i = 0; i < len; i++) {
            int j = i + lag;
            if (j >= 0 && j < len) {
                current_correlation += (int64_t)sig1[i] * (int64_t)sig2[j]; 
            }
        }
        if (current_correlation > max_correlation) {
            max_correlation = current_correlation;
            best_lag = lag;
        }
    }
    return best_lag;
}

void process_2d_localization() {
    if (detect_transient(mic_A, READ_LEN/8)) {
        
        int delay_x = calculate_sample_delay(mic_A, mic_B, READ_LEN/8);
        int delay_y = calculate_sample_delay(mic_C, mic_D, READ_LEN/8);
        
        if (delay_x == 0 && delay_y == 0) return;

        double angle_radians = atan2((double)delay_y, (double)delay_x);
        double angle_degrees = angle_radians * (180.0 / M_PI);
        
        if (angle_degrees < 0) angle_degrees += 360.0;
        
        printf("ACOUSTIC CONTACT. Bearing: %.2f degrees\n", angle_degrees);
        vTaskDelay(pdMS_TO_TICKS(500)); // Half-second cooldown
    }
}

void acoustic_radar_task(void *pvParameters) {
    init_microphone_arrays();
    printf("System Armed. Listening...\n");

    while(1) {
        grab_and_split_audio_frame();
        process_2d_localization();
        
        // Feed the watchdog timer so the board doesn't panic and reboot
        vTaskDelay(pdMS_TO_TICKS(1)); 
    }
}

void app_main() {
    // Pin the heavy math task specifically to Core 1
    xTaskCreatePinnedToCore(
        acoustic_radar_task, 
        "Radar_Task", 
        8192,  // Allocate plenty of stack memory
        NULL, 
        5,     // Set a high execution priority
        NULL, 
        1      // Lock to Core 1
    );
}
