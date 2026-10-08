#include "driver/gptimer.h"

// Configuración base de un timer
gptimer_handle_t timer = NULL;

gptimer_config_t config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1000000
};

ESP_ERROR_CHECK(
    gptimer_new_timer(&config, &timer)
);

// Ejemplo de alarma periódica de 500 ms
gptimer_alarm_config_t alarma = {
    .reload_count = 0,
    .alarm_count = 500000,
    .flags.auto_reload_on_alarm = true
};

ESP_ERROR_CHECK(
    gptimer_set_alarm_action(timer, &alarma)
);
