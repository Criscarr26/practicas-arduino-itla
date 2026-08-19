# Detector de distancia

**Tarea 3era** · Inteligencia Artificial e Internet de las Cosas — 2026-C-2
Cristian Carrera — Matrícula 2024-1932 · Prof. Luis Bessewell Feliz

---

## Qué hace

Mide la distancia con un **HC-SR04** y avisa con dos zumbadores que dicen cosas distintas:

- El del **pin 9** pulsa más rápido cuanto más cerca está el obstáculo.
- El del **pin 8** suena continuo solo cuando la máquina debe detenerse.

La medida es la **mediana de tres lecturas**, para que un eco perdido no dispare una falsa
alarma. No usa `delay()`: todo va con multitarea cooperativa sobre `millis()`, porque hay que
atender la medición y el sonido a ritmos distintos.

## Qué hay en esta carpeta

| Archivo | Qué es |
|---|---|
| `Tarea3_Distancia_Fisico.ino` | **La entrega.** Corre en la placa real |
| `Tarea3_Distancia_Motor/` | Aporte adicional: la versión con servomotor, para simulador |

## Los videos

| Video | Duración | Qué muestra |
|---|---|---|
| `Tarea3_demostracion.mp4` | 1:01 | El circuito funcionando |
| `Tarea3_explicacion.mp4` | 13:21 | Yo explicando el código |

## Cómo ejecutarlo

Abrir el `.ino` en el Arduino IDE, seleccionar **Arduino UNO** y el puerto, y pulsar **Subir**.

**No necesita instalar ninguna librería.**

### Sobre el motor

Usted me autorizó sustituirlo, porque lo que se evalúa es el objetivo —medir distancia y decidir
si algo continúa o se detiene—, no el componente. Ese papel lo cumple aquí el estado
EN MARCHA / DETENIDA, que se oye en los zumbadores. La versión con servomotor va incluida como
aporte adicional, en la subcarpeta.

### Sobre el solape de rangos

Los cuatro criterios del enunciado, pasados a centímetros sobre los 400 cm del sensor, **se
pisan entre sí**: el criterio 1 (380–20 cm) contiene enteros al 3 y al 4. El encabezado del
sketch trae el análisis completo, la regla que apliqué para resolverlo —*manda el criterio más
cercano*— y cómo se traduce cada criterio a este montaje.

### Un detalle del hardware

En el UNO, `tone()` solo puede sonar en un pin a la vez, porque usa un único temporizador. Por
eso los dos zumbadores no pueden ser los dos pasivos: uno es activo y el otro pasivo.

---

## Declaración de uso de inteligencia artificial

Usé asistencia de IA (Claude) para depurar, documentar y estructurar el código. Son míos el
montaje físico, la calibración, las pruebas sobre la placa y la explicación grabada en el video.
La declaración aparece también en la cabecera del código.
