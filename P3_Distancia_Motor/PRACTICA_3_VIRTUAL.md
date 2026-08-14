# Práctica 3 — Detector de distancia que gobierna un motor

**Cristian Carrera — 2024-1932 — 20241932@itla.edu.do**
IA e IoT 2026-C-2 · Prof. Luis Bessewell Feliz · **Valor: 8 puntos**
Entorno: **simulador Wokwi** (ver sección 9 para la justificación)

---

## 1. Requisitos del profesor

**Investigación previa**

1. Estructuras de control: `if`, `for`, `while`, `switch`.
2. Distancia máxima que soporta el detector y cómo funciona.
3. Sentencia para encender y apagar un rotor o motor.
4. Si Arduino soporta hilos y/o tareas, para programación paralela, concurrente y distribuida.

**Caso práctico** — encender un motor desde el arranque y controlarlo según la distancia:

| # | Condición | Comportamiento exigido |
|---|---|---|
| 1 | Entre 95 % y 5 % del máximo | Motor encendido + LED fijo encendido |
| 2 | Fuera de 95 %–5 % | Motor, LED y buzzer apagados |
| 3 | Entre 50 % y 25 % | LED parpadea + buzzer intermitente **al ritmo del LED** |
| 4 | Entre 26 % y 4 % | LED parpadea **más rápido** + buzzer con sonido particular, sincronizado |

---

## 2. Investigación previa

### 2.1 Estructuras de control

| Sentencia | Qué hace | Cuándo usarla | Uso en esta práctica |
|---|---|---|---|
| `if` / `else if` | Decide entre caminos según una condición | Cuando las condiciones son **rangos** o expresiones | Clasificar la distancia en zonas |
| `switch` | Elige entre **valores discretos** | Cuando comparas una variable contra constantes | Actuar según la zona ya calculada |
| `for` | Repite un número de veces **conocido** | Recorrer arreglos, contar iteraciones | Tomar 5 muestras del sensor |
| `while` | Repite **mientras** se cumpla algo | No sabes cuántas vueltas serán | Ordenar las muestras para la mediana |

```cpp
// if: compara RANGOS
if (d < UMBRAL_04)       return FUERA_DE_RANGO;
else if (d <= UMBRAL_26) return ZONA_CERCANA;

// switch: compara VALORES exactos
switch (zonaActual) {
  case ZONA_CERCANA: /* ... */ break;
  case ZONA_MEDIA:   /* ... */ break;
}

// for: 5 vueltas, número conocido
for (int i = 0; i < MUESTRAS; i++) { m[i] = medir(); }

// while: vueltas desconocidas, depende de los datos
while (j >= 0 && m[j] > clave) { m[j+1] = m[j]; j--; }
```

**El punto que conviene defender:** `switch` **no admite rangos**, solo valores constantes. Por eso el programa primero decide con `if` (que sí compara rangos) y después actúa con `switch` (que se lee mejor que seis `if` encadenados). No es que uno sea mejor: resuelven problemas distintos.

### 2.2 El HC-SR04: alcance y funcionamiento

| Parámetro | Valor |
|---|---|
| **Alcance** | **2 cm a 400 cm** (4 metros) |
| Precisión | ~3 mm |
| Ángulo de detección | cono de ~15° |
| Frecuencia | 40 kHz |
| Alimentación | 5 V, ~15 mA |

**Cómo funciona** — eco por ultrasonido, el principio del murciélago:

1. Se envía un pulso de **10 µs** por `TRIG`.
2. El sensor emite 8 ráfagas a 40 kHz.
3. El sonido viaja, rebota y vuelve.
4. `ECHO` permanece en alto exactamente el tiempo del viaje **de ida y vuelta**.
5. Se mide con `pulseIn()` y se convierte a distancia.

**La fórmula y de dónde sale:**

```
El sonido viaja a 343 m/s = 0.0343 cm/µs, o su inversa: 29.1 µs por cm

    distancia_cm = (tiempo_µs / 29.1) / 2
                                      ↑
                    se divide entre 2 porque el tiempo medido
                    es de IDA Y VUELTA, y solo interesa la ida
```

**Limitaciones reales:** las superficies blandas (tela, espuma) absorben el ultrasonido y no devuelven eco; las superficies en ángulo desvían el eco; y la velocidad del sonido depende de la temperatura, así que en un día caluroso la medida se desvía.

### 2.3 Encender y apagar un motor

Depende del tipo de motor:

| Tipo | Encender | Apagar |
|---|---|---|
| **Servo SG90** (el usado) | `miServo.attach(pin)` + `write(ángulo)` | `miServo.detach()` |
| Motor DC + L298N | `digitalWrite(IN1,HIGH); analogWrite(ENA,vel)` | `digitalWrite(IN1,LOW); digitalWrite(IN2,LOW)` |
| Paso a paso 28BYJ-48 | `motor.step(pasos)` | dejar de llamar a `step()` |

El servo no se controla con voltaje sino con el **ancho de un pulso** que se repite cada 20 ms: 1 ms = 0°, 2 ms = 180°. La librería `Servo.h` genera esos pulsos por debajo.

> ⚠️ **Nunca** conectar un motor directamente a un pin de Arduino. Un pin entrega 40 mA como máximo absoluto; un motor pide cientos. Siempre hace falta un driver o al menos un transistor.

### 2.4 ¿Arduino soporta hilos y tareas?

**Respuesta corta: no. Respuesta larga: sí hay concurrencia, que no es lo mismo.**

| Concepto | Definición | ¿En Arduino UNO? |
|---|---|---|
| **Secuencial** | Una instrucción tras otra, bloqueando | Sí, es lo natural |
| **Concurrente** | Varias tareas **progresan intercaladas** | ✅ Sí, con `millis()` |
| **Paralelo** | Varias tareas se ejecutan **literalmente a la vez** | ❌ No: un solo núcleo |
| **Distribuido** | Tareas en **máquinas distintas**, sin memoria común | ✅ Con red (ver práctica 5) |

El ATmega328P del UNO tiene **un núcleo**, sin sistema operativo ni gestión de memoria. No existen hilos reales.

**Las tres vías reales:**

1. **Multitarea cooperativa con `millis()`** ← la usada aquí. Ninguna función bloquea; cada tarea mira el reloj, se pregunta «¿ya me toca?», trabaja en microsegundos y devuelve el control. El `loop()` se convierte en un planificador.
2. **Interrupciones** (`attachInterrupt`, timers). Es lo más parecido a la expulsión de un SO: un evento externo detiene el programa principal.
3. **Un RTOS de verdad.** Existe un port de FreeRTOS para AVR, y el ESP32 trae FreeRTOS con **dos núcleos**, donde sí hay paralelismo real.

**Por qué esta práctica lo exige:** hay que medir la distancia, mover el servo, parpadear el LED y pitar **a ritmos distintos y al mismo tiempo**. Con `delay()`, mientras el LED espera su parpadeo el sensor estaría ciego y el servo congelado.

> **En todo el programa no hay ni un solo `delay()`.**

---

## 3. Interpretación del caso práctico

### Los porcentajes sobre 400 cm

| Porcentaje | Distancia |
|---|---|
| 95 % | 380 cm |
| 50 % | 200 cm |
| 26 % | 104 cm |
| 25 % | 100 cm |
| 5 % | 20 cm |
| 4 % | 16 cm |

### El problema: los rangos se solapan

```
   0    16   20        100  104        200            380   400 cm
   |----|----|----------|----|----------|--------------|-----|
        |=========== criterio 4: 4%-26% ==|
                       |==== criterio 3: 25%-50% ======|
             |================ criterio 1: 5%-95% ===========|
             
        ↑    ↑          ↑    ↑
        4%   5%        25%  26%
        └─ conflicto ──┘    └─ solape ─┘
```

**Dos conflictos concretos:**

1. Entre **100 y 104 cm** aplican a la vez el criterio 3 y el 4.
2. El criterio 4 baja hasta **16 cm**, por debajo del piso de 20 cm que el criterio 1 fija para operar.

### Resolución adoptada, y su justificación

**Se evalúa de MÁS CERCA a MÁS LEJOS.** En un sistema de proximidad, cuanto más cerca está el obstáculo más crítica es la alarma; por tanto la zona más cercana manda sobre la más lejana.

| Distancia | Estado | Motor | LED | Buzzer |
|---|---|---|---|---|
| < 16 cm | `FUERA_DE_RANGO` | ⬛ off | ⬛ off | ⬛ off |
| 16 – 104 cm | `ZONA_CERCANA` | ✅ on | ⚡ rápido (100 ms) | 🔊 sirena 2 tonos |
| 104 – 200 cm | `ZONA_MEDIA` | ✅ on | 💡 lento (400 ms) | 🔊 pitido simple |
| 200 – 380 cm | `ZONA_LEJANA` | ✅ on | 🔆 fijo | ⬛ off |
| > 380 cm | `FUERA_DE_RANGO` | ⬛ off | ⬛ off | ⬛ off |

El motor permanece encendido en las tres zonas activas porque el criterio 1 dice «encender… y **continúe su operatividad**»; solo se apaga al salir del rango útil.

---

## 4. Componentes virtuales

| Componente | Pieza en Wokwi | ¿Disponible? |
|---|---|---|
| Placa | `wokwi-arduino-uno` | ✅ verificado |
| Sensor de distancia | `wokwi-hc-sr04` | ✅ verificado |
| Motor | `wokwi-servo` | ✅ verificado |
| Zumbador | `wokwi-buzzer` | ✅ verificado |
| LED | `wokwi-led` | ✅ verificado |
| Resistencia 220 Ω | `wokwi-resistor` | ✅ verificado |

> **Verificación real:** cargué este `diagram.json` en Wokwi y comprobé que las 6 piezas se instancian sin error y que todos los pines usados existen. No es una suposición.

---

## 5. Diseño del circuito

```
                      ┌─────────────┐
        ┌─────────────┤  HC-SR04    │
        │  TRIG ── D9 │  ●●  sensor │
        │  ECHO ── D10│  ultrasónico│
        │  VCC  ── 5V └─────────────┘
        │  GND  ── GND
        │
   ┌────┴──────────────┐
   │   ARDUINO UNO     │──── D6 ──► [SERVO SG90]  (V+ a 5V, GND a GND)
   │                   │
   │                   │──── D8 ──► [BUZZER] ──► GND
   │                   │
   │                   │──── D13 ─► [LED] ─► [220Ω] ─► GND
   └───────────────────┘
```

**Por qué esos pines:** D9 y D10 son digitales normales, suficientes para TRIG/ECHO. D6 es PWM, requisito del servo. D8 sirve para `tone()`. D13 tiene el LED integrado en la placa, así que el estado se ve aunque no haya LED externo.

**Nota sobre temporizadores:** en el UNO la librería `Servo` usa el **Timer1** y `tone()` usa el **Timer2**. Son distintos, así que servo y zumbador conviven sin conflicto. Si se usaran ambos en el mismo timer, uno de los dos fallaría.

---

## 6. Tabla de conexiones

| Componente | Pin del componente | Pin Arduino | Notas |
|---|---|---|---|
| HC-SR04 | VCC | 5V | |
| HC-SR04 | GND | GND | |
| HC-SR04 | TRIG | **D9** | salida, pulso de 10 µs |
| HC-SR04 | ECHO | **D10** | entrada, se mide con `pulseIn` |
| Servo SG90 | V+ (rojo) | 5V | |
| Servo SG90 | GND (marrón) | GND | |
| Servo SG90 | PWM (naranja) | **D6** | pin PWM |
| Buzzer | (+) | **D8** | zumbador **pasivo** |
| Buzzer | (−) | GND | |
| LED | ánodo (A) | **D13** | |
| LED | cátodo (C) | → 220 Ω → GND | la resistencia es obligatoria |

---

## 7. Arquitectura de funcionamiento

```
   loop()  ──► PLANIFICADOR COOPERATIVO (sin delay)
     │
     ├─► tareaMedirDistancia()   cada  60 ms   ─► clasifica la zona
     ├─► tareaMoverMotor()       cada  15 ms   ─► 1° de barrido
     ├─► tareaLuzYSonido()       por evento    ─► switch(zona)
     └─► tareaReportar()         cada 500 ms   ─► monitor serie
```

Cada tarea compara `millis()` con su propia marca de tiempo y **devuelve el control en microsegundos**. Por eso las cuatro parecen simultáneas.

El filtrado del sensor usa **mediana de 5 muestras**, no promedio: el HC-SR04 suelta lecturas disparatadas de vez en cuando, y una sola lectura absurda arruinaría el promedio, mientras que a la mediana no le afecta.

---

## 8. Código Arduino

**Archivo completo y listo para copiar:** [`P3_Distancia_Motor.ino`](P3_Distancia_Motor.ino) (en esta misma carpeta).

Estructura del programa:

| Bloque | Líneas aprox. | Contenido |
|---|---|---|
| Encabezado | 1–150 | Los 4 temas de investigación documentados |
| Constantes | 150–200 | Pines, umbrales derivados de los porcentajes |
| `enum Zona` | 200–215 | Las 4 zonas con nombre |
| `setup()` | 240–270 | Pines, serie, arranque del motor |
| `loop()` | 270–285 | Planificador de 4 tareas |
| Tareas | 285–450 | Medición, motor, luz/sonido, reporte |

**El núcleo de la lógica** — clasificación con `if` (rangos) y actuación con `switch` (valores):

```cpp
Zona clasificarZona(float d) {
  if (d < 0)              return FUERA_DE_RANGO;   // sin eco
  else if (d < UMBRAL_04) return FUERA_DE_RANGO;   // < 4 %
  else if (d <= UMBRAL_26) return ZONA_CERCANA;    // criterio 4
  else if (d <= UMBRAL_50) return ZONA_MEDIA;      // criterio 3
  else if (d <= UMBRAL_95) return ZONA_LEJANA;     // criterio 1
  else                     return FUERA_DE_RANGO;  // > 95 %
}

switch (zonaActual) {
  case ZONA_CERCANA:                      // parpadeo rápido + sirena
    if (ahora - tParpadeo >= PARPADEO_RAPIDO) {
      tParpadeo = ahora;
      ledEncendido = !ledEncendido;
      digitalWrite(PIN_LED, ledEncendido);
      if (ledEncendido) {
        sirenaEnTonoA = !sirenaEnTonoA;   // alterna 2 frecuencias
        tone(PIN_ZUMBADOR, sirenaEnTonoA ? 1800 : 1200);
      } else noTone(PIN_ZUMBADOR);
    }
    break;
  // ...
}
```

Obsérvese que el sonido se enciende y apaga **dentro del mismo `if` que conmuta el LED**: así queda garantizada la sincronización que pide el enunciado.

---

## 9. Configuración de Arduino Cloud — y por qué no se usa

> ### ⚠️ Limitación del entorno, documentada
>
> **Arduino Cloud NO tiene simulador ni emulador de circuitos.**
>
> Según la [documentación oficial](https://docs.arduino.cc/arduino-cloud/), sus módulos son: Sketches, Things, Cloud Variables, Dashboards, Triggers, Scheduler, OTA, Webhooks y descarga de histórico. **Ninguno simula hardware.** Arduino Cloud sirve para programar y monitorear placas *reales*; no las sustituye.
>
> Además, el **Arduino UNO R3 no es compatible con Arduino IoT Cloud** porque carece de conectividad ([Arduino Help Center](https://support.arduino.cc/hc/en-us/articles/360016077320-Supported-Arduino-Cloud-devices)).

**Qué sí ofrece Arduino Cloud, y se usa:** el **Cloud Editor** (`app.arduino.cc/sketches`) permite escribir y compilar el sketch desde el navegador. El UNO R3 sí está soportado ahí. Sirve para demostrar que el código **compila**, aunque no para ejecutarlo sin placa.

**Sustitución adoptada:** **Wokwi** (`wokwi.com`), simulador que sí ejecuta el sketch con los periféricos. El propio facilitador mencionó esta categoría de herramientas en la sesión del 7 de agosto, refiriéndose a Tinkercad como equivalente.

| Objetivo académico | ¿Se cumple en Wokwi? |
|---|---|
| Ejecutar la lógica de los 4 criterios | ✅ |
| Variar la distancia y ver la respuesta | ✅ (control deslizante del HC-SR04) |
| Ver el LED parpadear a dos ritmos | ✅ |
| Oír el buzzer sincronizado | ✅ (audio real en el navegador) |
| Ver el motor girar | ✅ (servo animado) |
| Leer el monitor serie | ✅ |

**Ningún objetivo académico se pierde.**

---

## 10. Ejecución del simulador

1. Entrar a **wokwi.com** y crear cuenta (sirve la de Google).
2. **New Project → Arduino Uno**.
3. Pestaña del sketch: borrar todo y pegar `P3_Distancia_Motor.ino`.
4. Pestaña **`diagram.json`**: borrar todo y pegar el `diagram.json` de esta carpeta.
5. **Start Simulation**.
6. Hacer clic en el **HC-SR04**: aparece un control deslizante para fijar la distancia.

> Si sale *«Build Servers Busy»*, no es tu código: es la cola gratuita de compilación. Cierra el aviso y reintenta en unos minutos.

---

## 11. Casos de prueba

| # | Distancia | Zona esperada | Motor | LED | Buzzer |
|---|---|---|---|---|---|
| 1 | **400 cm** | FUERA_DE_RANGO | apagado | apagado | silencio |
| 2 | **300 cm** | ZONA_LEJANA | **encendido** | fijo | silencio |
| 3 | **380 cm** (borde 95 %) | ZONA_LEJANA | encendido | fijo | silencio |
| 4 | **150 cm** | ZONA_MEDIA | encendido | parpadeo lento | pitido 1000 Hz |
| 5 | **200 cm** (borde 50 %) | ZONA_MEDIA | encendido | parpadeo lento | pitido |
| 6 | **60 cm** | ZONA_CERCANA | encendido | parpadeo rápido | sirena 2 tonos |
| 7 | **104 cm** (borde solape) | **ZONA_CERCANA** | encendido | rápido | sirena |
| 8 | **16 cm** (borde 4 %) | ZONA_CERCANA | encendido | rápido | sirena |
| 9 | **10 cm** | FUERA_DE_RANGO | apagado | apagado | silencio |

**La prueba 7 es la importante:** demuestra que la resolución del solape funciona. A 104 cm aplican los criterios 3 y 4 a la vez, y el programa elige el 4 por ser el más urgente.

---

## 12. Resultados esperados

En el **monitor serie** (9600 baudios) debe verse:

```
== Practica 3: detector de distancia con motor ==
Umbrales calculados sobre 400 cm de alcance maximo:
  95% = 380.00 cm
  50% = 200.00 cm
  26% = 104.00 cm
   4% = 16.00 cm

>> MOTOR ENCENDIDO
*** CAMBIO DE ZONA -> LEJANA (LED fijo)
Distancia: 300.0 cm (75.0 %)  |  Zona: LEJANA (LED fijo)  |  Motor: ON
*** CAMBIO DE ZONA -> MEDIA (parpadeo lento)
Distancia: 150.0 cm (37.5 %)  |  Zona: MEDIA (parpadeo lento)  |  Motor: ON
*** CAMBIO DE ZONA -> CERCANA (parpadeo rapido)
Distancia:  60.0 cm (15.0 %)  |  Zona: CERCANA (parpadeo rapido)  |  Motor: ON
```

---

## 13. Evidencias que debo capturar

**Seis capturas. Ni una más.**

| # | Qué capturar | Qué demuestra |
|---|---|---|
| 1 | Circuito completo en Wokwi | El montaje con los 6 componentes |
| 2 | Código en el editor (el encabezado con la investigación) | Los 4 temas investigados |
| 3 | Distancia 300 cm: LED fijo, motor girando | **Criterio 1** |
| 4 | Distancia 400 cm: todo apagado | **Criterio 2** |
| 5 | Distancia 150 cm: LED parpadeando + monitor con ZONA MEDIA | **Criterio 3** |
| 6 | Distancia 60 cm: monitor con ZONA CERCANA | **Criterio 4** |

Para el **video**, un solo recorrido continuo: empezar en 400 cm y bajar despacio hasta 10 cm, narrando cada transición. Cubre los 4 criterios de una sola toma.

---

## 14. Explicación para la entrega

El enunciado pide tres formas:

| Forma | Qué subir |
|---|---|
| **A** | Las 6 capturas + el video del recorrido |
| **B** | Pegar el contenido de `P3_Distancia_Motor.ino` en el Editor de la plataforma |
| **C** | Adjuntar el archivo `P3_Distancia_Motor.ino` |

**Los tres puntos que conviene defender en voz alta:**

1. **El solape de rangos.** Que los criterios 3 y 4 se pisan entre 100 y 104 cm, y que la resolución (más cerca manda) es una decisión de diseño justificada, no un descuido.
2. **El `÷2` de la fórmula.** Porque el tiempo medido es de ida y vuelta.
3. **Por qué no hay `delay()`.** Que las cuatro tareas tienen ritmos distintos y con `delay()` el sensor quedaría ciego mientras el LED espera.

---

## 15. Posibles errores y soluciones

| Síntoma | Causa | Solución |
|---|---|---|
| El servo no se mueve | Falta la librería `Servo` | Viene con el IDE; en Wokwi se resuelve sola |
| Distancia siempre 0 o «sin eco» | TRIG y ECHO invertidos | TRIG → D9, ECHO → D10 |
| El LED no enciende | Falta la resistencia o está invertido | Ánodo (pata larga) al pin, cátodo a la resistencia |
| El buzzer no suena | Es un zumbador **activo** | En Wokwi la pieza `wokwi-buzzer` es pasiva; en físico, comprobarlo |
| «Build Servers Busy» | Cola gratuita de Wokwi saturada | Cerrar y reintentar; no es error del código |
| El LED parpadea a un solo ritmo | Se cambió el orden del `if` en `clasificarZona` | El orden debe ir de más cerca a más lejos |
| La placa se reinicia al mover el servo | Consumo del servo (en físico) | Fuente externa de 5 V compartiendo GND |

---

### Estado de verificación

| Elemento | Estado |
|---|---|
| `diagram.json` cargado en Wokwi | ✅ **verificado** — 6 piezas, 0 errores |
| Nombres de pines existen | ✅ **verificado** contra el DOM del simulador |
| Compilación del sketch | ⚠️ no comprobada (cola de Wokwi saturada) |
| Casos de prueba ejecutados | ⚠️ pendientes de ejecutar por el estudiante |
