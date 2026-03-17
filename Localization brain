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
