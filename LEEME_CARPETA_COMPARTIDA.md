# Prácticas de laboratorio — Arduino

**Inteligencia Artificial e Internet de las Cosas — 2026-C-2**
Instituto Tecnológico de Las Américas (ITLA)

| | |
|---|---|
| **Facilitador** | Luis Bessewell Feliz |
| **Sustentante** | Cristian Carrera — Matrícula 2024-1932 |
| **Correo** | 20241932@itla.edu.do |
| **Placa** | Elegoo UNO R3 (Super Starter Kit) |
| **Repositorio** | https://github.com/Criscarr26/practicas-arduino-itla |

---

## Qué hay aquí

Una subcarpeta por práctica. Dentro de cada una está **el código y el video**.

| Carpeta | Entrega del aula virtual | Qué hace |
|---|---|---|
| `Tarea1_SOS_LED_Audio/` | Tarea 1era | S.O.S. en código Morse por LED y zumbador a la vez |
| `Tarea2_Mario_LED_Audio/` | Tarea 2da | Melodía de Super Mario Bros con luz sincronizada |
| `Tarea3_Distancia_Fisico/` | Tarea 3era | HC-SR04 con aviso sonoro proporcional a la distancia |
| `Tarea4_Ausencia_Luz/` | Tarea 4ta | LDR que emite S.O.S. mientras no haya luz — **opción C** |
| `Tarea5_Arduino_Python/` | Tarea 5ta | El sensor de la Tarea 3 leído desde **Python y C#** por el puerto serie |

---

## Los videos

Cada práctica trae las dos evidencias: **que funciona** y **que la explico**.

| Práctica | Video | Duración |
|---|---|---|
| 1 | `Tarea1_explicacion.mp4` | 5:23 |
| 1 | `Tarea1_demostracion.mp4` | 0:33 |
| 2 | `Tarea2_video.mp4` — explicación y demostración | 4:43 |
| 3 | `Tarea3_explicacion.mp4` | 13:21 |
| 3 | `Tarea3_demostracion.mp4` | 1:01 |
| 4 | `Tarea4_video.mp4` — explicación y demostración | 8:47 |
| 5 | `Tarea5_video.mp4` — explicación y demostración | 8:18 |

---

## Declaración de uso de inteligencia artificial

Usé **asistencia de IA (Claude)** para depurar, documentar y estructurar el código. Son míos el
montaje físico, la calibración de los umbrales, las pruebas sobre la placa y la explicación
grabada en cada video.

Lo declaro porque usted dijo en clase que el uso de IA es aceptable siempre que se declare y se
sepa explicar lo entregado. La declaración aparece también en la cabecera de cada archivo de
código.

---

## Tres notas sobre lo entregado

**Tarea 3 — el motor.** Usted me autorizó sustituirlo, porque lo que se evalúa es el objetivo
—medir distancia y decidir si algo continúa o se detiene—, no el componente. En la placa real
ese papel lo cumplen dos zumbadores: uno pulsa más rápido cuanto más cerca está el obstáculo, y
otro suena continuo cuando hay que detenerse. La versión con servomotor está incluida como
aporte adicional, en `Tarea3_Distancia_Motor/`.

El encabezado del sketch incluye el **análisis del solape de rangos** del enunciado: los
criterios se pisan entre sí, y ahí está la regla que apliqué para resolverlo.

**Tarea 4 — elegí la opción C**, de las cuatro que ofrecía el enunciado: *«solo emitir el sonido
del S.O.S. cuando no exista luz en el entorno»*. Corre sobre la placa real y no usa ninguna
librería. La carpeta `Tarea4_Humedad_Temp_LED/` resuelve las cuatro opciones en simulador y va
como aporte adicional, no como la respuesta al enunciado.

**Tarea 5 — la resolví dos veces**, en Python y en C#, sin cambiar una sola línea del programa
de Arduino. La placa publica sus lecturas en CSV por el puerto serie y no le importa quién la
escuche. Los archivos `datos_distancia_*.csv` y la gráfica son resultados reales de una sesión
de captura.

---

## Cómo ejecutar cada práctica

**Prácticas 1 a 5, en la placa:** abrir el `.ino` en el Arduino IDE, seleccionar *Arduino UNO* y
el puerto, y pulsar **Subir**. Ninguna necesita instalar librerías.

**Práctica 5, además, del lado de la computadora:**

```
python lector_distancia.py
```

o abrir `LectorDistancia.sln` en Visual Studio 2022 y pulsar **F5**.

> El puerto serie admite un solo programa a la vez. Si el Monitor Serie del IDE de Arduino está
> abierto, el lector no puede conectarse.

**Los archivos `diagram.json`** son el montaje para [wokwi.com](https://wokwi.com), por si quiere
ver el circuito sin armarlo.
