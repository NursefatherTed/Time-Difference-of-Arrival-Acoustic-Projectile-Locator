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
