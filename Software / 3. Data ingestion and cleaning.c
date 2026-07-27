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
