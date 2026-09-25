Aquí ya no queremos:

Mientras mantengo BOOT → LED encendido

Queremos:

Pulsación 1 → LED azul ON
Pulsación 2 → LED azul OFF
Pulsación 3 → LED azul ON
...

La práctica pide detectar específicamente el flanco de bajada y mantener un pequeño delay, proponiendo 10 ms.

Como BOOT funciona:

sin pulsar = 1
pulsado    = 0

el flanco es:

1 ─────────┐
           │
           └──────── 0
            ↑
        PULSACIÓN

*Aquí GPIO4 ya no es necesario, porque el ejercicio 4.6 solo pide detectar BOOT y cambiar el estado del LED conectado a GPIO5.*
