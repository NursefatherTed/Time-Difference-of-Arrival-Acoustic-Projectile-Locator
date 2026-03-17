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
