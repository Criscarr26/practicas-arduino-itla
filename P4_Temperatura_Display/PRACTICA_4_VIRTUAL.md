# Práctica 4 — Temperatura, humedad, luz y display

**Cristian Carrera — 2024-1932 — 20241932@itla.edu.do**
IA e IoT 2026-C-2 · Prof. Luis Bessewell Feliz · **Valor: 8 puntos**
Entorno: **simulador Wokwi** (ver sección 9)

> Se implementan **las cuatro opciones** del enunciado: A (temperatura), B (humedad), C (luz) y D (display).

---

## 1. Requisitos del profesor

**Investigación previa:** `if`/`for`/`while`/`switch`; detector de ausencia de luz; medidor de temperatura; medidor de humedad; pantalla de mensajes; y si Arduino soporta hilos/tareas para programación paralela, concurrente y distribuida.

**Caso práctico:**

| Opción | Exigencia |
|---|---|
| **A** | Temperatura normal → LED verde. Precaución → LED rojo + zumbido **continuo** hasta que baje. Crítico → LED rojo + tono **S.O.S.** |
| **B** | Igual que A, pero con humedad |
| **C** | S.O.S. **solo cuando no exista luz** en el ambiente |
| **D** | Cualquiera de las anteriores **+ display** con información dinámica |

---

## 2. Investigación previa

### 2.1 Estructuras de control

| Sentencia | Uso en esta práctica |
|---|---|
| `if`/`else if` | Clasificar temperatura y humedad — son **rangos** |
| `switch` | Actuar según la alarma ya calculada, y traducir el estado a texto |
| `for` | Recorrer los 3 canales del LED RGB en `setup()` |
| `while` | El propio `loop()` es en el fondo un `while(true)` |

`switch` **no admite rangos**, solo constantes. Por eso se decide con `if` y se actúa con `switch`.

### 2.2 Medidor de temperatura y humedad (DHT)

| | DHT11 (kit físico) | DHT22 (Wokwi) |
|---|---|---|
| Temperatura | 0–50 °C, ±2 °C | −40–80 °C, ±0.5 °C |
| Humedad | 20–90 %, ±5 % | 0–100 %, ±2 % |
| Muestreo | 1 lectura/segundo máximo | igual |

**Por dentro:** dos sensores en una cápsula. Humedad → un sustrato entre electrodos cuya **capacidad** cambia al absorber vapor. Temperatura → un **termistor NTC**, resistencia que baja al calentarse.

**El protocolo de un solo hilo:** no es I2C ni SPI. El micro tira la línea a cero ~18 ms, la suelta, y el sensor responde con **40 bits**. Cada bit se codifica por la **duración del pulso alto**: ~26 µs = 0, ~70 µs = 1. Por eso se usa librería.

> ⚠️ **Advertencia honesta:** el DHT es un sensor de **ambiente**, no un termómetro clínico. Con ±2 °C es imposible medir fiebre con rigor, donde un grado decide. Aquí demuestra la **lógica de umbrales**. En producto real: MLX90614 infrarrojo o DS18B20.

### 2.3 Detector de ausencia de luz (LDR)

Una fotorresistencia **cambia su resistencia** con la luz: a oscuras sube a megaohmios, con luz baja a cientos de ohmios.

Como Arduino mide **voltaje**, no resistencia, se monta un **divisor de tensión**: LDR en serie con 10 kΩ, y el punto medio a una entrada analógica.

```
   5V ──[ LDR ]──┬──[ 10kΩ ]── GND
                 │
                 └──► A0    analogRead(): 0 = oscuro, 1023 = luz plena
```

El módulo del simulador ya trae el divisor y entrega la señal lista en `AO`. Umbral usado: **por debajo de 300 se considera oscuridad**.

### 2.4 La pantalla LCD 1602

16 columnas × 2 filas, controlador **HD44780**, estándar desde los años ochenta. Se usa en **modo de 4 bits**: cada byte va en dos mitades, gastando 4 pines en vez de 8.

- `RS` → indica si lo enviado es **comando** (0) o **carácter** (1)
- `E` → pulso de habilitación; el dato se lee en su flanco

> ⚠️ **El potenciómetro de 10 kΩ es obligatorio.** El pin `V0` controla el contraste. Sin él la pantalla se ve toda blanca o toda negra y **parece que el código falla**. Es el error número uno con este display.

### 2.5 ¿Arduino soporta hilos y tareas?

| Concepto | ¿En UNO? |
|---|---|
| Secuencial | Sí, es lo natural |
| **Concurrente** | ✅ Sí, con `millis()` |
| **Paralelo** | ❌ No: un solo núcleo |
| **Distribuido** | ✅ Con red (práctica 5) |

Esta práctica lo **exige**: leer el DHT cada 2 s, la luz cada 200 ms, refrescar pantalla cada 500 ms y sonar un S.O.S. de símbolos de 150–450 ms. **Todo a la vez.** Con `delay()` la pantalla se congelaría mientras suena la alarma.

**No hay un solo `delay()` en el programa**, y el S.O.S. está escrito como máquina de estados.

---

## 3. Interpretación del caso práctico

### 3.1 Umbrales, y de dónde salen

**Temperatura corporal** — rangos clínicos de uso común:

| Rango | Estado | Respuesta |
|---|---|---|
| < 36.0 °C | Hipotermia | LED naranja + zumbido |
| 36.0 – 37.2 °C | **Normal** | LED **verde**, silencio |
| 37.3 – 38.9 °C | Fiebre / precaución | LED naranja + **zumbido continuo** |
| ≥ 39.0 °C | **Crítico** | LED **rojo** + **S.O.S.** |

**Humedad relativa de interiores** — la EPA de EE. UU. y la norma **ASHRAE 55** recomiendan mantener el interior entre **30 % y 60 %** para limitar moho y ácaros; por debajo de 20 % aparece irritación respiratoria:

| Rango | Estado |
|---|---|
| 30 – 60 % | **Normal** |
| 20–30 % o 60–70 % | Precaución |
| < 20 % o > 70 % | **Crítico** |

### 3.2 El conflicto que el enunciado no resuelve

Hay **un solo zumbador** y **tres subsistemas** que pueden querer usarlo. Si cada tarea escribiera por su cuenta se pisarían entre sí. Se resuelve con una jerarquía explícita, decidida en un único sitio:

| Prioridad | Condición | Sonido |
|---|---|---|
| 1 | Temperatura **crítica** | S.O.S. |
| 2 | Humedad **crítica** | S.O.S. |
| 3 | **Oscuridad** (opción C) | S.O.S. |
| 4 | Temperatura o humedad en precaución | Zumbido continuo |
| 5 | Todo normal | Silencio |

El **color** sigue una regla aparte: manda el nivel más grave entre temperatura y humedad. Si todo está normal pero no hay luz, el LED va **azul tenue** para distinguir ese caso.

---

## 4. Componentes virtuales

| Componente | Pieza Wokwi | Verificado |
|---|---|---|
| Placa | `wokwi-arduino-uno` | ✅ |
| Temperatura + humedad | `wokwi-dht22` | ✅ pines `VCC/SDA/NC/GND` |
| Pantalla | `wokwi-lcd1602` (`pins: "full"`) | ✅ pines `RS/E/D4–D7/V0/A/K` |
| Contraste | `wokwi-potentiometer` | ✅ |
| LED RGB | `wokwi-rgb-led` (cátodo común) | ✅ pines `R/G/B/COM` |
| Sensor de luz | `wokwi-photoresistor-sensor` | ✅ pines `VCC/GND/AO/DO` |
| Zumbador | `wokwi-buzzer` | ✅ |

> **Verificación real:** cargué este `diagram.json` en Wokwi. Las **7 piezas se instancian sin error** y todos los pines existen. En el proceso descubrí un fallo del montaje anterior: el LCD ocupaba A0–A5, que es justo donde debe ir el LDR. Ya está corregido.

---

## 5. Diseño del circuito

```
  [DHT22] ──D2──┐                    ┌── D12,D11,D7,D4,D3,D10 ──[LCD 1602]
                │                    │                              │ V0
  [LDR]  ──A0───┤   ARDUINO UNO      │                         [POT 10kΩ]
                │                    │
  [BUZZER]──D8──┤                    └── D5,D6,D9 ──[LED RGB]── COM ─ GND
                │
              GND/5V
```

**Por qué esos pines:** el RGB necesita **PWM real** y se le reservan 5, 6 y 9. Se evitan los pines **3 y 11** para PWM porque `tone()` usa el **Timer2**, que es el mismo del PWM en esos pines — el color cambiaría solo al sonar la alarma. El LCD sí puede ir en 3 y 11 porque solo usa `digitalWrite`.

---

## 6. Tabla de conexiones

| Componente | Pin componente | Pin Arduino |
|---|---|---|
| DHT | DATA / VCC / GND | **D2** / 5V / GND |
| Buzzer | (+) / (−) | **D8** / GND |
| LED RGB | R / G / B / COM | **D5** / **D6** / **D9** / GND |
| LDR | AO / VCC / GND | **A0** / 5V / GND |
| LCD | RS / E | **D12** / **D11** |
| LCD | D4 / D5 / D6 / D7 | **D7** / **D4** / **D3** / **D10** |
| LCD | VSS / RW / K | GND |
| LCD | VDD / A | 5V |
| LCD | V0 | patilla central del potenciómetro |
| Potenciómetro | extremos | 5V y GND |

---

## 7. Arquitectura de funcionamiento

```
  loop() ──► PLANIFICADOR COOPERATIVO (sin delay)
    │
    ├─► tareaLeerDHT()        cada 2000 ms   opciones A y B
    ├─► tareaLeerLuz()        cada  200 ms   opción C
    ├─► tareaDecidirAlarma()  cada vuelta    jerarquía de las 3
    ├─► tareaLuzYSonido()     por evento     switch(alarma)
    ├─► tareaPantalla()       cada  500 ms   opción D
    └─► tareaSerie()          cada 2000 ms
```

El **display alterna dos pantallas** cada segundo para caber en 16×2 sin recortar información: una con temperatura y humedad, otra con el nivel de luz. La fila inferior siempre muestra el estado.

---

## 8. Código Arduino

**Archivo completo:** [`P4_Temperatura_Display.ino`](P4_Temperatura_Display.ino)

**El corazón — la jerarquía de alarma en un solo sitio:**

```cpp
void tareaDecidirAlarma() {
  if (nivelTemp == CRITICO || nivelHum == CRITICO) {
    alarmaActual = SOS;            // 1 y 2: lo más grave
  } else if (!hayLuz) {
    alarmaActual = SOS;            // 3: oscuridad (opción C)
  } else if (nivelTemp == PRECAUCION || nivelHum == PRECAUCION) {
    alarmaActual = ZUMBIDO;        // 4: precaución
  } else {
    alarmaActual = SIN_ALARMA;     // 5
  }
  // al cambiar, el S.O.S. se reinicia para empezar por el primer punto
  if (alarmaActual != alarmaAnterior) { pasoMorse = 0; morseArrancado = false; ... }
}
```

**El S.O.S. sin bloquear** — guarda solo las duraciones, porque el patrón alterna sonido/silencio; los índices pares suenan:

```cpp
const uint8_t PATRON_SOS[] = { 1,1,1,1,1,3,  3,1,3,1,3,3,  1,1,1,1,1,7 };
//                             └── S ──┘     └── O ──┘     └── S ──┘
```

---

## 9. Configuración de Arduino Cloud — y por qué no se usa

> **Arduino Cloud NO tiene simulador ni emulador de circuitos.**
>
> Según la [documentación oficial](https://docs.arduino.cc/arduino-cloud/): Sketches, Things, Cloud Variables, Dashboards, Triggers, Scheduler, OTA, Webhooks. **Ninguno simula hardware.**
>
> Además el **UNO R3 no es compatible con Arduino IoT Cloud** ([Arduino Help Center](https://support.arduino.cc/hc/en-us/articles/360016077320-Supported-Arduino-Cloud-devices)).

**Sustitución:** **Wokwi**, que sí ejecuta el sketch con los periféricos. El facilitador mencionó esta categoría de herramientas (Tinkercad) en la sesión del 7 de agosto.

| Objetivo académico | ¿Wokwi? |
|---|---|
| Variar temperatura y humedad en vivo | ✅ deslizadores del DHT |
| Variar la luz | ✅ deslizador del LDR |
| Ver el LED cambiar de color | ✅ |
| Oír zumbido y S.O.S. | ✅ audio real |
| Ver el display actualizarse | ✅ |

---

## 10. Ejecución del simulador

1. **wokwi.com** → **New Project** → **Arduino Uno**.
2. Pegar `P4_Temperatura_Display.ino` en la pestaña del sketch.
3. Pegar el `diagram.json` de esta carpeta en su pestaña.
4. **Library Manager** (`+`) → añadir **DHT sensor library** y **Adafruit Unified Sensor**.
5. **Start Simulation**.
6. Clic en el **DHT** → deslizadores de temperatura y humedad. Clic en el **LDR** → deslizador de luz.
7. **Girar el potenciómetro** hasta que se lea el texto del LCD.

---

## 11. Casos de prueba

| # | Temp | Hum | Luz | Estado esperado | LED | Sonido |
|---|---|---|---|---|---|---|
| 1 | 36.5 | 45 % | clara | NORMAL | 🟢 verde | silencio |
| 2 | 38.0 | 45 % | clara | PRECAUCIÓN TEMP | 🟠 naranja | zumbido continuo |
| 3 | **39.5** | 45 % | clara | TEMP CRÍTICA | 🔴 rojo | **S.O.S.** |
| 4 | 36.5 | **15 %** | clara | HUM. CRÍTICA | 🔴 rojo | **S.O.S.** |
| 5 | 36.5 | 65 % | clara | PRECAUCIÓN HUM. | 🟠 naranja | zumbido |
| 6 | 36.5 | 45 % | **oscura** | SIN LUZ — S.O.S. | 🔵 azul | **S.O.S.** |
| 7 | **39.5** | 45 % | **oscura** | TEMP CRÍTICA | 🔴 rojo | S.O.S. |
| 8 | 35.0 | 45 % | clara | PRECAUCIÓN TEMP | 🟠 naranja | zumbido |

**La prueba 7 es la importante:** con temperatura crítica **y** oscuridad a la vez, gana la temperatura. Demuestra que la jerarquía funciona y que las tres opciones no se pisan.

---

## 12. Resultados esperados

```
== Practica 4: temperatura, humedad, luz y display ==
Opciones A + B + C + D del enunciado

T: 36.5 C | H: 45 % | Luz: 780 (claro)  | Estado: NORMAL - TODO OK
T: 38.0 C | H: 45 % | Luz: 780 (claro)  | Estado: PRECAUCION TEMP
T: 39.5 C | H: 45 % | Luz: 780 (claro)  | Estado: TEMP CRITICA SOS
T: 36.5 C | H: 45 % | Luz: 120 (OSCURO) | Estado: SIN LUZ - S.O.S.
```

En el LCD, alternando cada segundo:
```
┌────────────────┐   ┌────────────────┐
│T:36.5C H:45%   │   │Luz:780 CLARO   │
│NORMAL - TODO OK│   │NORMAL - TODO OK│
└────────────────┘   └────────────────┘
```

---

## 13. Evidencias que debo capturar

**Seis capturas:**

| # | Qué capturar | Demuestra |
|---|---|---|
| 1 | Circuito completo con las 7 piezas | El montaje |
| 2 | Encabezado del código (la investigación) | Los 5 temas |
| 3 | Estado normal: LED verde + LCD | **Opción A** normal |
| 4 | Temp 39.5: LED rojo + «TEMP CRITICA SOS» | **Opción A** crítico |
| 5 | Humedad 15 %: «HUM. CRITICA SOS» | **Opción B** |
| 6 | LDR oscuro: «SIN LUZ - S.O.S.» | **Opción C** |

La **opción D** queda demostrada en todas, porque el LCD aparece en las cuatro últimas.

Para el **video**: un recorrido continuo subiendo la temperatura de 36 a 40, luego bajando la humedad a 15 %, y al final tapando el LDR. Cubre las cuatro opciones en una toma.

---

## 14. Explicación para la entrega

| Forma | Qué subir |
|---|---|
| **A** | Las 6 capturas + el video |
| **B** | Pegar `P4_Temperatura_Display.ino` en el Editor |
| **C** | Adjuntar `P4_Temperatura_Display.ino` |

**Los tres puntos que conviene defender:**

1. **De dónde salen los umbrales.** Los de humedad vienen de EPA/ASHRAE 55 (30–60 % interior); los de temperatura, de rangos clínicos de uso común. No son inventados.
2. **La jerarquía de la alarma.** Hay un solo buzzer y tres opciones que lo quieren; sin una prioridad explícita se pisarían.
3. **El conflicto de temporizadores.** `tone()` usa el Timer2, el mismo del PWM en los pines 3 y 11. Por eso el RGB va en 5, 6 y 9.

---

## 15. Posibles errores y soluciones

| Síntoma | Causa | Solución |
|---|---|---|
| Pantalla en blanco o toda negra | Contraste | **Girar el potenciómetro** |
| Cuadros negros en la fila superior | El LCD arrancó mal | Revisar RS, E y los 4 pines de datos |
| Colores invertidos | RGB de ánodo común | `CATODO_COMUN = false` |
| `Sensor sin dato` | Falta la librería DHT, o cable suelto | Instalar *DHT sensor library* + *Adafruit Unified Sensor* |
| El color cambia al sonar la alarma | RGB en pines 3 u 11 | Devolverlo a 5, 6 y 9 |
| El S.O.S. empieza a media frase | — | Ya resuelto: `morseArrancado` fuerza el primer punto |
| DHT devuelve NaN sin parar | Se consulta más de 1 vez/segundo | `PERIODO_DHT` mínimo 2000 ms |

---

### Estado de verificación

| Elemento | Estado |
|---|---|
| `diagram.json` en Wokwi | ✅ **verificado** — 7 piezas, 0 errores |
| Nombres de pines | ✅ **verificado** contra el DOM |
| Conflicto A0 detectado y corregido | ✅ |
| Compilación del sketch | ⚠️ no comprobada (cola de Wokwi) |
| Casos de prueba | ⚠️ pendientes de ejecutar |
