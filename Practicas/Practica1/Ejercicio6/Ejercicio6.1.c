#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"

#define LED_BLANCO GPIO_NUM_4
#define LED_AZUL   GPIO_NUM_5
#define PULSADOR   GPIO_NUM_0

typedef struct
{
    int numero_pulsaciones;
    int64_t timestamp;
} evento_boton_t;

static volatile int estado_led_azul = 0;
static volatile int contador_pulsaciones = 0;

static QueueHandle_t cola_boton;


// Interrupción del botón BOOT
static void IRAM_ATTR boton_ISR_handler(void *args)
{
    // Cambiamos inmediatamente el LED azul
    estado_led_azul = !estado_led_azul;
    gpio_set_level(LED_AZUL, estado_led_azul);

    // Incrementamos contador
    contador_pulsaciones++;

    // Creamos evento
    evento_boton_t evento;

    evento.numero_pulsaciones = contador_pulsaciones;
    evento.timestamp = esp_timer_get_time();

    // Enviamos el evento desde la interrupción
    xQueueSendFromISR(cola_boton, &evento, NULL);
}


// Tarea encargada de mostrar mensajes
void tarea_terminal(void *param)
{
    evento_boton_t evento;

    while (1)
    {
        if (xQueueReceive(cola_boton, &evento, portMAX_DELAY))
        {
            printf(
                "BOOT pulsado %d veces - Timestamp: %" PRId64 " us\n",
                evento.numero_pulsaciones,
                evento.timestamp
            );
        }
    }
}


void app_main(void)
{
    printf("Inicio del programa\n");

    // LED blanco GPIO4
    gpio_set_direction(LED_BLANCO, GPIO_MODE_OUTPUT);

    // LED azul GPIO5
    gpio_set_direction(LED_AZUL, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_AZUL, 0);

    // Creamos la cola
    cola_boton = xQueueCreate(10, sizeof(evento_boton_t));

    // Configuración BOOT GPIO0
    gpio_config_t config_boton = {
        .pin_bit_mask = (1ULL << PULSADOR),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    gpio_config(&config_boton);

    // Instalamos interrupción
    gpio_install_isr_service(0);

    gpio_isr_handler_add(
        PULSADOR,
        boton_ISR_handler,
        NULL
    );

    // Tarea que imprime por terminal
    xTaskCreate(
        tarea_terminal,
        "terminal",
        2048,
        NULL,
        5,
        NULL
    );

    // LED blanco sigue cambiando cada 3 segundos
    while (1)
    {
        // Pull-up: 0 = encendido
        gpio_set_level(LED_BLANCO, 0);

        vTaskDelay(pdMS_TO_TICKS(3000));

        gpio_set_level(LED_BLANCO, 1);

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
