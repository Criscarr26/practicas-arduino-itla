# Cómo se entrega — IA e IoT 2026-C-2

**Cristian Carrera — 2024-1932 — 20241932@itla.edu.do — Prof. Luis Bessewell Feliz**

📦 **Repositorio público:** https://github.com/Criscarr26/practicas-arduino-itla

> Revisión del 18 de agosto de 2026. La versión anterior mandaba a entregar las versiones de
> simulador y a pegar el código en Moodle: **las dos cosas son incorrectas** según lo que el
> profesor fijó en la clase de la Semana 14.

---

## ⚠️ Las tres reglas que no se pueden saltar

1. **Nada de código en Moodle.** La plataforma puede tomar un `.ino` por virus y borrarlo.
   Va todo a una **carpeta compartida**, con una subcarpeta por práctica que contenga el código
   **y** el video. En Moodle solo se pega el enlace, indicando qué práctica es.
2. **Dos evidencias por práctica:** que funciona físicamente, y que tú lo explicas con tus
   palabras. Pueden ir en un video o en dos.
3. **Confirmar el envío.** En estado borrador la tarea es editable pero **no cuenta como
   entregada**.

Y una más, que juega a tu favor: **el uso de IA se acepta si lo declaras** y sabes explicar lo
entregado. Está declarado en la cabecera de cada sketch y en el README.

---

## Ojo con la numeración

Las carpetas van **según el aula virtual**, no según el orden en que se desarrollaron. La 1 y la
2 están al revés de lo que uno esperaría:

> **Tarea 1era = S.O.S.** · **Tarea 2da = Mario Bros**

---

## Qué carpeta va a cada entrega

| En la plataforma | Carpeta que se entrega | Valor |
|---|---|---|
| **Reto 1era** — Encender y Apagar LED | `Reto1_Encender_Apagar_LED/` | — |
| **Reto 2da** — S.O.S con LED y audio | `Tarea1_SOS_LED_Audio/` (mismo contenido) | — |
| **Tarea 1era** — S.O.S con LED y audio | `Tarea1_SOS_LED_Audio/` | — |
| **Tarea 2da** — Música Mario Bros | `Tarea2_Mario_LED_Audio/` | — |
| **Tarea 3era** — Detector de Distancia | **`Tarea3_Distancia_Fisico/`** | 8 pts |
| **Tarea 4ta** — Temperatura / Humedad / Luz | **`Tarea4_Ausencia_Luz/`** | 8 pts |
| **Tarea 5ta** — Lectura vía otro lenguaje | `Tarea5_Arduino_Python/` | 8 pts |

### Las dos carpetas que NO son la entrega

| Carpeta | Qué es |
|---|---|
| `Tarea3_Distancia_Motor/` | La versión con servo, para simulador. **Aporte adicional** |
| `Tarea4_Humedad_Temp_LED/` | Las cuatro opciones A–D en simulador. **Aporte adicional** |

Van dentro de la subcarpeta de su práctica, **presentadas como extra**. El consejo del profesor
fue: primero cumple exactamente lo pedido, y encima agrega tu aporte; no lo sustituyas.

---

## Estructura de la carpeta compartida

```
Práctica N/
├── el .ino  (y el .py / el proyecto de C# en la práctica 5)
├── el video
└── (opcional) la versión de simulador, como extra
```

---

## Cómo ejecutar cada práctica

### Con la placa física — prácticas 3, 4 y 5

Abre el `.ino` en el Arduino IDE, selecciona *Arduino UNO* y el puerto, y pulsa **Subir**.

Para la **práctica 5**, además, del lado de la computadora:

```bash
python lector_distancia.py
```

o abre `LectorDistancia.sln` en **Visual Studio 2022** y pulsa **F5**.

> El puerto serie admite **un solo programa a la vez**. Si el Monitor Serie del IDE está
> abierto, el lector falla con *acceso denegado*.

### En el simulador — las dos versiones extra

1. [wokwi.com](https://wokwi.com) → **New Project** → **Arduino UNO**.
2. Pegar el `.ino` en la pestaña del sketch.
3. Pegar el `diagram.json` en la pestaña de ese nombre.
4. **Start Simulation**.

---

## Librerías

| Práctica | Qué hay que instalar |
|---|---|
| Reto 1, Tareas 1, 2 | nada |
| Tarea 3 — física | **nada** |
| Tarea 4 — opción C, física | **nada** |
| Tarea 5 — la placa | **nada** |
| Tarea 5 — Python | `pip install pyserial` |
| Tarea 5 — C# | `System.IO.Ports` (ya declarado en el `.csproj`) |
| Tarea 3 — simulador | `Servo` |
| Tarea 4 — simulador | `DHT sensor library` + `Adafruit Unified Sensor` |

---

## Videos de referencia

Comprobados uno por uno el 7 de agosto de 2026: existen, son públicos y tratan el tema.

**Para empezar**
- [Arduino desde cero — Cap. 3: entradas/salidas con LED](https://www.youtube.com/watch?v=BWhup75svIk) ← para el Reto 1
- [Cap. 55: buzzers activo y pasivo](https://www.youtube.com/watch?v=UQsixwoX5EQ) ← antes de las tareas 1 y 2

**Tarea 1 — S.O.S. en Morse**
- [ARDUINO UNO — Código Morse SOS — LED](https://www.youtube.com/watch?v=vNqjuyrh7M8)
- [LED and Buzzer S.O.S. in Morse Code](https://www.youtube.com/watch?v=ACfUOOKZCHs) · inglés

**Tarea 2 — Mario Bros**
- [Melodía de Super Mario Bros con Buzzer](https://www.youtube.com/watch?v=tJgaZJyQqyw)
- [Super Mario song with buzzer, LED and Arduino](https://www.youtube.com/watch?v=urTC2JcGJhs) ← el único con LED

**Tarea 3 — Distancia**
- [Cap. 5: HC-SR04](https://www.youtube.com/watch?v=mlw3APOUt8U) ← el mejor para el sensor
- [Alarma de proximidad con ultrasónico, LEDs y buzzer](https://www.youtube.com/watch?v=SFzbHsiydv4)
- [if, else, for, while, switch en Arduino](https://www.youtube.com/watch?v=UGTgpUH9xW8)

**Tarea 4 — Sensor de luz LDR**
- [Cap. 14: sensor crepuscular por LDR](https://www.youtube.com/watch?v=d3zcjfjqFxE)
- [Cómo utilizar un sensor de luz LDR](https://www.youtube.com/watch?v=h9wGZssIBOM)

**Tarea 5 — Arduino con Python**
- [PYTHON ARDUINO SERIAL — pyserial](https://www.youtube.com/watch?v=RRpEXRyjYUs) ← empieza aquí
- [Comunicación serial y registro de datos](https://www.youtube.com/watch?v=EV2kFHPJZy8)

---

## Qué falta

Está en [`MATRIZ_CUMPLIMIENTO.md`](MATRIZ_CUMPLIMIENTO.md), §6 — con el estado real de cada
práctica y de cada video.
