# Medidor de ausencia de luz — opción C

**Tarea 4ta** · Inteligencia Artificial e Internet de las Cosas — 2026-C-2
Cristian Carrera — Matrícula 2024-1932 · Prof. Luis Bessewell Feliz

---

## Qué hace

De las cuatro opciones del enunciado elegí la **C**: *«solo emitir el sonido del S.O.S. cuando
no exista luz en el entorno»*.

Mide la luz con una **fotorresistencia** en divisor de tensión y emite el S.O.S. en Morse
mientras haya oscuridad. Usa **dos umbrales** en vez de uno —histéresis— para que la alarma no
entre y salga varias veces por segundo cuando la lectura baila en la frontera.

No usa `delay()` ni ninguna librería: el S.O.S. está escrito como máquina de estados, para poder
seguir midiendo la luz mientras suena y callarse en el acto al encenderse una lámpara.

## Qué hay en esta carpeta

| Archivo | Qué es |
|---|---|
| `Tarea4_Ausencia_Luz.ino` | **La entrega.** Corre en la placa real, sin librerías |

## Los videos

| Video | Duración | Qué muestra |
|---|---|---|
| `Tarea4_video.mp4` | 8:47 | El circuito funcionando y yo explicando el código |

## Cómo ejecutarlo

Abrir el `.ino` en el Arduino IDE, seleccionar **Arduino UNO** y el puerto, y pulsar **Subir**.

**No necesita instalar ninguna librería.**

**Montaje:** LDR en A0 con resistencia de 10 kΩ a GND, zumbador en el pin 9, LED externo en el
pin 8, y el LED de la placa en el 13.

### Sobre los umbrales

No los copié de ningún tutorial. El programa imprime por el monitor serie el **mínimo y el
máximo** que ha visto desde que arrancó, y de ahí salieron: tapando y destapando la LDR en el
sitio donde se prueba. Los números de una fotorresistencia dependen del modelo, de la
resistencia fija y de la luz del lugar.

> La carpeta `Tarea4_Humedad_Temp_LED/` del material de apoyo resuelve las cuatro opciones A–D
> en simulador. Va como aporte adicional, no como la respuesta al enunciado.

---

## Declaración de uso de inteligencia artificial

Usé asistencia de IA (Claude) para depurar, documentar y estructurar el código. Son míos el
montaje físico, la calibración, las pruebas sobre la placa y la explicación grabada en el video.
La declaración aparece también en la cabecera del código.
