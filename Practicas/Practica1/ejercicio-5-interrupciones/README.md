# Práctica 1 - Ejercicio 5: Interrupciones

Placa: ESP32-S3  
Framework: ESP-IDF

## Ejercicio 5.1

El objetivo es mejorar el comportamiento observado
en el ejercicio 4.5 utilizando interrupciones.

### Pines

| Elemento | GPIO | Configuración |
|---|---:|---|
| LED blanco | GPIO4 | Salida pull-up |
| LED azul | GPIO5 | Salida pull-down |
| BOOT | GPIO0 | Entrada pull-up + interrupción |

### Resistencias

Los LEDs utilizan las mismas resistencias calculadas
en el ejercicio 4.2:

R = (3.3 V - 1.7 V) / 0.005 A

R = 320 ohm

Valor comercial utilizado: 330 ohm.

Cada LED utiliza su propia resistencia.

## Interrupción

BOOT utiliza una configuración pull-up:

- Sin pulsar: GPIO0 = 1
- Pulsado: GPIO0 = 0

Por tanto, al pulsarlo se produce un flanco de bajada:

1 -> 0

Se configura:

GPIO_INTR_NEGEDGE

Cada flanco ejecuta la ISR asociada al botón.

La ISR cambia el estado del LED azul:

OFF -> ON
ON  -> OFF

## Diferencia respecto al ejercicio 4.5

En el ejercicio 4.5 el botón solo se consultaba entre
los delays de 3 segundos, por lo que se podían perder
pulsaciones.

En el ejercicio 5.1 BOOT genera una interrupción,
por lo que el LED azul responde inmediatamente aunque
el programa principal esté dentro de vTaskDelay().
