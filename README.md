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

## Las cinco prácticas

| # | Carpeta | Qué hace | Valor |
|---|---|---|---|
| 1 | [`P1_Mario_LED_Tono/`](P1_Mario_LED_Tono) | Melodía de Super Mario Bros con luz sincronizada | — |
| 2 | [`P2_SOS_LED_Tono/`](P2_SOS_LED_Tono) | S.O.S. en código Morse con luz y sonido | — |
| 3 | [`P3_Distancia_Motor/`](P3_Distancia_Motor) | HC-SR04 que gobierna servo, LED y buzzer por rangos | 8 pts |
| 4 | [`P4_Temperatura_Display/`](P4_Temperatura_Display) | DHT11 + LED RGB + alarma + LCD1602 (opción D) | 8 pts |
| 5 | [`P5_Arduino_Python/`](P5_Arduino_Python) | Arduino sensa, Python recibe, guarda y grafica | 8 pts |

Cada carpeta trae:

- el **`.ino`** con la investigación previa documentada en el encabezado,
- un **`diagram.json`** listo para pegar en [Wokwi](https://wokwi.com),
- un **`GUION.md`** con el texto para grabar la explicación en video.

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
| 1 y 2 | ninguna |
| 3 | `Servo` (incluida en el IDE) |
| 4 | `DHT sensor library` de Adafruit + `Adafruit Unified Sensor` |
| 5 | `DHT sensor library` en Arduino, y en Python: `pip install pyserial matplotlib` |

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
