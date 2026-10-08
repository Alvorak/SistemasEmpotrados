#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_timer.h"

// ============================================
// 1. DEFINICION DE PINES
// ============================================

#define LED_BLANCO GPIO_NUM_4
#define LED_AZUL   GPIO_NUM_5
#define PULSADOR   GPIO_NUM_0

// ============================================
// 2. ESTRUCTURA DE DATOS
// ============================================

typedef struct
{
    int numero_pulsaciones;
    int64_t timestamp;
} evento_boton_t;

// ============================================
// 3. VARIABLES GLOBALES
// ============================================

static volatile int estado_led_azul = 0;
static volatile int contador_pulsaciones = 0;

static QueueHandle_t cola_boton = NULL;

// Contador de eventos que no pudieron
// introducirse en la cola.
static volatile uint32_t eventos_perdidos = 0;

// ============================================
// 4. INTERRUPCION DEL BOTON BOOT
// ============================================

static void IRAM_ATTR boton_ISR_handler(void *args)
{
    // Cambiamos el estado del LED azul.
    estado_led_azul = !estado_led_azul;

    gpio_set_level(
        LED_AZUL,
        estado_led_azul
    );

    // Aumentamos el contador.
    contador_pulsaciones++;

    // Preparamos los datos del evento.
    evento_boton_t evento;

    evento.numero_pulsaciones = contador_pulsaciones;
    evento.timestamp = esp_timer_get_time();

    // Enviamos a la cola sin bloquear.
    BaseType_t enviado = xQueueSendFromISR(
        cola_boton,
        &evento,
        NULL
    );

    if (enviado != pdTRUE)
    {
        eventos_perdidos++;
    }
}

// ============================================
// 5. TAREA DEL TERMINAL
// ============================================

static void tarea_terminal(void *param)
{
    evento_boton_t evento;

    printf("[TAREA] Terminal iniciado\n");

    while (1)
    {
        // Esperamos un evento de la cola.
        if (xQueueReceive(
                cola_boton,
                &evento,
                portMAX_DELAY
            ) == pdTRUE)
        {
            printf(
                "[BOOT] Pulsacion: %d | Timestamp: %" PRId64 " us\n",
                evento.numero_pulsaciones,
                evento.timestamp
            );

            printf(
                "[BOOT] Tiempo: %.3f segundos\n",
                evento.timestamp / 1000000.0
            );
        }
    }
}

// ============================================
// 6. PROGRAMA PRINCIPAL
// ============================================

void app_main(void)
{
    printf("\n");
    printf("====================================\n");
    printf("    EJERCICIO 6.1 - ESP32-S3\n");
    printf("====================================\n");

    // ----------------------------------------
    // CONFIGURAR LEDS
    // ----------------------------------------

    ESP_ERROR_CHECK(
        gpio_set_direction(
            LED_BLANCO,
            GPIO_MODE_OUTPUT
        )
    );

    ESP_ERROR_CHECK(
        gpio_set_direction(
            LED_AZUL,
            GPIO_MODE_OUTPUT
        )
    );

    // Estado inicial de los LEDs.
    // Blanco: activo en bajo.
    // Azul: activo en alto.

    ESP_ERROR_CHECK(
        gpio_set_level(LED_BLANCO, 1)
    );

    ESP_ERROR_CHECK(
        gpio_set_level(LED_AZUL, 0)
    );

    printf("[OK] LEDs configurados\n");

    // ----------------------------------------
    // CREAR COLA
    // ----------------------------------------

    cola_boton = xQueueCreate(
        10,
        sizeof(evento_boton_t)
    );

    if (cola_boton == NULL)
    {
        printf("[ERROR] No se pudo crear la cola\n");
        return;
    }

    printf("[OK] Cola creada correctamente\n");

    // ----------------------------------------
    // CREAR TAREA DEL TERMINAL
    // ----------------------------------------

    BaseType_t resultado = xTaskCreate(
        tarea_terminal,
        "terminal",
        4096,
        NULL,
        5,
        NULL
    );

    if (resultado != pdPASS)
    {
        printf("[ERROR] No se pudo crear la tarea\n");
        return;
    }

    printf("[OK] Tarea terminal creada\n");

    // ----------------------------------------
    // CONFIGURAR BOOT
    // ----------------------------------------

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

    printf("[OK] GPIO0 configurado\n");

    // ----------------------------------------
    // INSTALAR SERVICIO DE INTERRUPCIONES
    // ----------------------------------------

    ESP_ERROR_CHECK(
        gpio_install_isr_service(0)
    );

    printf("[OK] Servicio ISR instalado\n");

    // ----------------------------------------
    // REGISTRAR INTERRUPCION
    // ----------------------------------------

    ESP_ERROR_CHECK(
        gpio_isr_handler_add(
            PULSADOR,
            boton_ISR_handler,
            NULL
        )
    );

    printf("[OK] Interrupcion BOOT registrada\n");

    printf("\n");
    printf("Programa listo.\n");
    printf("Pulsa BOOT para generar eventos.\n");
    printf("\n");

    // ----------------------------------------
    // BUCLE PRINCIPAL
    // ----------------------------------------

    while (1)
    {
        // Encender LED blanco.
        // Activo en bajo: 0 = ON.

        gpio_set_level(LED_BLANCO, 0);

        printf("[MAIN] LED blanco encendido\n");

        vTaskDelay(
            pdMS_TO_TICKS(3000)
        );

        // Apagar LED blanco.

        gpio_set_level(LED_BLANCO, 1);

        printf("[MAIN] LED blanco apagado\n");

        vTaskDelay(
            pdMS_TO_TICKS(3000)
        );
    }
}
