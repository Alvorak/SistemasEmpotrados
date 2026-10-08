#include "esp_timer.h"

int64_t tiempo_actual = esp_timer_get_time();

if (hay_pulsacion_valida) {
    if (hay_pulsacion_anterior) {
        double intervalo_ms =
            (tiempo_actual - tiempo_anterior) / 1000.0;

        printf("Intervalo: %.2f ms\n", intervalo_ms);
    }

    tiempo_anterior = tiempo_actual;
    hay_pulsacion_anterior = true;
}
