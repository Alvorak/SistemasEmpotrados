#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_BLANCO GPIO_NUM_4
#define LED_AZUL   GPIO_NUM_5
#define PULSADOR   GPIO_NUM_0


void app_main(void)
{
    printf("Ejercicio 4.5\n");

    // LEDs como salidas.
    gpio_set_direction(LED_BLANCO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_AZUL, GPIO_MODE_OUTPUT);

    // BOOT como entrada pull-up.
    gpio_set_direction(PULSADOR, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PULSADOR, GPIO_PULLUP_ONLY);

    while (1)
    {
        /*
         * Encendemos el LED blanco.
         *
         * GPIO4 está conectado en pull-up:
         * 0 = encendido.
         */
        gpio_set_level(LED_BLANCO, 0);

        /*
         * Consultamos BOOT.
         */
        int pulsador = gpio_get_level(PULSADOR);

        if (pulsador == 0)
        {
            gpio_set_level(LED_AZUL, 1);
        }
        else
        {
            gpio_set_level(LED_AZUL, 0);
        }

        /*
         * Durante estos 3 segundos el programa
         * NO vuelve a comprobar el pulsador.
         */
        vTaskDelay(pdMS_TO_TICKS(3000));


        /*
         * Apagamos el LED blanco.
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

        /*
         * Otros 3 segundos sin consultar BOOT.
         */
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
