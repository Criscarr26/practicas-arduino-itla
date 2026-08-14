# Guion de grabación — Práctica 2: S.O.S. en código Morse

**Duración objetivo: 3 minutos.** Lo que va en *cursiva* es para leerlo en voz alta.

---

## Antes de grabar

- Deja que el S.O.S. se repita una vez completo antes de empezar, para confirmar que va bien.
- Monitor Serie abierto a 9600 baudios: va imprimiendo `S ( . . . )` y `O ( _ _ _ )`, lo que
  ayuda muchísimo a que se entienda el video.
- Si acabas de grabar la Práctica 1, **no repitas la investigación completa**: di que es la
  misma y resume en 20 segundos. Aquí lo interesante es otra cosa.

---

## Parte 1 — Presentación (20 s)

> *Buenas. Soy Cristian Carrera, matrícula 2024-1932, de Inteligencia Artificial e Internet de
> las Cosas con el profesor Luis Bessewell. Esta es la práctica de la señal de socorro S.O.S.
> en código Morse, con luz y sonido, en un Arduino UNO R3.*

---

## Parte 2 — La investigación previa, en corto (30 s)

> *Las sentencias investigadas son las mismas de la práctica anterior. Para el LED,
> `pinMode` en `setup()` para declarar el pin como salida, y después `digitalWrite` en HIGH
> para encender y en LOW para apagar.*
>
> *Para el sonido, `tone(pin, frecuencia, duración)` para emitir un tono durante un tiempo
> exacto, y `noTone(pin)` para silenciarlo. Y tiene que ser un zumbador pasivo, porque el
> activo trae su propio oscilador e ignoraría la frecuencia que le mando.*

---

## Parte 3 — Qué es el S.O.S. y por qué es así (45 s)

Esta parte es la que hace distinto tu video. Casi nadie la explica.

> *Ahora, lo interesante de esta práctica no es el código, es el estándar.*
>
> *Primero: el S.O.S. no son las iniciales de ninguna frase. Ni «Save Our Souls» ni nada de
> eso. Se eligió en 1906 precisamente porque su patrón en Morse es el más simple e
> inconfundible que existe: tres puntos, tres rayas, tres puntos.*
>
> *Y segundo, el Morse no define tiempos en milisegundos. Define PROPORCIONES respecto a una
> unidad básica, que es lo que dura un punto:*
>
> *El punto dura una unidad. La raya, tres. El espacio entre símbolos de una misma letra, una.
> Entre letras, tres. Y entre palabras, siete.*

Señala el bloque de constantes.

> *Por eso en mi código no hay ni un solo número mágico suelto. Todo se deriva de esta
> constante `UNIDAD`, que vale 200 milisegundos. La raya es `UNIDAD * 3`, la pausa entre
> letras es `UNIDAD * 3`, y así.*
>
> *La ventaja es directa: si cambio ese único 200 por un 100, el mensaje entero va al doble
> de velocidad y sigue siendo Morse correcto. No tengo que tocar nada más.*

---

## Parte 4 — El código (45 s)

Ve a `emitirSimbolo()`.

> *Toda la práctica se reduce a esta función. Recibe una duración, enciende el LED y el
> zumbador a la vez, espera, y los apaga.*
>
> *Fíjense en el detalle: como recibe la duración por parámetro, la MISMA función me sirve
> para el punto y para la raya. Lo único que cambia es el número que le paso. No tuve que
> escribir dos funciones casi idénticas.*

Señala el `delay(duracion)`.

> *Este `delay` es necesario porque `tone()` no bloquea. La sentencia devuelve el control
> enseguida y el tono sigue sonando por debajo; si no esperara aquí, el programa seguiría de
> largo y cortaría la nota antes de tiempo.*

Sube a `emitirLetraS()`.

> *Y arriba, la letra S son tres puntos y la O son tres rayas. Uso un bucle `for` porque sé de
> antemano cuántas repeticiones necesito: exactamente tres.*

---

## Parte 5 — La demostración (45 s)

Apunta al circuito. **Deja que se vea el ciclo completo.**

> *Y así funciona.*

Silencio mientras corre. Cuando vaya por la O:

> *Ahí van las tres rayas de la O: se nota que duran el triple que los puntos.*

Al terminar:

> *En el monitor serie voy viendo qué letra está emitiendo en cada momento, y después la pausa
> de siete unidades antes de repetir el mensaje.*

---

## Parte 6 — Cierre (15 s)

> *Eso es todo. S.O.S. en Morse con las proporciones del estándar internacional, usando
> `digitalWrite` para la luz y `tone` con `noTone` para el sonido. Gracias.*

---

## Qué entregar después

| Forma | Qué subir |
|---|---|
| **A** | Este video + capturas del código y del montaje |
| **B** | Copiar y pegar el contenido de `Tarea1_SOS_LED_Audio.ino` en el Editor de la plataforma |
| **C** | Adjuntar el archivo `Tarea1_SOS_LED_Audio.ino` |

> Ojo: ya tenías un `sos_led_audio` en la carpeta. Este lo sustituye: usa `tone()` con
> duración (el anterior no la pasaba), deriva todos los tiempos del estándar Morse en vez de
> usar números sueltos, e imprime por el monitor serie lo que va emitiendo.
