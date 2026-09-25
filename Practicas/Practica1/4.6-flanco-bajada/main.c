#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_AZUL GPIO_NUM_5
#define PULSADOR GPIO_NUM_0


void app_main(void)
{
    printf("Ejercicio 4.6\n");

    /*
     * LED azul como salida.
     */
    gpio_set_direction(LED_AZUL, GPIO_MODE_OUTPUT);

    /*
     * BOOT como entrada pull-up.
     */
    gpio_set_direction(PULSADOR, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PULSADOR, GPIO_PULLUP_ONLY);


    /*
     * Estado lógico actual del LED.
     *
     * Empieza apagado.
     */
    int estado_led = 0;

    gpio_set_level(LED_AZUL, estado_led);


    /*
     * Necesitamos recordar el estado anterior
     * del pulsador para poder detectar el cambio.
     */
    int pulsador_anterior = gpio_get_level(PULSADOR);


    while (1)
    {
        /*
         * Estado actual del botón.
         */
        int pulsador_actual = gpio_get_level(PULSADOR);


        /*
         * Detectamos el flanco de bajada:
         *
         * anterior = 1
         * actual   = 0
         *
         * Eso significa que BOOT acaba de
         * ser pulsado.
         */
        if (pulsador_anterior == 1 &&
            pulsador_actual == 0)
        {
            /*
             * Invertimos el estado del LED.
             *
             * 0 -> 1
             * 1 -> 0
             */
            estado_led = !estado_led;

            gpio_set_level(LED_AZUL, estado_led);

            printf(
                "BOOT pulsado - LED azul: %s\n",
                estado_led ? "ON" : "OFF"
            );
        }


        /*
         * El estado actual pasa a ser el
         * estado anterior para la siguiente
         * iteración.
         */
        pulsador_anterior = pulsador_actual;


        /*
         * Pequeño delay indicado en la práctica.
         */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
