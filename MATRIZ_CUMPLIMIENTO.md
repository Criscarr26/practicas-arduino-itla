# Mapa maestro de entregas

**Cristian Carrera — 2024-1932** · IA e IoT 2026-C-2 · Prof. Luis Bessewell Feliz
Revisión: **18 de agosto de 2026, 23:00**

---

## 1. Estado de las cinco prácticas

| # | Práctica | Código | Funciona | Lo explico | Estado |
|---|---|---|---|---|---|
| 1 | S.O.S. con LED y audio | ✅ | ✅ 0:33 | ✅ 5:23 | ✅ |
| 2 | Música Mario Bros | ✅ | ✅ 4:43 (un solo video) | ✅ | ✅ |
| 3 | Detector de distancia | ✅ | ✅ 1:01 | ✅ 13:21 | ✅ |
| 4 | Ausencia de luz (opción C) | ✅ | ✅ 8:47 (un solo video) | ✅ | ✅ |
| 5 | Lectura desde otro lenguaje | ✅ | ✅ 8:18 (un solo video) | ✅ | ✅ |

**Las cinco tienen las dos evidencias que el profesor exigió:** que funciona físicamente, y que
el estudiante lo explica con sus palabras.

| Práctica | Videos en la carpeta compartida |
|---|---|
| 1 | `Tarea1_explicacion.mp4` (5:23) · `Tarea1_demostracion.mp4` (0:33) |
| 2 | `Tarea2_video.mp4` (4:43) |
| 3 | `Tarea3_explicacion.mp4` (13:21) · `Tarea3_demostracion.mp4` (1:01) |
| 4 | `Tarea4_video.mp4` (8:47) |
| 5 | `Tarea5_video.mp4` (8:18) |

> Los videos **no están en este repositorio** — van en la carpeta compartida, que es el canal
> que el profesor fijó. El `.gitignore` los excluye a propósito.

---

## 2. Lo que el profesor fijó en clase (Semana 14, 14 de agosto)

- **Nada de código en Moodle.** La plataforma puede tomar un `.ino` por virus. Va todo a una
  **carpeta compartida**, con una subcarpeta por práctica que contenga el código **y** el video.
  En Moodle solo se pega el enlace, indicando qué práctica es.
- **Dos evidencias por práctica:** que funciona físicamente, y que tú lo explicas. Textual:
  *«ver que funciona físicamente y ver que tú me estás mostrando y explicando lo que
  desarrollaste»*.
- **La IA se acepta si la declaras** y sabes explicar lo entregado. Declarada en la cabecera de
  los diez archivos fuente y en el README.
- **Sustituir un componente se acepta si se negocia antes.** Autorizó cambiar el motor de la
  práctica 3.
- **Confirmar el envío.** En borrador la tarea es editable pero **no cuenta como entregada**.
- Consejo: primero cumple exactamente lo pedido, y encima agrega tu aporte; no lo sustituyas.

| Clase | Grabación | Transcript |
|---|---|---|
| Semana 13 | `Treceava Semana…20260807.mp4` (395 MB, Descargas) | ❌ **NO DISPONIBLE** |
| Semana 14 | `Catorceava Semana…20260814.mp4` (292 MB, Descargas) | ✅ |

> ⚠️ Falta el transcript de la Semana 13. No se puede descartar que asignara algo no cubierto
> aquí. El transcriptor de `tools/transcriptor.html` puede procesar la grabación.

---

## 3. Qué carpeta va a cada entrega

| En el aula virtual | Carpeta | Valor |
|---|---|---|
| **Reto 1era** — Encender y Apagar LED | `Reto1_Encender_Apagar_LED/` | — |
| **Tarea 1era** — S.O.S con LED y audio | `Tarea1_SOS_LED_Audio/` | — |
| **Tarea 2da** — Música Mario Bros | `Tarea2_Mario_LED_Audio/` | — |
| **Tarea 3era** — Detector de Distancia | `Tarea3_Distancia_Fisico/` | 8 pts |
| **Tarea 4ta** — Temperatura / Humedad / Luz | `Tarea4_Ausencia_Luz/` | 8 pts |
| **Tarea 5ta** — Lectura vía otro lenguaje | `Tarea5_Arduino_Python/` | 8 pts |

**Aportes adicionales**, dentro de la carpeta de su práctica y presentados como extra:
`Tarea3_Distancia_Motor/` (versión con servo, simulador) y `Tarea4_Humedad_Temp_LED/`
(las cuatro opciones A–D, simulador).

---

## 4. Notas por práctica

**Tarea 3 — el solape de rangos.** El enunciado da los criterios en porcentajes de 400 cm y
**se pisan**: el criterio 1 (380–20 cm) contiene enteros al 3 y al 4. El análisis y la regla
aplicada —*manda el criterio más cercano*— están en el encabezado del sketch, junto con la
traducción de cada criterio al montaje sin motor que el profesor autorizó.

> El video explicativo es del **16 de agosto** y el segundo zumbador se añadió el **18**. El
> demo sí muestra los dos funcionando.

**Tarea 4 — se eligió la opción C** de las cuatro que ofrecía el enunciado: *«solo emitir el
sonido del S.O.S. cuando no exista luz en el entorno»*. Sobre placa real, sin librerías.

**Tarea 5 — dos lectores, un solo sketch.** La placa publica CSV por el puerto; se lee desde
Python y desde C# (Visual Studio 2022). El `.ino` no cambió ni una línea entre uno y otro, y ese
es justamente el punto. La versión anterior por MQTT quedó archivada en
`_Material_de_apoyo/Tarea5_versiones_anteriores/`, junto con el video que la explicaba.

---

## 5. Lo que falta, y es tuyo

| # | Qué | Por qué no puedo hacerlo yo |
|---|---|---|
| 1 | **Crear la carpeta compartida** y subir las cinco subcarpetas | Requiere tu cuenta |
| 2 | **Pegar el enlace** en cada tarea de Moodle, diciendo qué práctica es | Requiere tu sesión |
| 3 | **Confirmar el envío** — en borrador no cuenta | Requiere tu sesión |
| 4 | Decidir si mencionas el segundo zumbador de la Tarea 3 | Criterio tuyo |

**Antes de subir:** si abriste Visual Studio o corriste el lector de Python, borra `bin/`,
`obj/`, `.vs/` y `__pycache__/` de la carpeta de la Tarea 5. Se regeneran solos y no pintan nada
en una carpeta que va a abrir el profesor.

---

## 6. Qué NO hay que tocar

Verificado práctica por práctica contra el enunciado y contra lo que dijo en clase:

- La lógica de las seis prácticas cumple lo que pide cada enunciado.
- Los montajes coinciden con lo que declara cada `diagram.json`.
- Las prácticas 3, 4 y 5 no llaman a `delay()` — usan multitarea cooperativa con `millis()`.
  (Matiz honesto: en las 3 y 5, `pulseIn()` sí bloquea hasta 25 ms esperando el eco.)
- La histéresis de la Tarea 4 y el filtro de mediana de la Tarea 3 están bien resueltos.
- Los diez archivos fuente declaran el uso de IA en su cabecera.
