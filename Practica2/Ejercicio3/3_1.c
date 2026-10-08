
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_attr.h"
#include "esp_err.h"

// PRACTICA 2 - EJERCICIO 3.1
// GPIO4 -> Senal de 1 Hz
// GPIO5 -> Senal de 120 Hz

#define LED_BLANCO GPIO_NUM_4
#define LED_AZUL   GPIO_NUM_5

// Resolucion: 1 MHz = 1 tick por microsegundo
#define RESOLUCION_TIMER 1000000

// Medio periodo para 1 Hz
#define ALARMA_1HZ 500000

// Medio periodo para 120 Hz
#define ALARMA_120HZ 4167

// Estados de las salidas
static volatile int estado_gpio4 = 0;
static volatile int estado_gpio5 = 0;


// CALLBACK TIMER GPIO4 - 1 Hz
static bool IRAM_ATTR callback_1hz(
    gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *event,
    void *user_data)
{
    // Invertimos el estado del GPIO4
    estado_gpio4 = !estado_gpio4;

    gpio_set_level(LED_BLANCO, estado_gpio4);

    // No hemos despertado ninguna tarea
    return false;
}


// CALLBACK TIMER GPIO5 - 120 Hz
static bool IRAM_ATTR callback_120hz(
    gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *event,
    void *user_data)
{
    // Invertimos el estado del GPIO5
    estado_gpio5 = !estado_gpio5;

    gpio_set_level(LED_AZUL, estado_gpio5);

    return false;
}


// PROGRAMA PRINCIPAL
void app_main(void)
{
    printf("Practica 2 - Ejercicio 3.1\n");
    printf("GPIO4 = 1 Hz\n");
    printf("GPIO5 = 120 Hz\n");

    // CONFIGURACION DE GPIO
    gpio_set_direction(
        LED_BLANCO,
        GPIO_MODE_OUTPUT
    );

    gpio_set_direction(
        LED_AZUL,
        GPIO_MODE_OUTPUT
    );

    gpio_set_level(LED_BLANCO, 0);
    gpio_set_level(LED_AZUL, 0);


    // TIMER 1 - GPIO4 - 1 Hz
    gptimer_handle_t timer1 = NULL;

    gptimer_config_t config1 = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = RESOLUCION_TIMER
    };

    ESP_ERROR_CHECK(
        gptimer_new_timer(&config1, &timer1)
    );

    // Alarma cada 500000 us
    gptimer_alarm_config_t alarma1 = {
        .reload_count = 0,
        .alarm_count = ALARMA_1HZ,
        .flags.auto_reload_on_alarm = true
    };

    ESP_ERROR_CHECK(
        gptimer_set_alarm_action(
            timer1,
            &alarma1
        )
    );

    // Registrar callback
    gptimer_event_callbacks_t callbacks1 = {
        .on_alarm = callback_1hz
    };

    ESP_ERROR_CHECK(
        gptimer_register_event_callbacks(
            timer1,
            &callbacks1,
            NULL
        )
    );


    // TIMER 2 - GPIO5 - 120 Hz
    gptimer_handle_t timer2 = NULL;

    gptimer_config_t config2 = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = RESOLUCION_TIMER
    };

    ESP_ERROR_CHECK(
        gptimer_new_timer(&config2, &timer2)
    );

    // Alarma cada 4167 us
    gptimer_alarm_config_t alarma2 = {
        .reload_count = 0,
        .alarm_count = ALARMA_120HZ,
        .flags.auto_reload_on_alarm = true
    };

    ESP_ERROR_CHECK(
        gptimer_set_alarm_action(
            timer2,
            &alarma2
        )
    );

    // Registrar callback
    gptimer_event_callbacks_t callbacks2 = {
        .on_alarm = callback_120hz
    };

    ESP_ERROR_CHECK(
        gptimer_register_event_callbacks(
            timer2,
            &callbacks2,
            NULL
        )
    );


    // HABILITAR LOS TEMPORIZADORES

    ESP_ERROR_CHECK(gptimer_enable(timer1));
    ESP_ERROR_CHECK(gptimer_enable(timer2));


    // INICIAR LOS TEMPORIZADORES

    ESP_ERROR_CHECK(gptimer_start(timer1));
    ESP_ERROR_CHECK(gptimer_start(timer2));

    printf("Temporizadores iniciados\n");


    // BUCLE PRINCIPAL
    // No generamos ninguna senal aqui.
    // Todo lo hacen los callbacks.

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
