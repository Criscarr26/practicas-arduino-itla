# Guion de grabación — Práctica 4: temperatura, humedad y display

**Duración objetivo: 6 minutos.** Vale 8 puntos. Lo que va en *cursiva* se lee en voz alta.

---

## Antes de grabar

- **Ajusta el contraste del LCD con el potenciómetro ANTES de grabar.** Es el error número uno:
  si no lo giras, la pantalla se ve en blanco y parece que el código falla.
- Ten a mano algo para calentar el sensor: los dedos, el aliento, o un secador de pelo a
  distancia y en frío-templado. **No lo acerques a una llama.**
- Monitor Serie a 9600.
- Al arrancar, el programa hace una prueba de los tres colores (rojo, verde, azul). Grábala:
  demuestra que el cableado del RGB está bien.

---

## Parte 1 — Presentación (20 s)

> *Buenas. Soy Cristian Carrera, matrícula 2024-1932, de Inteligencia Artificial e Internet de
> las Cosas con el profesor Luis Bessewell. Elegí la opción D del enunciado: medidor de
> temperatura y humedad con LED de colores, alarma sonora y pantalla de despliegue.*

---

## Parte 2 — Investigación: el DHT11 (75 s)

Muestra el sensor a cámara.

> *El sensor es el DHT11, y lo interesante es que mide DOS magnitudes con un solo componente.
> Según su hoja de datos: temperatura de 0 a 50 grados con precisión de más o menos 2 grados,
> y humedad del 20 al 90 por ciento con más o menos 5 por ciento.*
>
> *Por dentro lleva dos sensores en la misma cápsula. Para la humedad, un sustrato entre dos
> electrodos cuya CAPACIDAD cambia al absorber vapor de agua. Y para la temperatura, un
> termistor NTC, que es una resistencia que baja su valor al calentarse.*

El detalle que casi nadie explica:

> *Lo curioso es cómo se comunica: por UN SOLO CABLE, con un protocolo propio que no es I2C ni
> SPI. Yo tiro la línea a cero unos 18 milisegundos para pedirle datos, la suelto, y el sensor
> me responde con 40 bits. Y cada bit se codifica por la DURACIÓN del pulso: unos 26
> microsegundos es un cero y unos 70 es un uno. Leer eso a mano sería un dolor de cabeza, por
> eso se usa una librería.*

**Y ahora la parte honesta, que suma mucho:**

> *Hay una limitación que quiero señalar. El DHT11 es un sensor de AMBIENTE, no un termómetro
> clínico. Su precisión de más o menos 2 grados es enorme para medir fiebre, donde la
> diferencia entre normal y grave es de un grado.*
>
> *O sea: aquí lo uso para DEMOSTRAR la lógica de umbrales, calentándolo con los dedos. En un
> producto real haría falta un MLX90614 infrarrojo o un DS18B20.*

---

## Parte 3 — Investigación: la pantalla LCD1602 (60 s)

> *La pantalla es el LCD1602 del kit. 1602 significa 16 columnas por 2 filas, y lleva el
> controlador HD44780, que es el estándar de la industria desde los años ochenta.*
>
> *Un detalle importante del kit de Elegoo: este LCD es de PINES PARALELOS, no trae adaptador
> I2C. Eso significa que se maneja con la librería `LiquidCrystal`, la que ya viene con el
> IDE, y NO con `LiquidCrystal_I2C`.*
>
> *Lo uso en modo de 4 bits: el controlador acepta 8 bits de datos, pero se puede mandar cada
> byte en dos mitades. Gasto 4 pines en vez de 8 y la velocidad sobra de todas formas para
> texto.*
>
> *Los pines de control son dos: RS dice si lo que mando es un comando o un carácter, y E es
> el pulso de habilitación.*

Señala el potenciómetro:

> *Y este potenciómetro no es opcional. El pin V0 controla el contraste. Sin él la pantalla se
> ve completamente en blanco o completamente en negro, y uno jura que el código está mal
> cuando en realidad solo falta girar la perilla. Es el error número uno con este display.*

---

## Parte 4 — El montaje y los temporizadores (45 s)

> *Sobre el cableado hay dos decisiones que quiero explicar.*
>
> *La primera: puse el LCD en los pines analógicos, de A0 a A5. Los pines analógicos funcionan
> perfectamente como digitales, son los D14 a D19. Poniendo ahí la pantalla me quedan libres
> los pines PWM para el LED RGB, que sí necesita PWM de verdad para mezclar colores.*
>
> *La segunda, y esta me costó darme cuenta: la función `tone()` usa el Timer 2 del
> ATmega328, que es el MISMO que genera el PWM de los pines 3 y 11. Si hubiera puesto el LED
> RGB en esos pines, el color me habría cambiado solo cada vez que sonara la alarma. Por eso
> el RGB va en los pines 5, 6 y 9.*

---

## Parte 5 — Investigación: hilos y tareas (45 s)

> *El enunciado pedía investigar si Arduino soporta hilos o tareas. El ATmega328 tiene un solo
> núcleo y no tiene sistema operativo: hilos reales no hay.*
>
> *Pero sí hay CONCURRENCIA, que no es lo mismo que paralelismo. Concurrencia es que varias
> tareas progresen intercaladas; paralelismo es que se ejecuten literalmente a la vez, y eso
> necesita varios núcleos.*
>
> *Y esta práctica lo NECESITA: tengo que leer el sensor cada dos segundos, refrescar la
> pantalla cada medio segundo, y hacer sonar un S.O.S. cuyos símbolos duran 150 o 450
> milisegundos. Todo al mismo tiempo.*
>
> *Con `delay()` la pantalla se congelaría mientras suena la alarma. Por eso en este programa
> no hay ni un solo `delay()`, y hasta el S.O.S. está escrito como una máquina de estados que
> avanza paso a paso.*

Señala el arreglo `PATRON_SOS`:

> *Aquí está el truco: guardo solo las duraciones, porque el patrón siempre alterna sonido y
> silencio. Los índices pares suenan y los impares callan. Así la tabla ocupa la mitad.*

---

## Parte 6 — La demostración (90 s)

**Esta es la parte que vale los puntos.** Ve calentando el sensor despacio.

Estado inicial, temperatura ambiente:

> *Arranco a temperatura ambiente, unos 26 grados. Como está por debajo de 36, el sistema lo
> marca como HIPOTERMIA y enciende el LED en azul. En la pantalla veo la temperatura, la
> humedad y el estado.*

Calienta con los dedos hasta pasar de 36:

> *Ahora sujeto el sensor con los dedos... y al pasar de 36 grados entra en NORMAL: el LED se
> pone verde y no hay alarma.*

Sigue calentando hasta pasar de 37.2:

> *Sigo calentando... y al pasar de 37.2 entra en FIEBRE. El LED se pone rojo y el zumbador
> emite un tono continuo, que se mantiene hasta que la temperatura baje, como pide el
> enunciado.*

Hasta pasar de 39:

> *Y si sigo hasta pasar de 39 grados, entra en estado CRÍTICO. El LED sigue rojo, pero ahora
> el sonido cambia: es un S.O.S. en código Morse. Tres puntos, tres rayas, tres puntos.*

Déjalo sonar completo, y luego suelta el sensor:

> *Y fíjense en esto mientras suena la alarma: la pantalla se sigue refrescando y el sensor se
> sigue leyendo. Nada se congela. Eso es la concurrencia funcionando.*
>
> *Ahora suelto el sensor y, según se va enfriando, va bajando de crítico a fiebre, y de
> fiebre a normal, con el LED volviendo a verde.*

---

## Parte 7 — Cierre (20 s)

> *Eso es todo: opción D del enunciado, con el DHT11, el LED RGB, la alarma sonora y el
> display, todo con las estructuras de control investigadas y programado de forma concurrente.
> Gracias.*

---

## Qué entregar después

| Forma | Qué subir |
|---|---|
| **A** | Este video + capturas del código y del montaje |
| **B** | Copiar y pegar `P4_Temperatura_Display.ino` en el Editor de la plataforma |
| **C** | Adjuntar el archivo `P4_Temperatura_Display.ino` |

---

## Si algo no funciona

| Síntoma | Causa |
|---|---|
| Pantalla en blanco o toda negra | Falta girar el potenciómetro de contraste |
| Salen cuadros negros en la fila de arriba | El LCD arrancó mal: revisa RS, E y los cuatro pines de datos |
| Los colores salen invertidos | Tu LED RGB es de ánodo común: cambia `CATODO_COMUN` a `false` |
| `Sensor sin dato` todo el tiempo | Cable de datos suelto, o falta la librería DHT de Adafruit |
| El color cambia solo al sonar la alarma | Moviste el RGB a los pines 3 u 11: devuélvelo a 5, 6 y 9 |
