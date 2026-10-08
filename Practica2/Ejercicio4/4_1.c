#include "driver/ledc.h"

// 1. Configurar timer PWM
ledc_timer_config_t timer_pwm = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_10_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 100,
    .clk_cfg = LEDC_AUTO_CLK
};

ESP_ERROR_CHECK(ledc_timer_config(&timer_pwm));

// 2. Configurar canal GPIO5
ledc_channel_config_t canal = {
    .gpio_num = GPIO_NUM_5,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .duty = 0,
    .hpoint = 0
};

ESP_ERROR_CHECK(ledc_channel_config(&canal));

// 3. Actualizar duty
// porcentaje: 0, 20, 40, 60, 80, 100
uint32_t duty = (porcentaje * 1024) / 100;

ESP_ERROR_CHECK(
    ledc_set_duty(
        LEDC_LOW_SPEED_MODE,
        LEDC_CHANNEL_0,
        duty
    )
);

ESP_ERROR_CHECK(
    ledc_update_duty(
        LEDC_LOW_SPEED_MODE,
        LEDC_CHANNEL_0
    )
);
