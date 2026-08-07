# Prácticas de laboratorio — IA e IoT 2026-C-2

**Cristian Carrera — 2024-1932 — 20241932@itla.edu.do — Prof. Luis Bessewell Feliz**
Placa: **Elegoo UNO R3** (Super Starter Kit)

📦 **Repositorio público:** https://github.com/Criscarr26/practicas-arduino-itla

---

## Las cinco prácticas

| # | Carpeta | Qué hace | Valor |
|---|---|---|---|
| 1 | `P1_Mario_LED_Tono/` | Melodía de Super Mario Bros con luz sincronizada | — |
| 2 | `P2_SOS_LED_Tono/` | S.O.S. en código Morse con luz y sonido | — |
| 3 | `P3_Distancia_Motor/` | HC-SR04 que gobierna servo, LED y buzzer por rangos | 8 pts |
| 4 | `P4_Temperatura_Display/` | DHT11 + LED RGB + alarma + LCD1602 (opción D) | 8 pts |
| 5 | `P5_Arduino_Python/` | Arduino sensa, Python recibe, guarda y grafica | 8 pts |

Cada carpeta trae el `.ino`, un `diagram.json` para el simulador y un **`GUION.md`** con el
texto para grabarte explicando el código.

---

## Dos avisos importantes sobre tu placa

**1. El UNO R3 no sirve para Arduino IoT Cloud.** No tiene conectividad, así que no puede
crear un *Thing* ni un dashboard. Lo que sí puedes usar es:

- **Cloud Editor** (`app.arduino.cc`) → compilar y cargar desde el navegador. Requiere instalar
  el *Arduino Create Agent*. El UNO R3 sí está soportado aquí.
- **Wokwi** (`wokwi.com`) → el emulador. Pega el `.ino` y el `diagram.json` de cada carpeta.
- **Arduino IDE de escritorio** → la ruta normal, y la más cómoda para el hardware físico.

Como el enunciado dice *«Arduino Cloud de forma virtual con su emulador **o** su propio Arduino
y Arduino IDE de forma física»*, con tu UNO R3 la segunda opción es la que aplica de verdad. Te
dejo igual el `diagram.json` de cada práctica por si quieres mostrar también la simulación.

**2. El LCD1602 de tu kit es de pines paralelos, sin adaptador I2C.** Por eso la práctica 4 usa
la librería `LiquidCrystal` (incluida en el IDE) y **no** `LiquidCrystal_I2C`. Y necesita el
potenciómetro de 10k para el contraste: sin él la pantalla se ve en blanco y parece que el
código falla.

---

## Librerías a instalar

Desde el IDE: **Programa → Incluir Librería → Administrar Bibliotecas**

| Práctica | Librería |
|---|---|
| 1 y 2 | ninguna |
| 3 | `Servo` (ya viene incluida) |
| 4 | `DHT sensor library` de Adafruit + `Adafruit Unified Sensor` |
| 5 | `DHT sensor library` (Arduino) y `pip install pyserial matplotlib` (Python) |

---

## Videos de referencia

Los busqué y **comprobé uno por uno** el 7 de agosto de 2026: existen, son públicos y tratan el
tema. Casi todos en español.

### Para empezar: el IDE y el montaje básico

- [Arduino desde cero — Cap. 2: instalación del IDE y primer programa](https://www.youtube.com/watch?v=GUuWgk3dXd0) — bitwiseAr
- [Arduino desde cero — Cap. 3: entradas/salidas digitales con pulsador y LED](https://www.youtube.com/watch?v=BWhup75svIk) — bitwiseAr
- [Arduino desde cero — Cap. 55: buzzers activo y pasivo, tonos y melodías](https://www.youtube.com/watch?v=UQsixwoX5EQ) — bitwiseAr ← **empieza por este**
- [Instalar Arduino IDE para principiantes](https://www.youtube.com/watch?v=YVHKEFxpmU4) — Robotics Space NV
- [Tutorial Arduino IDE: todas sus funciones](https://www.youtube.com/watch?v=PlELXl_UX8w) — ElectronicSpot

### Práctica 1 — Mario Bros

- [Melodía de Super Mario Bros con Buzzer y Arduino — TUTORIAL](https://www.youtube.com/watch?v=tJgaZJyQqyw) — Ruben González
- [Melodía de Mario Bros con Buzzer Pasivo — Clase N°16](https://www.youtube.com/watch?v=dF_uaWd5f14) — D'_Electronic
- [Arduino tocando Mario Bros | Buzzer + Tinkercad](https://www.youtube.com/watch?v=iMKB9YEg33c) — santiago escobar
- [Super Mario song with buzzer, LED and Arduino](https://www.youtube.com/watch?v=urTC2JcGJhs) — Rocka Craft · inglés ← el único que incluye el LED
- [Mario Theme Song Using Arduino & Buzzer](https://www.youtube.com/watch?v=VRGvDYJx280) — Techno-E-Solution · inglés (explica `pitches.h`)

### Práctica 2 — S.O.S. en Morse

- [Práctica 5: señal SOS, código morse](https://www.youtube.com/watch?v=kfuxSLlqNv4) — makinando veleztecno
- [ARDUINO UNO — Código Morse SOS — LED](https://www.youtube.com/watch?v=vNqjuyrh7M8) — Dave D
- [Cómo hacer un mensaje en código Morse — Arduino](https://www.youtube.com/watch?v=pIu52oniqTY) — Piensa 3D
- [Código Morse Arduino | entradas y salidas digitales](https://www.youtube.com/watch?v=acZXhxwxqUY) — Electronic Lab
- [LED and Buzzer S.O.S. in Morse Code with Arduino Uno](https://www.youtube.com/watch?v=ACfUOOKZCHs) — MARIUS Petcu · inglés

### Práctica 3 — Distancia y motor

- [Arduino desde cero — Cap. 5: HC-SR04 sensor ultrasónico](https://www.youtube.com/watch?v=mlw3APOUt8U) — bitwiseAr ← **el mejor para entender el sensor**
- [if, else, for, while, switch — estructuras de control en Arduino](https://www.youtube.com/watch?v=UGTgpUH9xW8) — Pasión Electrónica
- [Programación en Arduino, HC-SR04 y librería NewPing](https://www.youtube.com/watch?v=Hs3UX0U1tEU) — Robótica, Impresión 3D y algo más
- [Alarma de proximidad con Arduino y sensor ultrasónico, LEDs y buzzer](https://www.youtube.com/watch?v=SFzbHsiydv4) — Nicolas Wasyleczko
- [Carro robot evasor de obstáculos: ultrasónico + servomotor + L298N](https://www.youtube.com/watch?v=I8hKgwnVGqM) — para la parte del motor

### Práctica 4 — Temperatura, humedad y display

**El sensor DHT11:**

- [Arduino desde cero — Cap. 7: temperatura y humedad con DHT22 o DHT11](https://www.youtube.com/watch?v=2tdsg_K-oQQ) — bitwiseAr
- [Cómo utilizar el sensor DHT11 en Arduino](https://www.youtube.com/watch?v=5rJnV9XyoJQ) — David Portilla
- [Sensor de temperatura y humedad DHT11 — Capítulo #45](https://www.youtube.com/watch?v=huIjJV2B8sA) — Johann Perez E

**La pantalla LCD1602 sin I2C** (así es la de tu kit):

- [Cómo usar una pantalla LCD 16x2 paso a paso desde cero](https://www.youtube.com/watch?v=zUGuOolVC8A) — Electrónicos por el Mundo ← **el importante**
- [Cómo usar el sensor DHT11/DHT22 con LCD1602 — Video 21](https://www.youtube.com/watch?v=lvBb1Ue8OJ4) — AlgoBack (junta sensor y pantalla)

**El LED RGB:**

- [Arduino desde cero — Cap. 12: LED RGB, ánodo y cátodo común](https://www.youtube.com/watch?v=xX8_QvwOJ6I) — bitwiseAr
- [Crea cualquier color con Arduino: tutorial LED RGB](https://www.youtube.com/watch?v=6IVFoQRnXeE) — ElectroLab Educativa

**Sensor de luz LDR** (por si eliges la opción C):

- [Arduino desde cero — Cap. 14: sensor crepuscular por LDR](https://www.youtube.com/watch?v=d3zcjfjqFxE) — bitwiseAr
- [Cómo utilizar un sensor de luz LDR](https://www.youtube.com/watch?v=h9wGZssIBOM) — Piensa 3D
- [Interruptor crepuscular con LDR y Arduino](https://www.youtube.com/watch?v=o2inQJlmiGA) — LSM Tutoriales
- [Detector de luz y oscuridad con LDR — TinkerCAD](https://www.youtube.com/watch?v=ET2Knx8E970) — Francisco Ramirez

### Práctica 5 — Arduino con Python

- [PYTHON ARDUINO SERIAL — pyserial — enviar datos de Arduino a Python](https://www.youtube.com/watch?v=RRpEXRyjYUs) — Turbo Código ← **empieza por este**
- [Visualiza temperatura y humedad en tiempo real con Python y Matplotlib](https://www.youtube.com/watch?v=JJTWqCMy5mg) — Programatumicro
- [Comunicación por puerto serial y registro de datos con Python y Arduino](https://www.youtube.com/watch?v=EV2kFHPJZy8) — Robótica con Python
- [Conexión serial Python-Arduino: sensor ultrasónico y potenciómetro](https://www.youtube.com/watch?v=hflgynhudUw) — Python 101
- [¿Cómo graficar datos de Arduino en tiempo real?](https://www.youtube.com/watch?v=Dkg4cSvqv10) — cctmexico

---

## Orden sugerido

1. **Práctica 1 y 2** primero: son las más simples y comparten montaje (buzzer + LED). Grábalas
   el mismo día sin desmontar nada.
2. **Práctica 5** después: solo añade sensores al mismo protoboard y no exige lógica nueva.
3. **Práctica 3** y **4** al final: son las de 8 puntos y las que más cableado piden.

---

## Lo que aún tienes que hacer tú

- Montar los circuitos y **grabar los cinco videos** (los guiones ya están escritos).
- En la práctica 4, **ajustar el contraste del LCD** con el potenciómetro antes de grabar.
- En la práctica 5, **cerrar el Monitor Serie** del IDE antes de ejecutar el Python.
- Subir cada entrega en las tres formas que pide el enunciado (video/imágenes, código pegado en
  el Editor, y el archivo adjunto).
