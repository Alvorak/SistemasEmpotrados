#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

// PRACTICA 2 - EJERCICIO 4.1
// PWM 100 Hz - Incremento del 20% con BOOT

// PINES
#define LED_AZUL GPIO_NUM_5
#define PULSADOR GPIO_NUM_0

// CONFIGURACION PWM
#define PWM_FRECUENCIA 100
#define PWM_RESOLUCION LEDC_TIMER_10_BIT

#define PWM_MODO LEDC_LOW_SPEED_MODE
#define PWM_TIMER LEDC_TIMER_0
#define PWM_CANAL LEDC_CHANNEL_0

#define PWM_MAX 1024

// CONFIGURAR PWM
void configurar_pwm(void)
{
    // Configurar el temporizador PWM
    ledc_timer_config_t timer_config = {
        .speed_mode = PWM_MODO,
        .duty_resolution = PWM_RESOLUCION,
        .timer_num = PWM_TIMER,
        .freq_hz = PWM_FRECUENCIA,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(
        ledc_timer_config(&timer_config)
    );

    // Configurar el canal de salida
    ledc_channel_config_t canal_config = {
        .gpio_num = LED_AZUL,
        .speed_mode = PWM_MODO,
        .channel = PWM_CANAL,
        .timer_sel = PWM_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 0,
        .hpoint = 0
    };

    ESP_ERROR_CHECK(
        ledc_channel_config(&canal_config)
    );
}

// CAMBIAR DUTY CYCLE
void cambiar_duty(int porcentaje)
{
    // Convertimos el porcentaje al valor PWM
    uint32_t duty =
        (PWM_MAX * porcentaje) / 100;

    // Establecemos el nuevo duty
    ESP_ERROR_CHECK(
        ledc_set_duty(
            PWM_MODO,
            PWM_CANAL,
            duty
        )
    );

    // Aplicamos el cambio
    ESP_ERROR_CHECK(
        ledc_update_duty(
            PWM_MODO,
            PWM_CANAL
        )
    );

    printf(
        "Duty cycle: %d%% | Valor: %lu\n",
        porcentaje,
        (unsigned long)duty
    );
}

// CONFIGURAR PULSADOR
void configurar_pulsador(void)
{
    gpio_set_direction(
        PULSADOR,
        GPIO_MODE_INPUT
    );

    gpio_set_pull_mode(
        PULSADOR,
        GPIO_PULLUP_ONLY
    );
}

// PROGRAMA PRINCIPAL
void app_main(void)
{
    printf("\n");
    printf("Practica 2 - Ejercicio 4.1\n");
    printf("PWM a 100 Hz en GPIO5\n");

    // Configuramos los perifericos
    configurar_pwm();
    configurar_pulsador();

    // Duty inicial: 0%
    int porcentaje = 0;

    cambiar_duty(porcentaje);

    // Estado anterior del pulsador
    int pulsador_anterior =
        gpio_get_level(PULSADOR);

    // BUCLE PRINCIPAL
    while (1)
    {
        int pulsador_actual =
            gpio_get_level(PULSADOR);

        // Detectamos flanco de bajada
        // Anterior = 1, actual = 0
        if (pulsador_anterior == 1 &&
            pulsador_actual == 0)
        {
            // Antirrebote sencillo
            vTaskDelay(pdMS_TO_TICKS(30));

            // Confirmamos que sigue pulsado
            if (gpio_get_level(PULSADOR) == 0)
            {
                porcentaje += 20;

                // Si supera 100%, volver a 0%
                if (porcentaje > 100)
                {
                    porcentaje = 0;
                }

                cambiar_duty(porcentaje);
            }
        }

        // Guardamos el estado anterior
        pulsador_anterior =
            gpio_get_level(PULSADOR);

        // Pequeno delay
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
