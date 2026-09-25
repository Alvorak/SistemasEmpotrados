#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"


/*
 * Pines utilizados.
 *
 * En el PDF:
 * GPIO4 -> LED rojo
 * GPIO5 -> LED verde
 *
 * En mi montaje:
 * GPIO4 -> LED blanco
 * GPIO5 -> LED azul
 */
#define LED_BLANCO GPIO_NUM_4
#define LED_AZUL   GPIO_NUM_5
#define BOTON_BOOT GPIO_NUM_0


/*
 * Estado del LED azul.
 *
 * volatile porque esta variable puede ser modificada
 * desde la rutina de interrupción.
 *
 * 0 -> apagado
 * 1 -> encendido
 */
static volatile int estado_led_azul = 0;


/*
 * Rutina de Servicio de Interrupción (ISR).
 *
 * Esta función se ejecuta automáticamente cuando
 * se detecta la pulsación de BOOT.
 *
 * BOOT está en pull-up:
 *
 * sin pulsar -> 1
 * pulsado    -> 0
 *
 * Por tanto, la pulsación provoca un flanco:
 *
 *      1 -> 0
 *
 * que configuraremos como GPIO_INTR_NEGEDGE.
 *
 * La ISR debe ser lo más corta posible.
 */
static void IRAM_ATTR boton_boot_ISR(void *args)
{
    /*
     * Invertimos el estado del LED azul.
     *
     * 0 -> 1
     * 1 -> 0
     */
    estado_led_azul = !estado_led_azul;

    /*
     * Actualizamos físicamente GPIO5.
     */
    gpio_set_level(LED_AZUL, estado_led_azul);
}


void app_main(void)
{
    printf("Ejercicio 5.1 - Interrupciones\n");


    /*
     * ---------------------------------------
     * CONFIGURACIÓN DE LOS LEDS
     * ---------------------------------------
     */

    // GPIO4 -> salida para el LED blanco.
    gpio_set_direction(LED_BLANCO, GPIO_MODE_OUTPUT);

    // GPIO5 -> salida para el LED azul.
    gpio_set_direction(LED_AZUL, GPIO_MODE_OUTPUT);

    // Inicialmente dejamos el LED azul apagado.
    gpio_set_level(LED_AZUL, 0);



    /*
     * ---------------------------------------
     * CONFIGURACIÓN DEL BOTÓN BOOT
     * ---------------------------------------
     *
     * En este ejercicio utilizamos gpio_config_t
     * porque necesitamos configurar además
     * la interrupción asociada al GPIO.
     */

    gpio_config_t config_boton = {

        /*
         * GPIO que queremos configurar.
         *
         * 1ULL << GPIO0
         */
        .pin_bit_mask = (1ULL << BOTON_BOOT),

        // BOOT es una entrada.
        .mode = GPIO_MODE_INPUT,

        /*
         * Activamos pull-up:
         *
         * sin pulsar = 1
         * pulsado    = 0
         */
        .pull_up_en = GPIO_PULLUP_ENABLE,

        .pull_down_en = GPIO_PULLDOWN_DISABLE,

        /*
         * Interrupción por flanco de bajada.
         *
         * 1 -> 0
         *
         * Esto ocurre cuando pulsamos BOOT.
         */
        .intr_type = GPIO_INTR_NEGEDGE
    };


    /*
     * Aplicamos la configuración anterior.
     */
    gpio_config(&config_boton);



    /*
     * ---------------------------------------
     * CONFIGURACIÓN DE LA INTERRUPCIÓN
     * ---------------------------------------
     */

    /*
     * Instalamos el servicio de interrupciones
     * de los GPIO.
     */
    gpio_install_isr_service(0);


    /*
     * Asociamos:
     *
     * GPIO0 (BOOT)
     *
     * con:
     *
     * boton_boot_ISR()
     *
     * Cuando se pulse BOOT, el procesador
     * ejecutará automáticamente esa función.
     */
    gpio_isr_handler_add(
        BOTON_BOOT,
        boton_boot_ISR,
        NULL
    );



    /*
     * ---------------------------------------
     * BUCLE PRINCIPAL
     * ---------------------------------------
     *
     * El programa principal solo se encarga
     * del LED blanco.
     *
     * El LED azul ya NO se controla desde aquí.
     * BOOT lo controla mediante interrupciones.
     */

    while (1)
    {
        /*
         * LED blanco encendido.
         *
         * Está conectado en pull-up,
         * por lo que 0 = encendido.
         */
        gpio_set_level(LED_BLANCO, 0);


        /*
         * Esperamos 3 segundos.
         *
         * Aunque estemos aquí esperando,
         * BOOT puede generar una interrupción.
         */
        vTaskDelay(pdMS_TO_TICKS(3000));


        /*
         * LED blanco apagado.
         *
         * 1 = apagado.
         */
        gpio_set_level(LED_BLANCO, 1);


        /*
         * Otros 3 segundos.
         *
         * BOOT continúa funcionando durante
         * este tiempo gracias a la interrupción.
         */
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
