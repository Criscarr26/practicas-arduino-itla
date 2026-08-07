# Guion de grabación — Práctica 5: leer los datos de Arduino desde Python

**Duración objetivo: 5 minutos.** Vale 8 puntos. Lo que va en *cursiva* se lee en voz alta.

---

## Antes de grabar

1. Carga `P5_Arduino_Python.ino` en la placa desde el Arduino IDE.
2. **CIERRA el Monitor Serie del IDE.** El puerto solo admite un programa a la vez; si el IDE
   lo tiene abierto, Python dará «Acceso denegado». Es el fallo más común de esta práctica.
3. Instala las dependencias:
   ```bash
   pip install pyserial matplotlib
   ```
4. Coloca las dos ventanas lado a lado: el código a la izquierda, la terminal a la derecha.
5. Ten a mano una linterna o el flash del celular (para el LDR) y algo con que tapar el sensor.

---

## Parte 1 — Presentación y el porqué (45 s)

> *Buenas. Soy Cristian Carrera, matrícula 2024-1932, de Inteligencia Artificial e Internet de
> las Cosas con el profesor Luis Bessewell. Esta práctica consiste en leer los datos sensados
> por Arduino desde otro lenguaje: elegí Python.*

Y ahora la justificación, que es lo que da valor al video:

> *Y quiero empezar por el porqué, porque no es un capricho.*
>
> *Un microcontrolador es buenísimo midiendo el mundo físico en tiempo real, pero es pésimo
> guardando históricos y haciendo análisis: el UNO tiene 2 kilobytes de RAM y 32 de programa.
> Una computadora es justo al revés.*
>
> *Entonces la división natural del trabajo es: Arduino sensa y envía; Python recibe, almacena,
> analiza y visualiza. Eso es la arquitectura de un sistema IoT en pequeño. Y es también un
> caso de programación distribuida: dos programas, en dos máquinas distintas, que no comparten
> memoria y se coordinan solo por mensajes.*

---

## Parte 2 — Cómo viaja el dato por el cable (60 s)

> *El UNO R3 no tiene WiFi ni Bluetooth. Su vía de comunicación es el puerto serie, el mismo
> cable USB con el que le cargo el programa.*
>
> *Y por dentro pasa algo que conviene saber: el ATmega328 habla UART, un protocolo serie a
> 5 voltios. Una computadora moderna no entiende eso, así que la placa lleva un SEGUNDO chip
> que hace de traductor USB a serie. En el UNO original es un ATmega16U2; en los clones y en
> muchos Elegoo es un CH340.*
>
> *Ese chip es el que crea el puerto COM que aparece en Windows. Y por eso, cuando Windows no
> reconoce una placa clónica, casi siempre lo que falta es el driver del CH340.*

---

## Parte 3 — El formato de los datos (45 s)

Muestra la función `enviarCSV` del sketch.

> *Podría imprimir «la temperatura es de 25 grados», pero eso obligaría al programa receptor a
> descifrar el texto. Es frágil y feo.*
>
> *Uso CSV: valores separados por comas, con una cabecera al arrancar. Las ventajas son
> concretas: Python lo parte con una sola llamada a `split`, se abre directo en Excel para el
> informe, y ocupa muy pocos bytes, que a 9600 baudios importa.*
>
> *La alternativa sería JSON, que es autodescriptivo, pero ocupa el triple y en un ATmega328
> con 2 KB de RAM eso pesa. Para cinco campos, CSV es la elección correcta.*

Señala los `-1`:

> *Y una decisión pequeña pero importante: cuando un sensor no da lectura válida, envío
> menos uno en vez de cero. Un cero sería engañoso, porque cero grados es una temperatura
> perfectamente real.*

---

## Parte 4 — Los tres problemas del puerto serie (60 s)

**Esta es la parte que distingue tu video.** Casi nadie explica esto.

> *Al programar el lector me encontré tres problemas que quiero mencionar, porque no son
> obvios.*
>
> *El primero es el AUTO-RESET. Al abrir el puerto serie, la señal DTR reinicia la placa. O
> sea: Python se conecta y Arduino arranca de cero justo en ese momento. Por eso mi programa
> espera dos segundos y medio antes de leer nada; si no, recibe las primeras líneas cortadas.*

Señala el `time.sleep(2.5)`.

> *El segundo: el puerto entrega BYTES, no líneas. Una lectura puede llegar partida por la
> mitad. Por eso además de usar `readline` valido que la línea tenga exactamente el número de
> campos que espero, y si no, la descarto.*

Señala la función `parsear`.

> *Y el tercero, los baudios. Los dos extremos tienen que usar el mismo número. Si Arduino
> manda a 9600 y Python escucha a 115200, se reciben símbolos raros. Y eso no es un error del
> programa: es que se están leyendo los bits a destiempo.*

---

## Parte 5 — La demostración (90 s)

Ejecuta en la terminal:

```bash
python lector_serial.py
```

> *Ejecuto el lector. Lo primero que hace es buscar el puerto solo, mirando el nombre del chip
> conversor. Ahí detectó la placa.*

Espera a que aparezca la cabecera y las primeras filas.

> *Recibió la cabecera que manda Arduino, y con ella nombra las columnas: así no las tengo
> escritas a mano en dos sitios distintos.*
>
> *Y ahí van llegando las lecturas, una por segundo: número de lectura, tiempo desde que
> arrancó la placa, temperatura, humedad, luz y distancia.*

**Ahora provoca cambios en vivo** — esto es lo que demuestra que es real:

Tapa el LDR con la mano:

> *Tapo el sensor de luz... y vean cómo el valor de luz se desploma.*

Ilumínalo con el celular:

> *Le pongo la linterna... y sube de golpe.*

Pon la mano frente al ultrasónico:

> *Y acerco la mano al sensor de distancia... la columna de distancia cambia al instante.*

Señala el LED de la placa:

> *El LED de la placa parpadea con cada envío, así confirmo visualmente que está transmitiendo.*

Ahora Ctrl+C:

> *Paro con Control+C, y al detenerse hace tres cosas: guarda todas las lecturas en un archivo
> CSV, imprime un resumen con el mínimo, el máximo y el promedio de cada magnitud, y dibuja la
> gráfica.*

Muestra la gráfica y el CSV abierto en Excel:

> *Aquí está la gráfica, con una subgráfica por sensor. Y este es el CSV, que se abre directo
> en Excel.*

Un detalle que suma:

> *Fíjense que en la gráfica la línea se CORTA donde el sensor no dio dato, en vez de caer a
> cero. Eso lo hice a propósito: dibujar un pico hasta abajo sería mentir sobre lo que midió
> el sensor.*

---

## Parte 6 — Cierre (20 s)

> *Eso es todo: Arduino sensando y enviando por el puerto serie, y Python recibiendo,
> validando, almacenando y graficando. Dos lenguajes, dos máquinas, coordinados solo por
> mensajes. Gracias.*

---

## Qué entregar después

| Forma | Qué subir |
|---|---|
| **A** | Este video + capturas de la terminal, la gráfica y el CSV en Excel |
| **B** | Copiar y pegar `P5_Arduino_Python.ino` en el Editor de la plataforma |
| **C** | Adjuntar `P5_Arduino_Python.ino` **y** `lector_serial.py` |

> Sube también el `datos_arduino.csv` y la `grafica_arduino.png` que genera el programa: son
> la evidencia de que corrió de verdad.

---

## Si algo no funciona

| Síntoma | Causa |
|---|---|
| «Acceso denegado» al abrir el puerto | El Monitor Serie del IDE está abierto. Ciérralo. |
| No detecta ningún puerto | Falta el driver del CH340 (placas clónicas) |
| Llegan símbolos raros | Los baudios no coinciden entre el sketch y Python |
| No llega ninguna lectura | ¿Cargaste el sketch? ¿Es el puerto correcto? |
| `ModuleNotFoundError: serial` | `pip install pyserial` (ojo: **pyserial**, no «serial») |
