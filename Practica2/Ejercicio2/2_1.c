
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "esp_adc/adc_oneshot.h"

// PRACTICA 2 - EJERCICIO 2.1
//
// Potenciometro:
// Extremo 1 -> 3.3 V
// Extremo 2 -> GPIO7
// Extremo 3 -> GND
//
// GPIO7 -> ADC1_CHANNEL_6

#define ADC_UNIDAD ADC_UNIT_1
#define ADC_CANAL  ADC_CHANNEL_6

void app_main(void)
{
    printf("Practica 2 - Ejercicio 2.1\n");

    // CONFIGURAR ADC
    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIDAD
    };

    adc_oneshot_new_unit(
        &init_config,
        &adc_handle
    );

    // Configurar resolucion y atenuacion

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT
    };

    adc_oneshot_config_channel(
        adc_handle,
        ADC_CANAL,
        &config
    );

    // VARIABLES

    int muestra1 = 0;
    int muestra2 = 0;
    float promedio = 0;

    // Tomamos la primera muestra
    adc_oneshot_read(
        adc_handle,
        ADC_CANAL,
        &muestra1
    );

    // BUCLE PRINCIPAL

    while (1)
    {
        // Esperamos 100 milisegundos
        vTaskDelay(pdMS_TO_TICKS(100));

        // Tomamos la segunda muestra
        adc_oneshot_read(
            adc_handle,
            ADC_CANAL,
            &muestra2
        );

        // Calculamos la media
        promedio = (muestra1 + muestra2) / 2.0f;

        // Timestamp en microsegundos
        int64_t tiempo = esp_timer_get_time();

        // Mostramos los resultados
        printf(
            "Muestra 1: %d | Muestra 2: %d | Media: %.1f | Tiempo: %" PRId64 " us\n",
            muestra1,
            muestra2,
            promedio,
            tiempo
        );

        // La segunda muestra se convierte
        // en la primera del siguiente ciclo
        muestra1 = muestra2;
    }
}
