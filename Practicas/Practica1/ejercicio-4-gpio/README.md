# Práctica 1 - Ejercicio 4: GPIO

Placa: ESP32-S3
Framework: ESP-IDF

## Pines utilizados

| Elemento | GPIO | Configuración |
|----------|------|---------------|
| LED blanco | GPIO4 | Salida pull-up |
| LED azul | GPIO5 | Salida pull-down |
| BOOT | GPIO0 | Entrada pull-up |

## Resistencias

Datos de diseño:

- Vcc = 3.3 V
- VLED = 1.7 V
- ILED = 5 mA

R = (3.3 - 1.7) / 0.005
R = 320 ohm

Valor utilizado: 330 ohm.

Cada LED utiliza su propia resistencia.

## Ejercicio 4.4

- GPIO4 parpadea cada 100 ms.
- GPIO5 sigue el estado del botón BOOT.

## Ejercicio 4.5

- GPIO4 cambia de estado cada 3 segundos.
- Se observa que vTaskDelay impide consultar el botón
  durante ese intervalo.

## Ejercicio 4.6

- Se detecta el flanco de bajada de BOOT.
- Cada pulsación cambia el estado de GPIO5.
