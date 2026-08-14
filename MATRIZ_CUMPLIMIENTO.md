# Matriz de cumplimiento — Prácticas 3, 4 y 5

**Cristian Carrera — 2024-1932** · IA e IoT 2026-C-2 · Prof. Luis Bessewell Feliz
Revisión: 14 de agosto de 2026

**Criterio de esta matriz:** no se marca nada como completado solo porque exista código. Cada fila indica si está **implementado**, si está **verificado** (yo lo comprobé), o si está **pendiente de ejecución** por el estudiante.

| Símbolo | Significado |
|---|---|
| ✅ | Implementado **y verificado** por mí |
| 🟡 | Implementado, **pendiente de que lo ejecutes** |
| 📄 | Justificado por escrito (limitación del entorno) |

---

## Práctica 3 — Detector de distancia (8 pts)

### Investigación previa

| Requisito | Estado | Evidencia |
|---|---|---|
| `if`, `for`, `while`, `switch` | ✅ | §2.1 del `.md` + las cuatro usadas en el `.ino` con comentario de por qué cada una |
| Distancia máxima del sensor | ✅ | §2.2: **400 cm**, con hoja de datos y fórmula deducida |
| Cómo funciona el sensor | ✅ | §2.2: eco 40 kHz, pulso 10 µs, `÷29.1` y `÷2` explicados |
| Sentencia para encender/apagar motor | ✅ | §2.3: `attach()`/`detach()` + alternativas L298N y stepper |
| ¿Arduino soporta hilos/tareas? | ✅ | §2.4: tabla secuencial/concurrente/paralelo/distribuido |

### Caso práctico

| Criterio del profesor | Estado | Evidencia |
|---|---|---|
| **1.** 95 %–5 %: motor ON + LED fijo | 🟡 | `case ZONA_LEJANA` — prueba 2 y 3 |
| **2.** Fuera de rango: todo apagado | 🟡 | `case FUERA_DE_RANGO` — pruebas 1 y 9 |
| **3.** 50 %–25 %: LED parpadea + buzzer sincronizado | 🟡 | `case ZONA_MEDIA` — prueba 4 |
| **4.** 26 %–4 %: parpadeo rápido + sonido particular | 🟡 | `case ZONA_CERCANA`, sirena 2 tonos — prueba 6 |
| Sincronización LED↔buzzer | ✅ | El `tone()` está **dentro del mismo `if`** que conmuta el LED |
| **Análisis del solape de rangos** | ✅ | §3 con diagrama; resolución «más cerca manda», justificada |
| Motor encendido desde el arranque | ✅ | `encenderMotor()` al final de `setup()` |

### Entorno

| Elemento | Estado | Nota |
|---|---|---|
| `diagram.json` válido en Wokwi | ✅ | **6 piezas, 0 errores**, pines confirmados contra el DOM |
| Compilación del sketch | 🟡 | Cola de Wokwi saturada durante la sesión |
| Casos de prueba ejecutados | 🟡 | 9 casos definidos, pendientes de correr |

---

## Práctica 4 — Temperatura, humedad, luz y display (8 pts)

### Investigación previa

| Requisito | Estado | Evidencia |
|---|---|---|
| `if`, `for`, `while`, `switch` | ✅ | §2.1 |
| Detector de ausencia de luz | ✅ | §2.3: LDR + divisor de tensión explicado y dibujado |
| Medidor de temperatura | ✅ | §2.2: termistor NTC, protocolo de un hilo, 40 bits |
| Medidor de humedad | ✅ | §2.2: sustrato capacitivo |
| Pantalla de mensajes | ✅ | §2.4: HD44780, modo 4 bits, RS/E, contraste obligatorio |
| ¿Hilos/tareas? | ✅ | §2.5 |

### Caso práctico

| Requisito | Estado | Evidencia |
|---|---|---|
| **A.** Temp normal → LED verde | 🟡 | `clasificarTemperatura()` — prueba 1 |
| **A.** Precaución → LED rojo + **zumbido continuo** | 🟡 | `case ZUMBIDO`, `tone()` sin duración — prueba 2 |
| **A.** Crítico → LED rojo + **S.O.S.** | 🟡 | `case SOS` + `PATRON_SOS[]` — prueba 3 |
| **B.** Humedad con los tres estados | 🟡 | `clasificarHumedad()` — pruebas 4 y 5 |
| **C.** S.O.S. **solo** sin luz | 🟡 | `tareaLeerLuz()` + prioridad 3 — prueba 6 |
| **D.** Display con info dinámica | 🟡 | `tareaPantalla()`, alterna 2 pantallas — todas las pruebas |
| **Umbrales justificados** | ✅ | §3.1: EPA/ASHRAE 55 para humedad (30–60 %); rangos clínicos para temperatura |
| Jerarquía entre las tres alarmas | ✅ | §3.2: un solo buzzer, prioridad explícita — prueba 7 |

### Entorno

| Elemento | Estado | Nota |
|---|---|---|
| `diagram.json` válido | ✅ | **7 piezas, 0 errores** |
| Conflicto de pines detectado | ✅ | El LCD ocupaba A0–A5, donde va el LDR. **Corregido** |
| Conflicto de temporizadores | ✅ | `tone()` usa Timer2 = PWM de pines 3 y 11 → RGB movido a 5, 6, 9 |
| Compilación | 🟡 | No comprobada |
| Casos de prueba | 🟡 | 8 definidos, pendientes |

---

## Práctica 5 — Arduino + Python (8 pts)

| Requisito | Estado | Evidencia |
|---|---|---|
| Leer datos sensados desde otro lenguaje | 🟡 | `receptor_mqtt.py` en Python |
| Arquitectura Arduino → transmisión → Python → visualización | ✅ | §2.3 con diagrama del flujo |
| **Mecanismo técnicamente realista** | ✅ | MQTT sobre WiFi; Wokwi sí da internet a las placas con radio |
| **Limitación identificada, no ocultada** | 📄 | §2.2: Wokwi no crea puerto COM → `pyserial` imposible |
| Justificación de la alternativa | ✅ | §2.3 con tabla comparativa serie vs MQTT |
| Procesamiento en Python | ✅ | Validación JSON **probada con 5 casos límite** |
| Visualización | 🟡 | CSV + resumen estadístico + gráfica matplotlib |
| Sintaxis del receptor | ✅ | `py_compile` sin errores |
| Flujo extremo a extremo | 🟡 | Requiere ejecutar los dos lados a la vez |

---

## Requisitos transversales del prompt

| Requisito | Estado | Nota |
|---|---|---|
| No asumir hardware físico | ✅ | Todo diseñado para simulador |
| Priorizar Arduino Cloud + simulación | 📄 | **Arduino Cloud no tiene simulador** — verificado contra [docs oficiales](https://docs.arduino.cc/arduino-cloud/). Sustituido por Wokwi, documentado en la §9 de cada práctica |
| No inventar componentes del simulador | ✅ | Las 13 piezas usadas fueron cargadas y verificadas |
| No inventar funciones de Arduino Cloud | ✅ | Se declara explícitamente lo que no tiene |
| No cambiar requisitos del profesor | ✅ | Los 4 criterios de distancia y las 4 opciones A–D, íntegros |
| Porcentajes implementados correctamente | ✅ | Derivados de 400 cm, no escritos a mano |
| Analizar rangos superpuestos | ✅ | §3 de la práctica 3, con diagrama |
| Justificar rangos de temperatura y humedad | ✅ | §3.1 de la práctica 4, con fuente |
| LED, buzzer, motor y display incluidos | ✅ | P3: LED+buzzer+motor · P4: LED RGB+buzzer+display |
| Investigar concurrencia real | ✅ | En las tres prácticas, con la tabla de 4 conceptos |
| Método de comunicación realista en P5 | ✅ | MQTT, con la limitación del COM explicada |
| Explicar limitaciones, no ocultarlas | ✅ | 3 limitaciones declaradas |
| No agregar complejidad innecesaria | ✅ | 5–6 capturas por práctica, no más |

---

## Las tres limitaciones declaradas

| # | Limitación | Cómo se resolvió |
|---|---|---|
| 1 | **Arduino Cloud no tiene simulador** | Wokwi. Ningún objetivo académico se pierde (tabla en §9) |
| 2 | **El UNO R3 no es compatible con Arduino IoT Cloud** | Solo el Cloud Editor sirve, y solo para compilar |
| 3 | **Wokwi no crea puerto COM** | P5 pasa de `pyserial` a MQTT con ESP32 |

---

## Lo que falta, y es tuyo

| Tarea | Prácticas |
|---|---|
| Cargar los proyectos en Wokwi y **compilar** | 3, 4, 5 |
| Ejecutar los **casos de prueba** | 9 + 8 + 8 = 25 casos |
| Tomar las **capturas** | 6 + 6 + 5 = 17 capturas |
| Grabar los **videos** | 3 |
| Ejecutar `receptor_mqtt.py` con Wokwi corriendo | 5 |

**Ninguna fila marcada 🟡 debería declararse cumplida ante el profesor hasta que la ejecutes.** Ahí está la diferencia entre «tiene código» y «funciona».

---

## Nota sobre el hardware

Este trabajo asume que **no tienes hardware**, según el planteamiento más reciente. Si el kit Elegoo sí está disponible, cambian dos cosas:

- **Prácticas 3 y 4:** funcionan igual en físico, solo cambia el DHT22 por DHT11 (`#define TIPO_DHT DHT11`).
- **Práctica 5:** con hardware **sí puedes usar `pyserial`**, que es la vía más directa y convincente. El archivo `lector_serial.py` de esta misma carpeta ya la implementa y está probado.
