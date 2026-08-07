# Guion de grabación — Práctica 3: detector de distancia con motor

**Duración objetivo: 6 a 7 minutos.** Vale 8 puntos y tiene cuatro temas de investigación,
así que este video es más largo. Lo que va en *cursiva* es para leerlo en voz alta.

---

## Antes de grabar

- Ten una regla o cinta métrica en la mesa, y un objeto plano (un libro, una carpeta) para
  acercar y alejar del sensor. **Una superficie plana y dura**: la tela y la espuma absorben
  el ultrasonido y el sensor no las ve.
- Monitor Serie a 9600. Va imprimiendo distancia, porcentaje y zona: es lo que hace el video
  convincente.
- Marca en la mesa con cinta adhesiva las distancias clave: **16 cm, 104 cm y 200 cm**. Así
  las transiciones de zona salen limpias en cámara y no tienes que improvisar.
- Si la placa se reinicia sola al mover el servo, aliméntalo con una fuente externa de 5 V
  compartiendo el GND.

---

## Parte 1 — Presentación (20 s)

> *Buenas. Soy Cristian Carrera, matrícula 2024-1932, de Inteligencia Artificial e Internet de
> las Cosas con el profesor Luis Bessewell. Esta es la práctica del detector de distancia que
> gobierna un motor, un LED y un zumbador, montada en un Arduino UNO R3.*

---

## Parte 2 — Investigación 1: estructuras de control (60 s)

> *La primera investigación eran las estructuras de control. Arduino es C++, así que tiene las
> cuatro, y en este programa uso las cuatro, cada una donde corresponde.*
>
> *El `if` con `else if` sirve para DECIDIR entre caminos. Lo uso para clasificar la distancia
> en zonas, porque las condiciones son RANGOS.*
>
> *El `switch` sirve para elegir entre valores concretos. Lo uso para actuar según la zona ya
> calculada.*

Este es el punto que te distingue:

> *Y aquí está la razón de usar los dos: `switch` NO admite rangos, solo valores exactos. Por
> eso primero decido con `if`, que sí sabe comparar rangos, y después actúo con `switch`, que
> se lee mucho mejor que seis `if` encadenados. No es que uno sea mejor: es que resuelven
> problemas distintos.*
>
> *El `for` repite un número de veces conocido de antemano; lo uso para tomar cinco muestras
> del sensor. Y el `while` repite mientras se cumpla una condición, sin saber cuántas vueltas
> serán; lo uso en el ordenamiento de esas muestras.*

---

## Parte 3 — Investigación 2: el HC-SR04 (75 s)

Toma el sensor en la mano y muéstralo a cámara.

> *La segunda investigación era el alcance máximo del sensor y cómo funciona. Este es el
> HC-SR04, y según su hoja de datos mide **de 2 centímetros a 400 centímetros**, o sea cuatro
> metros, con una precisión de unos 3 milímetros y un cono de unos 15 grados.*
>
> *Funciona por eco, el mismo principio del murciélago. Yo mando un pulso de 10 microsegundos
> por el pin TRIG. El sensor emite ocho ráfagas de ultrasonido a 40 kilohercios. El sonido
> viaja, rebota en el obstáculo y vuelve. Y el pin ECHO se queda en alto exactamente el tiempo
> que tardó el viaje de ida y vuelta.*

Señala la fórmula en el código.

> *La fórmula sale de la velocidad del sonido: 343 metros por segundo, que son 29.1
> microsegundos por centímetro. Entonces distancia igual a tiempo entre 29.1, y todo eso
> dividido entre dos.*
>
> *Y ese «entre dos» es la parte que más se olvida: divido entre dos porque el tiempo que medí
> es de IDA Y VUELTA, y a mí solo me interesa la ida.*

Menciona las limitaciones — demuestra que investigaste de verdad:

> *Tiene tres limitaciones que conviene conocer. Las superficies blandas, como una cortina,
> absorben el ultrasonido y el sensor no las ve. Las superficies en ángulo desvían el eco
> hacia otro lado. Y la velocidad del sonido depende de la temperatura, así que en un día muy
> caluroso la medida se desvía un poco.*

---

## Parte 4 — Investigación 3: encender y apagar el motor (45 s)

> *La tercera investigación era la sentencia para encender y apagar un motor. Depende del tipo.*
>
> *Yo uso el servomotor SG90 del kit. Se incluye la librería `Servo.h`, y entonces
> `attach(pin)` lo enciende, `write(ángulo)` lo posiciona entre 0 y 180 grados, y `detach()`
> lo apaga dejando de enviarle pulsos.*
>
> *Lo curioso del servo es que no se controla con voltaje, sino con el ancho de un pulso que
> se repite cada 20 milisegundos: un milisegundo son 0 grados, dos milisegundos son 180.*
>
> *Para un motor de corriente continua sería distinto: haría falta un driver L298N y se
> controlaría con `digitalWrite` en las entradas y `analogWrite` en el pin de habilitación
> para la velocidad.*

Y el aviso de seguridad:

> *Algo que nunca hay que hacer: conectar un motor directamente a un pin de Arduino. Un pin
> entrega 40 miliamperios como máximo absoluto y un motor pide cientos. Siempre hace falta un
> driver, o al menos un transistor.*

---

## Parte 5 — Investigación 4: hilos y tareas (60 s)

Esta parte conecta con el trabajo grande de la asignatura. Aprovéchala.

> *La cuarta investigación era si Arduino soporta programación en hilos o tareas. La respuesta
> corta es no, y la larga es más interesante.*
>
> *El ATmega328 del UNO tiene un solo núcleo, no tiene sistema operativo y no tiene gestión de
> memoria. No existen hilos reales: en cada instante se ejecuta una sola instrucción.*
>
> *Pero sí se puede tener CONCURRENCIA, que no es lo mismo que paralelismo. Concurrencia es
> organizar el programa para que varias tareas progresen intercaladas. Paralelismo es
> ejecutarlas literalmente a la vez, y eso sí necesita varios núcleos.*

Y ahora la justificación práctica, que es lo que el profesor quiere ver:

> *Y por qué esto importa aquí: este programa tiene que hacer cuatro cosas a la vez con ritmos
> distintos. Medir la distancia cada 60 milisegundos, mover el servo cada 15, parpadear el LED
> cada 100 o 400 según la zona, y reportar cada 500.*
>
> *Si lo hubiera programado con `delay()`, mientras el LED espera su parpadeo el sensor
> estaría ciego y el servo congelado. Por eso en todo este programa **no hay ni un solo
> `delay()`**. Cada tarea mira el reloj con `millis()`, se pregunta si ya le toca, hace su
> trabajo en microsegundos y devuelve el control. El `loop()` se convirtió en un planificador.*

---

## Parte 6 — Los cuatro criterios y el solapamiento (60 s)

**Esta parte es la que casi nadie va a hacer.** El enunciado tiene rangos que se pisan.

> *Ahora, los cuatro criterios del enunciado. Con 400 centímetros de máximo, el 95 por ciento
> son 380, el 50 son 200, el 26 son 104, el 25 son 100 y el 4 por ciento son 16 centímetros.*
>
> *Al calcularlos me di cuenta de que los rangos SE SOLAPAN. El criterio 3 va del 25 al 50 por
> ciento y el criterio 4 va del 4 al 26: se pisan entre 100 y 104 centímetros. Y además el
> criterio 4 baja hasta el 4 por ciento, por debajo del 5 por ciento que el criterio 1 fija
> como límite de operación.*
>
> *Así que tuve que tomar una decisión de diseño y justificarla. Evalúo de más urgente a menos
> urgente, o sea, de más cerca a más lejos. La razón es que en un sistema de proximidad,
> mientras más cerca está el obstáculo más crítica es la alarma, así que la zona más cercana
> manda sobre la más lejana.*

---

## Parte 7 — La demostración (90 s)

Aquí es donde se ganan los puntos. Ve acercando el objeto despacio y **narra cada transición**.

Empieza con el objeto lejos, más allá de 380 cm (o apunta al techo/lejos):

> *Empiezo fuera de rango, más allá del 95 por ciento: motor apagado, LED apagado, sin sonido.
> Criterio 2.*

Acerca hasta unos 300 cm:

> *Ahora entro en rango. El motor arranca, y el LED queda fijo encendido. Criterio 1.*

Acerca hasta unos 150 cm:

> *Cruzo los 200 centímetros, el 50 por ciento. El LED empieza a parpadear y el zumbador suena
> al mismo ritmo que la luz. Criterio 3.*

Acerca hasta unos 60 cm:

> *Y ahora paso de los 104 centímetros. El parpadeo se acelera y el sonido cambia: ya no es un
> pitido simple, es una sirena de dos tonos. Criterio 4.*

Acerca a menos de 16 cm:

> *Y si me acerco por debajo del 4 por ciento, todo se apaga: estoy fuera del rango útil.*

Señala el monitor serie:

> *En el monitor voy viendo la distancia, el porcentaje respecto al máximo, la zona y el
> estado del motor. Y cada cambio de zona lo avisa al instante.*

---

## Parte 8 — Cierre (20 s)

> *Eso es todo. Detector de distancia con las cuatro estructuras de control, el HC-SR04 con su
> alcance de cuatro metros, el servo encendido y apagado con `attach` y `detach`, y toda la
> lógica escrita de forma concurrente y sin bloqueos. Gracias.*

---

## Qué entregar después

| Forma | Qué subir |
|---|---|
| **A** | Este video + capturas del código y del montaje |
| **B** | Copiar y pegar `P3_Distancia_Motor.ino` en el Editor de la plataforma |
| **C** | Adjuntar el archivo `P3_Distancia_Motor.ino` |
