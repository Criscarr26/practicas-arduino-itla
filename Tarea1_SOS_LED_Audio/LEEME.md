# Señal S.O.S. con luz LED y audio

**Tarea 1era** · Inteligencia Artificial e Internet de las Cosas — 2026-C-2
Cristian Carrera — Matrícula 2024-1932 · Prof. Luis Bessewell Feliz

---

## Qué hace

Emite la señal **S.O.S. en código Morse** por dos medios a la vez: un LED y un zumbador.
Tres puntos, tres rayas, tres puntos, en bucle.

La luz y el sonido van sincronizados a propósito: se encienden y se apagan en la misma función,
en líneas consecutivas, para que no puedan desfasarse. Las duraciones salen del estándar Morse,
donde todo es múltiplo de lo que dura un punto.

## Qué hay en esta carpeta

| Archivo | Qué es |
|---|---|
| `Tarea1_SOS_LED_Audio.ino` | El código, con la investigación previa en el encabezado |
| `diagram.json` | El montaje, por si quiere verlo en [wokwi.com](https://wokwi.com) |

## Los videos

| Video | Duración | Qué muestra |
|---|---|---|
| `Tarea1_demostracion.mp4` | 0:33 | El circuito funcionando |
| `Tarea1_explicacion.mp4` | 5:23 | Yo explicando el código |

## Cómo ejecutarlo

Abrir el `.ino` en el Arduino IDE, seleccionar **Arduino UNO** y el puerto, y pulsar **Subir**.

**No necesita instalar ninguna librería.**

**Montaje:** LED en el pin 13 con resistencia de 220 Ω, zumbador en el pin 8.

---

## Declaración de uso de inteligencia artificial

Usé asistencia de IA (Claude) para depurar, documentar y estructurar el código. Son míos el
montaje físico, la calibración, las pruebas sobre la placa y la explicación grabada en el video.
La declaración aparece también en la cabecera del código.
