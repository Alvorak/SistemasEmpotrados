
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_attr.h"

// PRACTICA 2 - EJERCICIO 3.3
#define PULSADOR GPIO_NUM_0

// Antirrebote de 50 milisegundos
#define ANTIRREBOTE_US 50000

// Cola para comunicar ISR y tarea
static QueueHandle_t cola_boton = NULL;


// INTERRUPCION DEL BOTON
static void IRAM_ATTR boton_ISR(void *args)
{
    // Enviamos un evento a la tarea.
    // No imprimimos dentro de la ISR.

    uint8_t evento = 1;

    BaseType_t tarea_despertada = pdFALSE;

    xQueueSendFromISR(
        cola_boton,
        &evento,
        &tarea_despertada
    );

    if (tarea_despertada == pdTRUE)
    {
        portYIELD_FROM_ISR();
    }
}


// TAREA PARA MEDIR LOS INTERVALOS

static void tarea_pulsador(void *param)
{
    uint8_t evento;

    int64_t tiempo_anterior = 0;
    bool primera_pulsacion = false;

    int contador = 0;

    while (1)
    {
        // Esperamos un evento de BOOT.
        if (xQueueReceive(
                cola_boton,
                &evento,
                portMAX_DELAY) == pdTRUE)
        {
            // Tiempo actual en microsegundos.
            int64_t tiempo_actual =
                esp_timer_get_time();

            // Evitar rebotes del pulsador.
            if (primera_pulsacion &&
                (tiempo_actual - tiempo_anterior)
                    < ANTIRREBOTE_US)
            {
                continue;
            }

            contador++;

            // Primera pulsacion:
            // solo guardamos el tiempo.
            if (!primera_pulsacion)
            {
                tiempo_anterior = tiempo_actual;

                primera_pulsacion = true;

                printf(
                    "Primera pulsacion registrada\n"
                );
            }
            else
            {
                // Tiempo entre pulsaciones
                // en microsegundos.
                int64_t intervalo_us =
                    tiempo_actual - tiempo_anterior;

                // Conversion a milisegundos.
                double intervalo_ms =
                    intervalo_us / 1000.0;

                printf(
                    "Pulsacion %d | Intervalo: %.2f ms\n",
                    contador,
                    intervalo_ms
                );

                // Actualizamos el tiempo anterior.
                tiempo_anterior = tiempo_actual;
            }
        }
    }
}


// PROGRAMA PRINCIPAL
void app_main(void)
{
    printf("\n");
    printf("Practica 2 - Ejercicio 3.3\n");
    printf("Medicion entre pulsaciones BOOT\n");

    // CREAR COLA
    cola_boton = xQueueCreate(
        10,
        sizeof(uint8_t)
    );

    if (cola_boton == NULL)
    {
        printf("ERROR creando la cola\n");
        return;
    }

    // CREAR TAREA
    BaseType_t resultado = xTaskCreate(
        tarea_pulsador,
        "tarea_pulsador",
        4096,
        NULL,
        5,
        NULL
    );

    if (resultado != pdPASS)
    {
        printf("ERROR creando la tarea\n");
        return;
    }

    // CONFIGURAR BOOT
    gpio_config_t config_boton = {
        .pin_bit_mask = (1ULL << PULSADOR),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    ESP_ERROR_CHECK(
        gpio_config(&config_boton)
    );

    // CONFIGURAR INTERRUPCIONES
    ESP_ERROR_CHECK(
        gpio_install_isr_service(0)
    );

    ESP_ERROR_CHECK(
        gpio_isr_handler_add(
            PULSADOR,
            boton_ISR,
            NULL
        )
    );

    printf("Sistema preparado\n");
    printf("Pulsa BOOT dos veces\n");

    // BUCLE PRINCIPAL
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
