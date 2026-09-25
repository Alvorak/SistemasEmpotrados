#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

/*
 * En la práctica original:
 * GPIO4 = LED rojo
 * GPIO5 = LED verde
 *
 * En mi montaje:
 * GPIO4 = LED blanco
 * GPIO5 = LED azul
 */
#define LED_BLANCO GPIO_NUM_4
#define LED_AZUL   GPIO_NUM_5
#define PULSADOR   GPIO_NUM_0


void app_main(void)
{
    printf("Ejercicio 4.4\n");

    /*
     * GPIO4 y GPIO5 son salidas digitales.
     */
    gpio_set_direction(LED_BLANCO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_AZUL, GPIO_MODE_OUTPUT);

    /*
     * El botón BOOT está conectado al GPIO0.
     *
     * Lo configuramos como entrada con pull-up:
     *
     * Sin pulsar -> 1
     * Pulsado    -> 0
     */
    gpio_set_direction(PULSADOR, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PULSADOR, GPIO_PULLUP_ONLY);

    while (1)
    {
        /*
         * LED blanco conectado en pull-up.
         *
         * GPIO = 0 -> LED encendido
         */
        gpio_set_level(LED_BLANCO, 0);

        /*
         * Leemos el estado de BOOT.
         */
        int pulsador = gpio_get_level(PULSADOR);

        /*
         * Si BOOT está pulsado:
         *
         * pulsador = 0
         *
         * Encendemos el LED azul.
         */
        if (pulsador == 0)
        {
            gpio_set_level(LED_AZUL, 1);
        }
        else
        {
            gpio_set_level(LED_AZUL, 0);
        }

        /*
         * Esperamos 100 ms.
         */
        vTaskDelay(pdMS_TO_TICKS(100));


        /*
         * Apagamos LED blanco.
         *
         * GPIO = 1 -> LED apagado
         */
        gpio_set_level(LED_BLANCO, 1);

        /*
         * Volvemos a consultar BOOT.
         */
        pulsador = gpio_get_level(PULSADOR);

        if (pulsador == 0)
        {
            gpio_set_level(LED_AZUL, 1);
        }
        else
        {
            gpio_set_level(LED_AZUL, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
