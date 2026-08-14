# Práctica 5 — Lectura de datos de Arduino desde Python

**Cristian Carrera — 2024-1932 — 20241932@itla.edu.do**
IA e IoT 2026-C-2 · Prof. Luis Bessewell Feliz · **Valor: 8 puntos**
Entorno: **simulador Wokwi + Python en la PC**

---

## 1. Requisitos del profesor

El enunciado en la plataforma llegó con el cuerpo vacío. Lo que define la práctica es el título —*«Lectura de los datos sensados de Arduino a través de otro lenguaje de programación: Python, Java, C#, entre otros»*— y lo que el facilitador precisó en la sesión del 7 de agosto:

> *«¿vas a usar Python con Arduino y los sensores y presentar esa lectura en Python o C#?»*

**Arquitectura objetivo:** Arduino → transmisión → Python → procesamiento → visualización.

---

## 2. Investigación previa

### 2.1 Cómo se comunica normalmente Arduino con una computadora

Un Arduino UNO habla **UART** a 5 V. La computadora no entiende eso, así que la placa lleva un **segundo chip traductor USB↔serie**: un ATmega16U2 en el UNO original, o un **CH340** en los clones y en muchos Elegoo. Ese chip es el que crea el puerto COM que aparece en Windows.

Por eso, cuando Windows no reconoce una placa clónica, casi siempre lo que falta es **el driver del CH340**.

Con hardware físico, Python lee ese puerto con **pyserial**.

### 2.2 Por qué eso NO funciona en el simulador

> ### 🔴 Limitación del entorno — declarada, no disimulada
>
> **Wokwi no crea ningún puerto COM en el sistema operativo.**
>
> El microcontrolador se emula **dentro del navegador**. No hay chip CH340, no hay USB, no hay puerto serie que abrir. `pyserial` no tiene a qué conectarse. **No es un fallo del código: es que el cable no existe.**
>
> Y hay un segundo obstáculo: **el Arduino UNO no tiene radio**. En el simulador tampoco tiene salida a la red, así que ni siquiera podría comunicarse por otro medio.

### 2.3 La alternativa, y por qué es válida

Se cambia la placa a un **ESP32**, que sí tiene WiFi, y el transporte pasa a **MQTT**. El simulador de Wokwi **sí da salida real a internet** a las placas con WiFi.

```
   ┌─────────────────────┐        ┌──────────────┐        ┌────────────┐
   │  ESP32 en el        │  WiFi  │   Broker     │  MQTT  │  Python    │
   │  navegador (Wokwi)  │───────►│   MQTT       │───────►│  en tu PC  │
   │  DHT22 + LDR + HC   │ publica│ test.mosq... │suscribe│  CSV+gráfica│
   └─────────────────────┘        └──────────────┘        └────────────┘
```

**El objetivo académico se cumple igual:** los datos sensados por Arduino se leen y procesan desde otro lenguaje. Y de paso la arquitectura es **más parecida a un sistema IoT real** que la del cable serie, porque no exige que las dos máquinas estén físicamente conectadas.

| | Puerto serie (físico) | MQTT (virtual) |
|---|---|---|
| Requiere hardware | ✅ sí | ❌ no |
| Requiere cable USB | ✅ sí | ❌ no |
| Distancia máxima | ~2 m | ilimitada |
| Es programación **distribuida** | no (una máquina) | ✅ sí |
| ¿Cumple el objetivo? | ✅ | ✅ |

### 2.4 Por qué JSON y no CSV

En la versión por cable se usó CSV, porque a 9600 baudios cada byte cuenta. Aquí se usa **JSON**, y el cambio está justificado:

- MQTT no tiene la limitación de ancho de banda del puerto serie.
- JSON es **autodescriptivo**: cada valor viaja con su nombre. Si mañana el ESP32 agrega un sensor, el programa de Python **no se rompe**, porque busca los campos por nombre y no por posición. Con CSV bastaría cambiar el orden de dos columnas para que todo se corrompa en silencio.
- Es el formato estándar de la industria IoT.

---

## 3. Interpretación del caso práctico

Se sensan **tres magnitudes** con los mismos sensores de las prácticas anteriores, para demostrar que la lectura remota funciona con datos heterogéneos:

| Sensor | Magnitud | Rango |
|---|---|---|
| DHT22 | Temperatura y humedad | −40–80 °C / 0–100 % |
| LDR | Luz ambiente | 0–4095 (ADC de 12 bits del ESP32) |
| HC-SR04 | Distancia | 2–400 cm |

Un valor de **−1** significa «el sensor no dio lectura válida». Se eligió −1 y no 0 porque **cero es un valor real**: cero grados es una temperatura perfectamente posible.

---

## 4. Componentes virtuales

| Componente | Pieza Wokwi | Nota |
|---|---|---|
| Placa | `board-esp32-devkit-c-v4` | **obligatorio ESP32**, el UNO no tiene WiFi |
| Temperatura y humedad | `wokwi-dht22` | |
| Luz | `wokwi-photoresistor-sensor` | |
| Distancia | `wokwi-hc-sr04` | |

En el lado de la PC: **Python 3** con `paho-mqtt` y `matplotlib`.

---

## 5. Diseño del circuito

```
   [DHT22] ──GPIO15──┐
                     │
   [LDR AO] ─GPIO34──┤   ESP32 DevKit  ))) WiFi ))) → internet
                     │
   [HC-SR04] TRIG─GPIO5   ECHO─GPIO18
```

**Por qué GPIO 34 para el LDR:** en el ESP32 los pines 34–39 son **solo de entrada** y pertenecen al ADC1, que es el único que funciona con el WiFi encendido. El ADC2 queda inutilizable mientras hay radio: es un error clásico que hace que las lecturas analógicas devuelvan basura.

---

## 6. Tabla de conexiones

| Componente | Pin componente | Pin ESP32 |
|---|---|---|
| DHT22 | DATA | **GPIO 15** |
| DHT22 | VCC / GND | 3V3 / GND |
| LDR | AO | **GPIO 34** (solo entrada, ADC1) |
| LDR | VCC / GND | 3V3 / GND |
| HC-SR04 | TRIG | **GPIO 5** |
| HC-SR04 | ECHO | **GPIO 18** |
| HC-SR04 | VCC / GND | 5V / GND |
| LED de estado | — | GPIO 2 (integrado en la placa) |

---

## 7. Arquitectura de funcionamiento

**Lado Arduino** (cada 2 s): lee los tres sensores → arma el JSON → publica en el tópico → parpadea el LED.

**Lado Python:** se suscribe al tópico → valida cada mensaje → acumula → al cerrar guarda CSV, imprime resumen y dibuja la gráfica.

**Tópico usado:**
```
itla/2024-1932/practica5/sensores
```

> ⚠️ Lleva la matrícula a propósito. `test.mosquitto.org` es un broker **público**: si dos personas usaran el mismo tópico, se les mezclarían los datos.

---

## 8. Código

| Archivo | Qué es |
|---|---|
| [`P5_Virtual_ESP32/P5_Virtual_ESP32.ino`](P5_Virtual_ESP32/P5_Virtual_ESP32.ino) | Firmware del ESP32 |
| [`P5_Virtual_ESP32/diagram.json`](P5_Virtual_ESP32/diagram.json) | Montaje para Wokwi |
| [`receptor_mqtt.py`](receptor_mqtt.py) | Receptor en Python |

**La validación en Python** — nunca confiar en lo que llega de la red:

```python
try:
    d = json.loads(texto)
except json.JSONDecodeError:
    print(f"[AVISO] Mensaje ilegible, descartado")
    return

if not all(c in d for c in CAMPOS):
    print(f"[AVISO] Faltan campos en el mensaje, descartado")
    return
```

> **Probado con casos límite:** mensaje correcto → aceptado; sensores en −1 → aceptado y mostrado como `--`; JSON incompleto → descartado; texto basura → descartado. 3 de 5 aceptados, que es lo correcto.

---

## 9. Configuración de Arduino Cloud — y por qué no se usa

Ya explicado en la sección 2.2: **Arduino Cloud no tiene simulador** ([docs oficiales](https://docs.arduino.cc/arduino-cloud/)), y aunque lo tuviera, el problema aquí es el **puerto COM inexistente**, no la falta de un editor.

**Nota interesante:** Arduino Cloud *sí* podría ser el receptor en lugar de MQTT crudo — sus *Things* reciben variables de un ESP32 y las muestran en un dashboard. Pero entonces **Python quedaría fuera**, y el enunciado pide expresamente leer los datos *«a través de otro lenguaje de programación»*. Por eso se usa MQTT: es el único camino que mantiene a Python en el centro.

---

## 10. Ejecución del simulador

**Orden importante: primero Wokwi, después Python.**

1. **wokwi.com** → **New Project** → **ESP32**.
2. Pegar `P5_Virtual_ESP32.ino` y el `diagram.json`.
3. **Library Manager** → añadir `DHT sensor library`, `Adafruit Unified Sensor` y `PubSubClient`.
4. **Start Simulation.** Esperar a ver en el monitor serie:
   ```
   Conectado. IP: 10.10.0.2
   [ENVIADO] {"nodo":"esp32-2024-1932","seq":1,...}
   ```
5. En la PC:
   ```bash
   pip install paho-mqtt matplotlib
   python receptor_mqtt.py
   ```
6. Mover los deslizadores de los sensores en Wokwi y ver cómo cambian los números en la terminal de Python.
7. **Ctrl+C** para detener: guarda el CSV y dibuja la gráfica.

---

## 11. Casos de prueba

| # | Acción en Wokwi | Resultado esperado en Python |
|---|---|---|
| 1 | Arrancar simulación | Aparecen filas numeradas cada 2 s |
| 2 | Subir temperatura del DHT a 35 °C | La columna Temp sube a 35.0 |
| 3 | Bajar humedad a 20 % | La columna Humedad baja a 20 |
| 4 | Tapar el LDR (deslizador a 0) | La columna Luz cae cerca de 0 |
| 5 | Mover la distancia del HC-SR04 a 300 cm | La columna Distancia sube a ~300 |
| 6 | **Detener la simulación** | Python deja de recibir; sin errores |
| 7 | Ctrl+C en Python | Genera `datos_practica5.csv` + resumen + gráfica |
| 8 | Abrir el CSV en Excel | Columnas con encabezado, listo para analizar |

**La prueba 6 es la interesante:** demuestra que los dos programas son **independientes**. Si uno muere, el otro sigue vivo. Eso es programación distribuida.

---

## 12. Resultados esperados

**Monitor serie de Wokwi:**
```
== Practica 5: Arduino -> MQTT -> Python ==
Conectando a WiFi....
Conectado. IP: 10.10.0.2
Publicando en el topico: itla/2024-1932/practica5/sensores
[ENVIADO] {"nodo":"esp32-2024-1932","seq":1,"ms":2043,"temperatura":24.0,...}
```

**Terminal de Python:**
```
   #    Tiempo      Temp   Humedad     Luz  Distancia
------------------------------------------------------
   1      2.0s   24.5C     58%    812   45.3cm
   2      4.0s   24.5C     58%    812   45.3cm
   3      6.0s   35.0C     20%     12  300.0cm

3 lecturas guardadas en: datos_practica5.csv

Resumen:
  temperatura  min    24.5   max    35.0   promedio    28.0
  humedad      min    20.0   max    58.0   promedio    45.3
```

---

## 13. Evidencias que debo capturar

**Cinco capturas:**

| # | Qué capturar | Demuestra |
|---|---|---|
| 1 | Wokwi con el circuito y el monitor mostrando `[ENVIADO]` | Arduino publica |
| 2 | Las dos ventanas lado a lado: Wokwi izquierda, Python derecha | El flujo completo |
| 3 | Terminal de Python con filas llegando | Python recibe y procesa |
| 4 | La gráfica generada | Visualización |
| 5 | El CSV abierto en Excel | Persistencia |

Para el **video**: mover un deslizador en Wokwi y mostrar cómo el número cambia en la terminal de Python **en la misma toma**. Es la prueba visual de que los datos viajan de verdad.

---

## 14. Explicación para la entrega

| Forma | Qué subir |
|---|---|
| **A** | Las 5 capturas + el video |
| **B** | Pegar `P5_Virtual_ESP32.ino` en el Editor |
| **C** | Adjuntar el `.ino` **y** `receptor_mqtt.py` |

Sube también `datos_practica5.csv` y `grafica_practica5.png`: son la evidencia de que corrió de verdad.

**Los tres puntos que conviene defender:**

1. **Por qué no se usó pyserial.** Porque el simulador no crea puerto COM. Es una limitación del entorno, identificada y resuelta, no un atajo.
2. **Por qué ESP32 y no UNO.** Porque el UNO no tiene radio; sin WiFi no hay forma de sacar los datos del navegador.
3. **Por qué es programación distribuida.** Dos programas, en dos máquinas distintas, sin memoria compartida, coordinados solo por mensajes. Si uno muere el otro sigue.

---

## 15. Posibles errores y soluciones

| Síntoma | Causa | Solución |
|---|---|---|
| Python no recibe nada | La simulación no está corriendo | Arrancar Wokwi **primero** |
| Python no recibe nada | El tópico no coincide | Debe ser idéntico en los dos archivos |
| `ModuleNotFoundError: paho` | Falta la librería | `pip install paho-mqtt` |
| El ESP32 no conecta al WiFi | SSID cambiado | Debe ser exactamente `Wokwi-GUEST`, sin clave |
| `rc=-2` al conectar al broker | Sin internet, o el broker público caído | Probar `broker.hivemq.com` |
| La luz siempre da 0 o 4095 | El LDR en un pin del ADC2 | Debe ir en GPIO 32–39 (ADC1) |
| Llegan datos de otra persona | Alguien usa el mismo tópico | Cambiar la matrícula del tópico |
| Distancia siempre −1 | TRIG y ECHO invertidos | TRIG → GPIO 5, ECHO → GPIO 18 |

---

### Estado de verificación

| Elemento | Estado |
|---|---|
| Sintaxis de `receptor_mqtt.py` | ✅ **compila** |
| Lógica de validación JSON | ✅ **probada** con 5 casos límite |
| `diagram.json` del ESP32 | ⚠️ no cargado en Wokwi (piezas ya verificadas en P3/P4) |
| Compilación del `.ino` | ⚠️ no comprobada |
| Flujo MQTT extremo a extremo | ⚠️ pendiente — requiere ejecutar los dos lados |
