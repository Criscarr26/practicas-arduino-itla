# Guion de grabación — Práctica 1: Mario Bros con luz y sonido

**Duración objetivo: 3 a 4 minutos.** Lo que va en *cursiva* es para leerlo en voz alta.
Lo demás son instrucciones para ti.

> Consejo: no busques la toma perfecta. Si te trabas, sigue hablando. El profesor evalúa
> que entiendas lo que hiciste, no tu dicción.

---

## Antes de grabar

- Monta el circuito y deja que la melodía suene una vez, para confirmar que todo va bien.
- Abre el Arduino IDE con el código a la vista y el Monitor Serie abierto (9600 baudios).
- Ten a mano el celular o la webcam apuntando al protoboard, donde se vea el LED.
- `Win + Alt + R` graba la pantalla en Windows. Para el montaje físico, graba con el celular.

---

## Parte 1 — Presentación (20 s)

Con la cara o con el circuito en pantalla.

> *Buenas. Soy Cristian Carrera, matrícula 2024-1932, de Inteligencia Artificial e Internet
> de las Cosas con el profesor Luis Bessewell. Esta es la práctica de la melodía introductoria
> de Super Mario Bros, con comportamiento de luz y de sonido, montada en un Arduino UNO R3.*

---

## Parte 2 — El montaje (30 s)

Señala cada componente con el dedo mientras hablas.

> *El montaje es sencillo. En el pin 8 tengo un zumbador pasivo: el positivo al pin 8 y el
> negativo a tierra. En el pin 13, un LED con una resistencia de 220 ohmios en serie.*
>
> *Dos aclaraciones que importan. Primera: tiene que ser un zumbador PASIVO. El activo trae
> su propio oscilador y siempre suena igual, así que ignoraría la frecuencia que le mando y
> haría un solo pitido en vez de la melodía.*
>
> *Segunda: el LED siempre lleva resistencia. Sin ella circula demasiada corriente y se quema
> el LED, el pin de la placa, o los dos.*

---

## Parte 3 — La investigación previa (60 s)

Esta es la parte que el profesor pide explícitamente. Ve al encabezado del código.

**Primero, el LED:**

> *La primera investigación era qué sentencia enciende y apaga un LED. Son dos, y trabajan
> juntas.*
>
> *`pinMode(pin, OUTPUT)` se declara una sola vez, dentro de `setup()`. Le dice a la placa
> que ese pin va a entregar corriente en lugar de leerla.*
>
> *Y ya después, `digitalWrite(pin, HIGH)` lo enciende poniéndolo en 5 voltios, y
> `digitalWrite(pin, LOW)` lo apaga poniéndolo en cero.*
>
> *El detalle es que `digitalWrite` sola no basta. Si se olvida el `pinMode`, el pin se queda
> como entrada y el LED apenas alumbra, porque una entrada solo entrega unos pocos
> microamperios.*

**Después, el tono:**

> *La segunda investigación era cómo emitir un tono con una duración específica y cómo
> silenciarlo. La sentencia es `tone()`, y tiene dos formas:*
>
> *`tone(pin, frecuencia)` suena indefinidamente, hasta que algo lo pare. Y
> `tone(pin, frecuencia, duración)` suena exactamente los milisegundos que le digas.*
>
> *Para silenciar es `noTone(pin)`.*
>
> *Lo interesante es cómo funciona por dentro: `tone()` no reproduce audio. Lo que hace es
> generar una onda cuadrada, o sea, encender y apagar el pin miles de veces por segundo. Si
> lo hace 440 veces por segundo, el zumbador vibra a 440 hercios y nosotros oímos un LA.*

**Y el detalle que más cuesta entender** (este párrafo te distingue):

> *Hay tres cosas de `tone()` que conviene saber. La primera es que `tone()` con duración NO
> bloquea: la sentencia devuelve el control de inmediato y el tono sigue sonando por debajo
> gracias a un temporizador del microcontrolador. Por eso en mi código pongo un `delay()`
> detrás, para esperar a que la nota termine.*
>
> *La segunda es que `tone()` usa el Timer 2 del ATmega328, que es el mismo que usa
> `analogWrite()` en los pines 3 y 11. Mientras suena un tono, esos dos pines pierden el PWM.*
>
> *Y la tercera: solo puede sonar un tono a la vez en toda la placa. Si llamo a `tone()` en
> otro pin, corta el anterior.*

---

## Parte 4 — El código (70 s)

Baja hasta los arreglos.

> *La melodía son dos listas que se recorren en paralelo. En `notas[]` guardo la frecuencia
> de cada nota en hercios, donde el cero significa silencio. Y en `duraciones[]` guardo
> cuánto dura cada una, como divisor de una redonda.*
>
> *La cuenta es esta: `duracionNota = 1000 / duraciones[i]`. O sea, un doce da mil partido
> doce, unos 83 milisegundos.*

Señala la constante `TOTAL_NOTAS`.

> *Aquí el compilador cuenta solo cuántas notas hay, dividiendo el tamaño total del arreglo
> entre el tamaño de un elemento. Así, si mañana agrego o quito una nota, no tengo que
> acordarme de actualizar ningún número a mano.*

Baja al `for`.

> *Uso un bucle `for` porque sé de antemano cuántas repeticiones necesito: una por nota.*
>
> *Dentro, un `if` separa los dos casos. Si la nota es un silencio, apago las dos salidas:
> ni suena ni se ilumina. Un silencio también es música.*
>
> *Si no es silencio, ahí están las dos sentencias investigadas trabajando juntas: `tone()`
> con su duración, y `digitalWrite` en HIGH para que el LED se encienda mientras suena.*

Señala el `pausaEntreNotas`.

> *Y este 30 % extra es lo que hace que la melodía se reconozca. Sin esa separación las notas
> se pegan una con otra y suena como un zumbido continuo. Lo probé sin ella y no se entendía
> nada.*

Señala la comprobación del `setup()`.

> *Por último, esta comprobación. Si los dos arreglos no midieran lo mismo, el programa leería
> memoria que no le pertenece y sonarían notas basura. Prefiero que se detenga y me avise con
> un parpadeo rápido.*

---

## Parte 5 — La demostración (60 s)

Apunta la cámara al circuito. **Deja que suene la melodía completa** — es lo que el profesor
quiere ver.

> *Y así queda funcionando.*

Deja correr unos 30 segundos en silencio, sin narrar. Luego:

> *Como ven, el LED se enciende con cada nota y se apaga en los silencios, así que la luz
> sigue exactamente el ritmo de la música. Y en el monitor serie voy viendo cuándo empieza y
> cuándo termina cada repetición.*

---

## Parte 6 — Cierre (15 s)

> *Eso es todo. Melodía de Mario Bros con las dos sentencias investigadas: `digitalWrite`
> para la luz y `tone` con `noTone` para el sonido. Gracias.*

---

## Qué entregar después

Según pide el enunciado, en tres formas:

| Forma | Qué subir |
|---|---|
| **A** | Este video + capturas del código y del montaje |
| **B** | Copiar y pegar el contenido de `P1_Mario_LED_Tono.ino` en el Editor de la plataforma |
| **C** | Adjuntar el archivo `P1_Mario_LED_Tono.ino` |
