#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"

// GPIO7 = ADC1_CHANNEL_6
adc_oneshot_unit_handle_t adc_handle;

adc_oneshot_unit_init_cfg_t init = {
    .unit_id = ADC_UNIT_1
};

ESP_ERROR_CHECK(
    adc_oneshot_new_unit(&init, &adc_handle)
);

adc_oneshot_chan_cfg_t config = {
    .atten = ADC_ATTEN_DB_12,
    .bitwidth = ADC_BITWIDTH_DEFAULT
};

ESP_ERROR_CHECK(
    adc_oneshot_config_channel(
        adc_handle, ADC_CHANNEL_6, &config
    )
);

// Dentro del bucle:
int muestra1, muestra2;

ESP_ERROR_CHECK(
    adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &muestra1)
);
ESP_ERROR_CHECK(
    adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &muestra2)
);

float promedio = (muestra1 + muestra2) / 2.0f;
int64_t tiempo_us = esp_timer_get_time();
