#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_adc/adc_oneshot.h"

// PRACTICA 2 - EJERCICIO 2.1
// Potenciometro: extremos a 3V3 y GND; cursor al GPIO7.
// GPIO7 en ESP32-S3 corresponde a ADC1, canal 6.

#define ADC_UNIDAD          ADC_UNIT_1
#define ADC_CANAL           ADC_CHANNEL_6
#define PERIODO_LECTURA_MS  100

void app_main(void)
{
    printf("\nPractica 2 - Ejercicio 2.1: ADC\n");
    printf("GPIO7 / ADC1_CHANNEL_6, 12 bits, dos muestras cada 100 ms\n");

    // 1. Crear unidad ADC1 en modo one-shot.
    adc_oneshot_unit_handle_t adc_handle = NULL;
    adc_oneshot_unit_init_cfg_t unidad_config = {
        .unit_id = ADC_UNIDAD,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unidad_config, &adc_handle));

    // 2. Configurar canal: atenuacion pedida en el enunciado.
    // ADC_BITWIDTH_DEFAULT corresponde a 12 bits en ESP32-S3.
    adc_oneshot_chan_cfg_t canal_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CANAL, &canal_config));

    // 3. Obtener dos lecturas y publicar un promedio cada 100 ms.
    // vTaskDelayUntil evita acumular deriva del periodo de tarea.
    const TickType_t periodo_ticks = pdMS_TO_TICKS(PERIODO_LECTURA_MS);
    TickType_t proxima_lectura = xTaskGetTickCount();

    while (1) {
        int muestra1 = 0;
        int muestra2 = 0;

        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CANAL, &muestra1));
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CANAL, &muestra2));

        // 2.0f fuerza division en coma flotante.
        float promedio = (muestra1 + muestra2) / 2.0f;
        int64_t timestamp_us = esp_timer_get_time();

        printf("t=%" PRId64 " us | ADC: %d, %d | media: %.1f\n",
               timestamp_us, muestra1, muestra2, promedio);

        vTaskDelayUntil(&proxima_lectura, periodo_ticks);
    }

    // El programa es continuo: si algun dia sale del bucle,
    // se podria liberar adc_handle con adc_oneshot_del_unit().
}
