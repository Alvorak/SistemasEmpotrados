¿Qué diferencia hay entre el 4.5 y el 5.1?

Esta es realmente la pregunta importante del apartado 5.

Ejercicio 4.5

Teníamos aproximadamente:

pulsador = gpio_get_level(BOTON_BOOT);

/* ... */

vTaskDelay(pdMS_TO_TICKS(3000));

Por tanto:

CPU
 │
 ├─ lee BOOT
 │
 ├──────── delay 3 segundos ────────┐
 │                                   │
 │              PULSO BOOT           │
 │                   ↓               │
 │                  X                │
 │            nadie lo está leyendo  │
 │                                   │
 └─ vuelve a leer BOOT

Una pulsación corta puede perderse.

Ejercicio 5.1

Ahora:

Programa principal
      │
      ├──── vTaskDelay(3000) ──────────────
      │
      │
      │          BOOT pulsado
      │                ↓
      │        INTERRUPCIÓN
      │                ↓
      │        boton_boot_ISR()
      │                ↓
      │        LED azul cambia
      │                ↓
      │
      └──── continúa el programa

El vTaskDelay(3000) no impide que la interrupción se atienda.

Eso es precisamente lo que pretende demostrar el ejercicio 5.1: que la acción del pulsador ocurra sin el retraso que aparecía en el ejercicio anterior.

Qué tienes que ver físicamente

Cuando ejecutes el 5.1:

LED BLANCO:

ON
│
├── 3 segundos
│
OFF
│
├── 3 segundos
│
ON
...

Mientras tanto:

BOOT pulsación 1 → LED azul ON

BOOT pulsación 2 → LED azul OFF

BOOT pulsación 3 → LED azul ON

BOOT pulsación 4 → LED azul OFF

Y no importa en qué momento de los 3 segundos pulses BOOT. El azul debería cambiar inmediatamente.
