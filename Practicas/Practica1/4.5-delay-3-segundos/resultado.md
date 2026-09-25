Aquí prácticamente no cambia nada.

La intención del ejercicio es aumentar mucho el delay y comprobar el problema que provoca: mientras está dentro de vTaskDelay(), no estamos leyendo BOOT.

Hay además una pequeña inconsistencia en el PDF: el texto dice 3 segundos, aunque al lado aparece pdMS_TO_TICKS(1000). Para que sean realmente tres segundos hay que utilizar:

pdMS_TO_TICKS(3000)

Si hacemos:
Entramos en delay
      ↓
┌───────────────────────┐
│      3 segundos       │
└───────────────────────┘
      ↑
pulsas BOOT aquí

y sueltas BOOT antes de que termine el delay, el programa puede no detectar nunca esa pulsación.

Ese es precisamente el problema que prepara los siguientes ejercicios. => INTERRUPCIONES
