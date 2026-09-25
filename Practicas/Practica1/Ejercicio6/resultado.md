# Resultado esperado - Ejercicio 6.1

## Objetivo

En este ejercicio se parte del programa realizado en el ejercicio 5.1.

El comportamiento de los LEDs se mantiene:

- El LED blanco conectado al GPIO4 cambia de estado cada 3 segundos.
- El LED azul conectado al GPIO5 cambia de estado cada vez que se pulsa el botón BOOT.
- El botón BOOT funciona mediante una interrupción, por lo que responde sin tener que esperar a que termine el `vTaskDelay()` del programa principal.

Además, cada pulsación del botón BOOT muestra información a través del terminal serie.

---

## Resultado de los LEDs

### LED blanco - GPIO4

El LED blanco cambia de estado cada 3 segundos.

```text
ENCENDIDO
    |
    | 3 segundos
    v
APAGADO
    |
    | 3 segundos
    v
ENCENDIDO
    |
   ...
