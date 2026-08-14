# Entregables — IA e IoT 2026-C-2

**Cristian Carrera — 2024-1932 — 20241932@itla.edu.do — Prof. Luis Bessewell Feliz**

📦 **Repositorio público:** https://github.com/Criscarr26/practicas-arduino-itla

---

## ⚠️ Léelo antes de subir nada

Las carpetas están nombradas **según la numeración del aula virtual**, no según el orden en que
se desarrollaron. Ojo con esto, porque la 1 y la 2 son al revés de lo que uno esperaría:

> **Tarea 1era = S.O.S.** · **Tarea 2da = Mario Bros**

Cada carpeta se llama igual que la tarea a la que va. Si subes `Tarea1_SOS_LED_Audio` a la
«1era. Asignación Práctica de Laboratorio», estás bien.

---

## Mapa: aula virtual → carpeta

### Retos

| En la plataforma | Carpeta | Estado |
|---|---|---|
| **Reto 1era** — Encender y Apagar Luz LED | `Reto1_Encender_Apagar_LED/` | ✅ listo |
| **Reto 2da** — S.O.S con luz LED y audio | usa `Tarea1_SOS_LED_Audio/` | ✅ mismo contenido |

> El Reto 2 y la Tarea 1era piden lo mismo. Puedes subir el mismo sketch a ambos; el de la
> Tarea 1era está más documentado.

### Prácticas (las que tienen puntuación)

| En la plataforma | Carpeta | Valor |
|---|---|---|
| **Tarea 1era** — S.O.S con luz LED y audio | `Tarea1_SOS_LED_Audio/` | — |
| **Tarea 2da** — Música Mario Bros con luz LED y audio | `Tarea2_Mario_LED_Audio/` | — |
| **Tarea 3era** — Detector o Medidor de Distancia / Temperatura | `Tarea3_Distancia_Motor/` | 8 pts |
| **Tarea 4ta** — Distancia, Humedad, Temperatura, Colores LED | `Tarea4_Humedad_Temp_LED/` | 8 pts |
| **Tarea 5ta** — Lectura de Datos Sensados vía otro lenguaje | `Tarea5_Arduino_Python/` | 8 pts |

---

## Qué hay en cada carpeta

| Archivo | Para qué sirve |
|---|---|
| `*.ino` | El código. **Es el que se adjunta y se pega en el Editor** |
| `diagram.json` | El montaje, para pegar en Wokwi. No se entrega |
| `GUION.md` | Texto para grabarte explicando el código |
| `PRACTICA_*_VIRTUAL.md` | Documento de 15 secciones (solo tareas 3, 4 y 5) |

`MATRIZ_CUMPLIMIENTO.md`, en la raíz, dice requisito por requisito qué está verificado y qué
está pendiente de que lo ejecutes.

---

## Cómo pasarlo a Wokwi (4 pasos por tarea)

1. **wokwi.com** → **New Project** → elegir placa:
   - **Arduino UNO** para Reto 1, Tareas 1, 2, 3 y 4
   - **ESP32** para la Tarea 5 (necesita WiFi)
2. Pegar el contenido del `.ino` en la pestaña del sketch.
3. Pegar el `diagram.json` en la pestaña de ese nombre. El montaje aparece cableado.
4. **Start Simulation**.

**Librerías** (solo tareas 4 y 5): en el botón `+` del Library Manager añadir
`DHT sensor library`, `Adafruit Unified Sensor` y, para la 5, `PubSubClient`.

> Si sale *«Build Servers Busy»* no es tu código: es la cola gratuita de Wokwi. Cierra y
> reintenta en unos minutos.

---

## Nota sobre Arduino Cloud

**Arduino Cloud no tiene simulador.** Sus módulos son Sketches, Things, Dashboards, Triggers,
OTA y Webhooks — ninguno simula hardware ([docs oficiales](https://docs.arduino.cc/arduino-cloud/)).
Y el **UNO R3 tampoco es compatible con Arduino IoT Cloud** por no tener conectividad.

Lo que sí ofrece es el **Cloud Editor**, que compila desde el navegador. Para simular de verdad
el camino es **Wokwi** (el facilitador mencionó Tinkercad, que es la misma categoría de
herramienta).

---

## Qué falta, y es tuyo

| Tarea | Cuánto |
|---|---|
| Cargar en Wokwi y comprobar que **compila** | 6 sketches |
| Ejecutar los **casos de prueba** | 25 casos (tareas 3, 4 y 5) |
| Tomar **capturas** | 17 |
| Grabar **videos** | 3 a 5 |

Cada entrega pide **tres formas**: (A) imágenes o video, (B) el código pegado en el Editor de la
plataforma, y (C) el archivo `.ino` adjunto.

---

## Videos de referencia

Comprobados uno por uno el 7 de agosto de 2026: existen, son públicos y tratan el tema.

### Para empezar

- [Arduino desde cero — Cap. 2: instalación del IDE](https://www.youtube.com/watch?v=GUuWgk3dXd0) — bitwiseAr
- [Arduino desde cero — Cap. 3: entradas/salidas digitales con LED](https://www.youtube.com/watch?v=BWhup75svIk) — bitwiseAr ← **para el Reto 1**
- [Arduino desde cero — Cap. 55: buzzers activo y pasivo](https://www.youtube.com/watch?v=UQsixwoX5EQ) — bitwiseAr ← **míralo antes de las tareas 1 y 2**

### Tarea 1era — S.O.S. en Morse

- [ARDUINO UNO — Código Morse SOS — LED](https://www.youtube.com/watch?v=vNqjuyrh7M8) — Dave D
- [Práctica 5: señal SOS, código morse](https://www.youtube.com/watch?v=kfuxSLlqNv4) — makinando veleztecno
- [Cómo hacer un mensaje en código Morse](https://www.youtube.com/watch?v=pIu52oniqTY) — Piensa 3D
- [LED and Buzzer S.O.S. in Morse Code](https://www.youtube.com/watch?v=ACfUOOKZCHs) — MARIUS Petcu · inglés

### Tarea 2da — Mario Bros

- [Melodía de Super Mario Bros con Buzzer — TUTORIAL](https://www.youtube.com/watch?v=tJgaZJyQqyw) — Ruben González
- [Melodía de Mario Bros con Buzzer Pasivo — Clase N°16](https://www.youtube.com/watch?v=dF_uaWd5f14) — D'_Electronic
- [Super Mario song with buzzer, LED and Arduino](https://www.youtube.com/watch?v=urTC2JcGJhs) — inglés ← el único que incluye el LED

### Tarea 3era — Distancia y motor

- [Arduino desde cero — Cap. 5: HC-SR04](https://www.youtube.com/watch?v=mlw3APOUt8U) — bitwiseAr ← **el mejor para el sensor**
- [if, else, for, while, switch en Arduino](https://www.youtube.com/watch?v=UGTgpUH9xW8) — Pasión Electrónica
- [Alarma de proximidad con ultrasónico, LEDs y buzzer](https://www.youtube.com/watch?v=SFzbHsiydv4) — Nicolas Wasyleczko
- [Ultrasónico + servomotor + L298N](https://www.youtube.com/watch?v=I8hKgwnVGqM) — para el motor

### Tarea 4ta — Temperatura, humedad, luz y display

**Sensor DHT:**
- [Cap. 7: temperatura y humedad con DHT22 o DHT11](https://www.youtube.com/watch?v=2tdsg_K-oQQ) — bitwiseAr
- [Cómo utilizar el sensor DHT11](https://www.youtube.com/watch?v=5rJnV9XyoJQ) — David Portilla

**Pantalla LCD sin I2C:**
- [Cómo usar una LCD 16x2 paso a paso](https://www.youtube.com/watch?v=zUGuOolVC8A) — Electrónicos por el Mundo ← **clave**
- [DHT11/DHT22 con LCD1602 — Video 21](https://www.youtube.com/watch?v=lvBb1Ue8OJ4) — AlgoBack

**LED RGB:**
- [Cap. 12: LED RGB, ánodo y cátodo común](https://www.youtube.com/watch?v=xX8_QvwOJ6I) — bitwiseAr
- [Crea cualquier color con Arduino](https://www.youtube.com/watch?v=6IVFoQRnXeE) — ElectroLab Educativa

**Sensor de luz LDR:**
- [Cap. 14: sensor crepuscular por LDR](https://www.youtube.com/watch?v=d3zcjfjqFxE) — bitwiseAr
- [Cómo utilizar un sensor de luz LDR](https://www.youtube.com/watch?v=h9wGZssIBOM) — Piensa 3D
- [Detector de luz y oscuridad — TinkerCAD](https://www.youtube.com/watch?v=ET2Knx8E970) — Francisco Ramirez

### Tarea 5ta — Arduino con Python

- [PYTHON ARDUINO SERIAL — pyserial](https://www.youtube.com/watch?v=RRpEXRyjYUs) — Turbo Código ← **empieza aquí**
- [Comunicación serial y registro de datos](https://www.youtube.com/watch?v=EV2kFHPJZy8) — Robótica con Python
- [Graficar datos de Arduino en tiempo real](https://www.youtube.com/watch?v=Dkg4cSvqv10) — cctmexico
- [Visualiza temperatura y humedad con Python y Matplotlib](https://www.youtube.com/watch?v=JJTWqCMy5mg) — Programatumicro

---

## Orden sugerido

1. **Reto 1** — 5 minutos, y te sirve de calentamiento con Wokwi.
2. **Tareas 1 y 2** — comparten montaje (buzzer en el 8 + LED en el 13). Hazlas seguidas sin
   cambiar el circuito.
3. **Tarea 5** — con el ESP32, independiente de las demás.
4. **Tareas 3 y 4** — las de 8 puntos, las que más cableado piden.
