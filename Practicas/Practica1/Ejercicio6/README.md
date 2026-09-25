# Práctica 1 - Ejercicio 6: Depuración

Placa: ESP32-S3  
Framework: ESP-IDF

## Objetivo

En este apartado se prueban dos formas de depurar un programa
ejecutado en el ESP32-S3:

1. Depuración mediante mensajes enviados por el terminal serie.
2. Depuración mediante JTAG.

---

# Ejercicio 6.1 - Depuración por terminal serie

El ejercicio 6.1 parte del programa realizado en el ejercicio 5.1.

Se mantienen:

- LED blanco conectado a GPIO4.
- LED azul conectado a GPIO5.
- Botón BOOT conectado a GPIO0.
- Interrupción asociada al botón BOOT.
- LED blanco cambiando de estado cada 3 segundos.
- LED azul cambiando de estado cada vez que se pulsa BOOT.

Además, cada vez que se pulsa BOOT se muestra información
por el terminal serie.

La información mostrada es:

- Número de veces que se ha pulsado BOOT.
- Timestamp en microsegundos desde el último reset del ESP32-S3.

La comunicación serie se realiza a:

115200 baudios

## Timestamp

Para obtener el número de microsegundos transcurridos desde
el arranque se utiliza:

```c
esp_timer_get_time();
