<p align="center">
  <img src="itla_logo.png" alt="ITLA" width="190">
</p>

<h1 align="center">Prácticas de laboratorio — Arduino</h1>

<p align="center">
  <b>Inteligencia Artificial e Internet de las Cosas — 2026-C-2</b><br>
  Instituto Tecnológico de Las Américas (ITLA)
</p>

| | |
|---|---|
| **Facilitador** | Luis Bessewell Feliz |
| **Sustentante** | Cristian Carrera — Matrícula 2024-1932 |
| **Correo** | 20241932@itla.edu.do |
| **Placa** | Elegoo UNO R3 (Super Starter Kit) |

---

## Las prácticas

| # | Carpeta | Qué hace |
|---|---|---|
| Reto 1 | [`Reto1_Encender_Apagar_LED/`](Reto1_Encender_Apagar_LED) | Encender y apagar un LED |
| Tarea 1 | [`Tarea1_SOS_LED_Audio/`](Tarea1_SOS_LED_Audio) | S.O.S. en código Morse con luz y sonido |
| Tarea 2 | [`Tarea2_Mario_LED_Audio/`](Tarea2_Mario_LED_Audio) | Melodía de Super Mario Bros con luz sincronizada |
| Tarea 3 | [`Tarea3_Distancia_Motor/`](Tarea3_Distancia_Motor) | HC-SR04 que gobierna un servo por rangos — **simulador** |
| Tarea 3 | [`Tarea3_Distancia_Fisico/`](Tarea3_Distancia_Fisico) | HC-SR04 con aviso sonoro proporcional — **placa real** |
| Tarea 4 | [`Tarea4_Ausencia_Luz/`](Tarea4_Ausencia_Luz) | LDR que emite S.O.S. al faltar la luz — **opción C, placa real** |
| Tarea 4 | [`Tarea4_Humedad_Temp_LED/`](Tarea4_Humedad_Temp_LED) | DHT11 + LED RGB + alarma + LCD1602 — las cuatro opciones, simulador |
| Tarea 5 | [`Tarea5_Arduino_Python/`](Tarea5_Arduino_Python) | El sensor de la Tarea 3 leído **desde Python** por el puerto serie |

Cada carpeta trae el **`.ino`** con la investigación previa documentada en el encabezado, y
las que están pensadas para el simulador traen además un **`diagram.json`** listo para pegar
en [Wokwi](https://wokwi.com).

### Qué hace la Tarea 5

El profesor lo planteó así: *«yo lo que quiero es que tú salgas de Arduino… que desde otro
lenguaje leas por el puerto lo que él sensa»*, y dejó abierto el tema — *«tomen una de esas
prácticas y conviértanla»*.

Así que **no se monta nada nuevo**: se reutiliza tal cual el montaje de la Tarea 3, y lo que se
añade es que la placa, además de avisar por sonido, publique cada lectura en CSV por el puerto.
Del otro lado, Python la recibe, la tabula en vivo, la guarda y saca un informe.

El informe mide lo mismo que el profesor puso de ejemplo con la máquina de coser —*cuánto se
detiene, qué tan rápido trabaja*—: tiempo en cada zona, tiempo en marcha frente a detenida,
cuántas veces tuvo que parar y cuál fue la parada más larga.

Es además el tercer escalón del tema de la asignatura: dos programas en dos máquinas distintas
que no comparten memoria y se coordinan solo por mensajes. Eso es **programación distribuida**.

### Por qué la Tarea 4 tiene dos versiones

El enunciado plantea cuatro opciones —A temperatura, B humedad, C ausencia de luz, y D
cualquiera de esas más un display— y pide **elegir una**.

- **`Tarea4_Ausencia_Luz`** es la opción C, la que corre en la placa: fotorresistencia con
  divisor de tensión, y un S.O.S. en Morse mientras no haya luz. No usa ninguna librería.
- **`Tarea4_Humedad_Temp_LED`** hace las cuatro a la vez y está pensada para el simulador.
  Necesita el DHT, el LCD y su potenciómetro.

### Por qué la Tarea 3 tiene dos versiones

No conseguí el motor a tiempo y lo negocié con el profesor antes de la entrega. Lo que se
evalúa es el objetivo —medir distancia y decidir cuándo algo debe continuar o detenerse—, no
la pieza concreta.

- **`Tarea3_Distancia_Motor`** es la versión completa con servomotor, y corre en el simulador.
- **`Tarea3_Distancia_Fisico`** es la que corre en la placa real. Lleva **dos zumbadores**,
  cada uno con un mensaje distinto: el del pin 9 pulsa más rápido cuanto más cerca está el
  obstáculo, y el del pin 8 suena continuo solo cuando la máquina debe detenerse. No usa la
  librería `Servo`.

  Un detalle del hardware: en el UNO, `tone()` solo puede sonar en un pin a la vez, porque usa
  un único temporizador. Por eso los dos zumbadores no pueden ser pasivos — aquí uno es activo
  y el otro pasivo.

---

## Cómo ejecutar cada práctica

### En el simulador, sin hardware

1. Entra a [wokwi.com](https://wokwi.com) y crea un proyecto nuevo de **Arduino UNO**.
2. Pega el `.ino` en la pestaña del sketch.
3. Pega el `diagram.json` en la pestaña de ese mismo nombre: el montaje aparece ya cableado.
4. **Start simulation**.

### Con la placa física

Abre el `.ino` en el Arduino IDE, selecciona *Arduino UNO* y el puerto, y pulsa **Subir**.

---

## Librerías

Desde el IDE: **Programa → Incluir Librería → Administrar Bibliotecas**

| Práctica | Librería |
|---|---|
| Reto 1, Tareas 1 y 2 | ninguna |
| Tarea 3 — versión simulador | `Servo` |
| Tarea 3 — versión física | **ninguna** |
| Tarea 4 — opción C, física | **ninguna** |
| Tarea 4 — simulador | `DHT sensor library` de Adafruit + `Adafruit Unified Sensor` |
| Tarea 5 | `DHT sensor library` en Arduino, y en Python: `pip install pyserial matplotlib` |

La `Servo` no viene instalada de fábrica: hay que añadirla desde el Administrador de
Bibliotecas. Si al compilar la versión física aparece un aviso pidiendo instalar algo de
motores, es que está abierto el sketch del simulador y no el de la placa.

---

## Notas técnicas que conviene leer

**El LCD1602 del kit Elegoo es de pines paralelos, sin adaptador I2C.** Por eso la práctica 4
usa la librería `LiquidCrystal` y **no** `LiquidCrystal_I2C`. Necesita además un potenciómetro
de 10k para el contraste: sin él la pantalla se ve en blanco y parece que el código falla.

**`tone()` usa el Timer2 del ATmega328P**, que es el mismo que genera el PWM de los pines 3 y
11. Por eso en la práctica 4 el LED RGB va en los pines 5, 6 y 9: si estuviera en el 3 o el 11,
el color cambiaría solo cada vez que sonara la alarma.

**El UNO R3 no es compatible con Arduino IoT Cloud** (no tiene conectividad). Sí funciona con
el *Cloud Editor* para compilar y cargar desde el navegador.

**Ninguna de las prácticas 3, 4 y 5 usa `delay()`.** Todas están escritas con multitarea
cooperativa basada en `millis()`, porque tienen que atender varias tareas con ritmos distintos
al mismo tiempo. Es la respuesta práctica a la pregunta de investigación sobre si Arduino
soporta programación en hilos o tareas: hilos reales no, pero concurrencia sí.

---

## Licencia

Trabajo académico. El código es de elaboración propia. El logo del ITLA pertenece al
Instituto Tecnológico de Las Américas y se usa aquí únicamente para identificar la institución
en la que se presenta este trabajo.
