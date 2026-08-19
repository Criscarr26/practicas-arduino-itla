# Mapa maestro de entregas

**Cristian Carrera — 2024-1932** · IA e IoT 2026-C-2 · Prof. Luis Bessewell Feliz
Revisión: **18 de agosto de 2026, 20:00**

> Esta revisión sustituye por completo a la del 14 de agosto, que partía de la premisa
> equivocada de que no había hardware. Las prácticas 3, 4 y 5 están resueltas **sobre la placa
> real**, y la práctica 5 se resolvió por **puerto serie**, no por MQTT.

| Símbolo | Significado |
|---|---|
| ✅ | Listo para entregar |
| 🟡 | Casi listo — le falta un detalle concreto |
| 🔴 | Falta trabajo |
| ⚠️ | Depende de una acción manual tuya |

---

## 1. Estado de cada práctica

| # | Práctica | Código | Video: funciona | Video: lo explicas | Estado |
|---|---|---|---|---|---|
| Reto 1 | Encender y apagar LED | ✅ | ❌ | ❌ | ⚠️ |
| Tarea 1 | S.O.S. con LED y audio | ✅ | ❌ | ❌ | ⚠️ |
| Tarea 2 | Mario Bros con LED y audio | ✅ | ❌ | ❌ | ⚠️ |
| Tarea 3 | Detector de distancia | ✅ | ✅ `Tarea_3.mp4` 1:01 | 🟡 `Reunión…20260816` 13:21 | 🟡 |
| Tarea 4 | Ausencia de luz (opción C) | ✅ | ✅ `Tarea_4.mp4` 8:47 | ✅ (el mismo) | ✅ |
| Tarea 5 | Lectura desde otro lenguaje | ✅ | 🔴 | 🔴 | 🔴 |

---

## 2. Clases, transcripts y videos

| Clase | Grabación | Transcript | Qué fijó | Tareas |
|---|---|---|---|---|
| **Semana 13** | `Treceava Semana…20260807.mp4` (395 MB, Descargas) | ❌ **NO DISPONIBLE** | — | — |
| **Semana 14** | `Catorceava Semana…20260814.mp4` (292 MB, Descargas) | ✅ | Criterios de entrega + planteamiento de la práctica 5 | 3, 4, 5 |

**De la Semana 14 salieron las reglas que gobiernan todo lo demás:**

- No subir código a Moodle. La plataforma puede tomar un `.ino` por virus. Va **carpeta
  compartida**, con subcarpeta por práctica (código + video dentro), y en Moodle solo el enlace.
- **Dos evidencias por práctica:** que funciona físicamente, y que tú lo explicas con tus
  palabras. Textual: *«ver que funciona físicamente y ver que tú me estás mostrando y
  explicando lo que desarrollaste»*.
- **La IA se acepta si la declaras** y sabes explicar lo entregado.
- **Sustituir un componente se acepta si se negocia antes.** Autorizó cambiar el motor de la
  práctica 3.
- Práctica 5, textual: *«que tú salgas de Arduino, que desde otro lenguaje leas por el puerto
  lo que él sensa»*, y *«tomen una de esas prácticas y conviértanla»*. Ejemplo de la máquina de
  coser: medir cuánto se detiene y qué tan rápido trabaja.
- **Estado borrador no cuenta.** Hay que confirmar el envío.

> ⚠️ **Falta el transcript de la Semana 13.** No se puede descartar que asignara algo que no
> esté cubierto aquí. La grabación existe; el transcriptor de `tools/transcriptor.html` puede
> procesarla.

---

## 3. Tarea 3 — Detector de distancia

**Dos versiones, a propósito:**

| Carpeta | Para qué | Cumple los 4 criterios |
|---|---|---|
| `Tarea3_Distancia_Fisico/` | La que se graba, sobre la placa | Reinterpretados y **declarados en el encabezado** |
| `Tarea3_Distancia_Motor/` | Simulador Wokwi, con servo | En su forma literal |

El enunciado da los criterios en porcentajes de 400 cm, y **se solapan**: el criterio 1
(380–20 cm) contiene enteros al 3 y al 4. El análisis del solape y la regla aplicada
—*manda el criterio más cercano*— están escritos en el encabezado del sketch físico.

**🟡 Lo que le falta:** el video explicativo es del **16 de agosto** y el **segundo zumbador se
añadió el 18** (commit `ed746fb`). Explicas una versión con un zumbador y entregas una con dos.
El demo de hoy sí muestra los dos funcionando.

> **Decisión tuya:** o grabas 30 segundos extra explicando el segundo zumbador, o lo mencionas
> al entregar. No es un error de código; es que el código mejoró después del video.

---

## 4. Tarea 4 — Opción C, ausencia de luz

El enunciado da cuatro opciones (A temperatura, B humedad, C luz, D cualquiera + display) y
pide **hacer una**. Se eligió la **C**: *«solo emitir el sonido del S.O.S. cuando no exista luz
en el entorno»*.

| Carpeta | Papel |
|---|---|
| `Tarea4_Ausencia_Luz/` | **La entrega.** Opción C sobre placa real. Sin librerías |
| `Tarea4_Humedad_Temp_LED/` | Aporte adicional: las cuatro opciones en simulador |

**✅ Está lista.** Código, calibración y video de 8:47 grabado el 18 de agosto.

> La segunda carpeta se presenta **como extra, nunca como la respuesta al enunciado**. El
> profesor dijo: primero cumple exactamente lo pedido, y encima agrega tu aporte.

---

## 5. Tarea 5 — Lectura desde otro lenguaje

Reutiliza el montaje de la Tarea 3 sin tocar un cable. La placa publica CSV por el puerto; del
otro lado hay **dos lectores**, y ese es el punto: el `.ino` no cambió ni una línea entre uno y
otro.

| Pieza | Estado |
|---|---|
| `Tarea5_Distancia_Serial/` — el sketch | ✅ |
| `lector_distancia.py` — Python | ✅ probado contra la placa |
| `LectorDistancia/` — C#, Visual Studio 2022 | ✅ probado contra la placa |

### 🔴 El video no sirve

`Tarea_5.mp4` son 15:16 grabados el **14 de agosto**. Ese día tu Tarea 5 era la versión
**ESP32 + MQTT en simulador**, que hoy está archivada en
`_Material_de_apoyo/Tarea5_versiones_anteriores/`. El código actual es del 18 de agosto.

**Ese video explica un trabajo que ya no entregas. Hay que regrabarlo.**

---

## 6. Lo que falta, y es tuyo

| # | Qué | Práctica | Por qué no puedo hacerlo yo |
|---|---|---|---|
| 1 | **Regrabar el video** | 5 | Tienes que salir tú explicando |
| 2 | **Crear la carpeta compartida** y subir código + video por subcarpeta | Todas | Requiere tu cuenta |
| 3 | **Pegar el enlace** en cada tarea de Moodle | Todas | Requiere tu sesión |
| 4 | **Confirmar el envío** — en borrador no cuenta | Todas | Requiere tu sesión |
| 5 | Decidir sobre los videos del Reto 1 y las Tareas 1 y 2 | Reto 1, 1, 2 | Ver abajo |
| 6 | Decidir sobre el segundo zumbador de la Tarea 3 | 3 | Ver §3 |

**Sobre el punto 5:** el Reto 1 y las Tareas 1 y 2 no tienen ningún video. El criterio de las
dos evidencias se fijó el **14 de agosto**; si esas tres las entregaste antes, probablemente se
rigen por las reglas anteriores. **Si aún no están enviadas, necesitan video.**

---

## 7. Qué NO hay que tocar

Verificado práctica por práctica contra el enunciado y contra lo que el profesor dijo en clase:

- La lógica de las seis prácticas cumple lo que pide cada enunciado.
- Los montajes coinciden con lo que declara cada `diagram.json`.
- Las prácticas 3, 4 y 5 no llaman a `delay()` — usan multitarea cooperativa con `millis()`.
  (Matiz honesto: en las 3 y 5, `pulseIn()` sí bloquea hasta 25 ms esperando el eco.)
- La histéresis de la Tarea 4 y el filtro de mediana de la Tarea 3 están bien resueltos.

**No se cambió el comportamiento de ningún programa en esta revisión.** Todo lo que se tocó
fueron comentarios, documentación, y un respaldo que el lector de Python prometía y no tenía.
