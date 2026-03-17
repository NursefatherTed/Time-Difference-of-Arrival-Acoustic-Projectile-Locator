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
