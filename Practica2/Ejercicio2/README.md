# Práctica 2 — Ejercicio 2.1: lectura de ADC (ESP32-S3)

## Objetivo del enunciado

Programar en **ESP-IDF** una lectura del potenciómetro conectado a **GPIO7** cada **100 ms**; obtener dos muestras y presentar su **promedio**, junto con un **timestamp**, en el terminal serie. El enunciado exige `ADC_BITWIDTH_DEFAULT` y `ADC_ATTEN_DB_12`.

## Archivos

- `main/main.c`: implementación completa.
- `main/CMakeLists.txt`: componentes `esp_adc` y `esp_timer`.
- `CMakeLists.txt`: configuración raíz del proyecto ESP-IDF.

## Montaje eléctrico

Con la placa **desconectada del USB**:

| Patilla del potenciómetro | Conexión |
|---|---|
| Extremo 1 | **3V3** |
| Centro o cursor 2 | **GPIO7** |
| Extremo 3 | **GND** |

Si intercambias los extremos 1 y 3, cambia el sentido en el que aumenta la lectura al girar el mando, pero no el funcionamiento general. No conectes el cursor a 5 V: el ADC del ESP32-S3 no es tolerante a 5 V. El enunciado establece como tensión de trabajo hasta 3,3 V.

**Importante:** GPIO7 corresponde a **ADC1_CHANNEL_6** en ESP32-S3. El número de canal no coincide con el número del GPIO.

## Teoría y cálculos

### 1. Resolución del ADC

La conversión analógico-digital transforma una tensión en un número entero. Para **12 bits** existen:

```text
2^12 = 4096 códigos, de 0 a 4095
```

La resolución nominal, *si se representa idealmente el rango completo 0–3,3 V*, sería aproximadamente:

```text
3,3 V / 4095 = 0,000806 V ≈ 0,806 mV por código
```

Esta **es una equivalencia teórica simplificada**, no una calibración real del ADC. Debido a la atenuación, tolerancias y no linealidad, el valor leído no se debe convertir directamente a voltios con esa regla si se desea precisión. Para voltajes fiables, investigar las funciones de calibración `adc_cali_*` de ESP-IDF.

### 2. Promedio de dos muestras

Para dos lecturas `muestra1` y `muestra2`, la media aritmética es:

```text
promedio = (muestra1 + muestra2) / 2
```

Por ejemplo, para lecturas 2030 y 2050:

```text
promedio = (2030 + 2050) / 2 = 2040
```

El código utiliza `/ 2.0f` para mantener posibles medios valores (p. ej. `2040.5`). Promediar reduce parte del ruido aleatorio, pero **no elimina errores sistemáticos ni asegura una medida exacta**.

### 3. Período de lectura y tasa de resultados

```text
T = 100 ms = 0,1 s
f = 1 / T = 10 resultados por segundo
```

**Interpretación elegida:** cada 100 ms se realizan **dos conversiones seguidas** y se muestra **un resultado promediado**. El enunciado pide un resultado cada 100 ms y promediar dos muestras; no especifica separarlas por 100 ms.

Se utiliza `vTaskDelayUntil()` para mantener un período de publicación aproximadamente estable. El instante exacto depende del planificador, las conversiones y la salida serie.

### 4. Timestamp

`esp_timer_get_time()` devuelve microsegundos desde la inicialización del temporizador de ESP-IDF durante el arranque. Se captura después de obtener las dos muestras; por tanto corresponde al instante aproximado del **resultado**, no necesariamente al instante exacto de cada conversión.

```text
1 segundo = 1 000 000 microsegundos
100 ms = 100 000 microsegundos
```

El tipo `int64_t` permite almacenar valores grandes y la macro `PRId64` permite imprimirlos correctamente con `printf`.

## Por qué se usan estas funciones

| Elemento | Función |
|---|---|
| `adc_oneshot_new_unit` | Inicializa la unidad ADC1. |
| `adc_oneshot_config_channel` | Configura el canal 6, atenuación y resolución. |
| `adc_oneshot_read` | Realiza una conversión puntual y entrega el valor entero. |
| `ADC_ATTEN_DB_12` | Selecciona la atenuación solicitada, apropiada para ampliar el rango de entrada. |
| `ADC_BITWIDTH_DEFAULT` | Usa el ancho de conversión predeterminado, de 12 bits en ESP32-S3. |
| `esp_timer_get_time` | Obtiene una marca de tiempo en microsegundos. |
| `vTaskDelayUntil` | Mantiene un período de tarea más regular que pausas relativas repetidas. |
| `ESP_ERROR_CHECK` | Informa y detiene la ejecución si falla una llamada de ESP-IDF. |

## Cómo compilar y ejecutar

Abre la carpeta `ejercicio-2-1-adc` como proyecto de ESP-IDF en VS Code y selecciona la placa ESP32-S3 y su puerto serie. Desde un terminal con el entorno ESP-IDF cargado:

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p COM8 flash monitor
```

Sustituye `COM8` por el puerto real de tu equipo (o usa la opción *ESP-IDF: Monitor*). Si tu salida se enruta mediante USB Serial/JTAG, elige el puerto y la configuración de consola correspondientes.

La salida esperada sería similar a:

```text
Practica 2 - Ejercicio 2.1: ADC
GPIO7 / ADC1_CHANNEL_6, 12 bits, dos muestras cada 100 ms
t=2000000 us | ADC: 2030, 2050 | media: 2040.0
t=2100000 us | ADC: 2035, 2047 | media: 2041.0
```

Los números son ilustrativos y no constituyen resultados medidos.

## Comprobaciones para la demostración y el vídeo

1. Enseñar la placa, el potenciómetro y las tres conexiones.
2. Mostrar nombres de los integrantes, como exige el documento.
3. Mostrar el terminal con lecturas y timestamps.
4. Girar el potenciómetro lentamente: el promedio debe variar.
5. Comprobar que las marcas de tiempo avanzan cerca de 100 000 µs entre resultados.

El enunciado requiere vídeo de cada ejercicio; **en el 2.1 no pide osciloscopio**.

## Preguntas que podrían hacerte

1. **¿Por qué GPIO7 usa ADC_CHANNEL_6?** Por el mapeo de GPIO a canales ADC del ESP32-S3.
2. **¿Qué significa ADC de 12 bits?** Que produce 4096 códigos posibles, entre 0 y 4095.
3. **¿Por qué dos muestras?** Para promediar y reducir parte de las fluctuaciones aleatorias.
4. **¿Qué diferencia hay entre one-shot y continuo?** One-shot realiza conversiones solicitadas explícitamente; el modo continuo obtiene secuencias de muestras.
5. **¿Para qué sirve la atenuación?** Para adecuar el rango de entrada admitido por el ADC.
6. **¿Por qué usar `2.0f`?** Para realizar la división en coma flotante.
7. **¿Qué mide el timestamp?** El tiempo transcurrido desde que se inicializó el temporizador de ESP-IDF en el arranque.
8. **¿Son 4095 exactamente 3,3 V?** No necesariamente: esa relación es solo un modelo idealizado; medir tensión precisa requiere calibración.

## Limitaciones

- Se imprime **ADC crudo promediado**, no milivoltios calibrados: es lo que pide expresamente el enunciado.
- Dos lecturas consecutivas pueden estar correlacionadas; promediarlas no elimina todos los errores.
- La velocidad del terminal y la planificación de FreeRTOS pueden introducir algo de variación temporal.
- No se ha probado con una placa física en este entorno: comprueba compilación y lectura en tu ESP32-S3.
