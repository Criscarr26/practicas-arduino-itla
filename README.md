# Proyectos de Arduino

Colección de ocho proyectos sobre una **Arduino UNO R3**, del LED que parpadea hasta un sistema
de telemetría en el que la placa publica sus lecturas por el puerto serie y otro programa, en
otra máquina y en otro lenguaje, las recibe y las analiza.

No son ejemplos copiados. Cada sketch lleva en su cabecera por qué está resuelto así: qué se
investigó antes de escribirlo, qué se probó, y qué falló por el camino.

---

## Los proyectos

| Carpeta | Qué hace |
|---|---|
| [`parpadeo_led/`](parpadeo_led) | Encender y apagar un LED. El punto de partida |
| [`sos_morse/`](sos_morse) | S.O.S. en código Morse, por LED y zumbador sincronizados |
| [`melodia_mario/`](melodia_mario) | El tema de Super Mario Bros, con el LED siguiendo cada nota |
| [`sensor_distancia/`](sensor_distancia) | HC-SR04 con dos zumbadores: uno pulsa más rápido al acercarse, otro suena continuo al detenerse |
| [`sensor_distancia_servo/`](sensor_distancia_servo) | La misma idea gobernando un servo — **simulador** |
| [`detector_oscuridad/`](detector_oscuridad) | Fotorresistencia con histéresis que emite S.O.S. mientras no hay luz |
| [`estacion_ambiental/`](estacion_ambiental) | DHT11 + LDR + LED RGB + LCD 16x2 — **simulador** |
| [`telemetria_serie/`](telemetria_serie) | La placa publica CSV por el puerto; se lee desde **Python y C#** |

Las carpetas marcadas como simulador traen un `diagram.json` listo para pegar en
[wokwi.com](https://wokwi.com); las demás corren sobre la placa real.

---

## Lo que tiene de interesante

### Telemetría: el mismo problema, dos lenguajes

`telemetria_serie/` es el proyecto que más enseña. La placa mide, avisa por sonido y **publica
cada lectura en CSV por el puerto serie**. Del otro lado hay dos programas que hacen lo mismo:
uno en Python y otro en C#.

Lo interesante es lo que **no** pasó: para escribir el segundo lector no hubo que tocar ni una
línea del firmware. La placa publica y sigue; no espera respuesta ni sabe quién la escucha. Si
se desconecta la computadora, los zumbadores siguen funcionando igual.

Son dos programas, en dos máquinas, que no comparten memoria y se coordinan solo por mensajes.

Dos detalles del puerto serie que costaron horas y no aparecen en los tutoriales:

- **`SerialPort` de .NET no levanta la señal DTR por su cuenta, y pyserial sí.** Sin
  `DtrEnable = true` la placa no se reinicia al conectarse, y la numeración de lecturas continúa
  donde la dejó la sesión anterior en vez de empezar en 1.
- **Hay que vaciar el buffer *antes* de esperar el arranque, no después.** Al revés se pierde la
  línea de cabecera, que la placa manda una sola vez y no repite nunca.

### Histéresis: por qué dos umbrales y no uno

En `detector_oscuridad/`, lo natural sería un solo número: por debajo de 300, oscuro. El
problema es la frontera — la lectura nunca es estable y el detector entra y sale de la alarma
varias veces por segundo.

La solución son **dos umbrales separados**: entra en alarma a 280 y sale a 380. Entre medio no
pasa nada. Es el mismo principio del termostato de una nevera.

Y los umbrales están **calibrados, no copiados**: el programa imprime el mínimo y el máximo que
ha visto desde que arrancó, porque los valores de una LDR dependen del modelo, de la resistencia
fija y de la luz del sitio.

### Concurrencia sin hilos

El ATmega328P tiene **un núcleo y ningún sistema operativo**: hilos reales no hay. Pero
concurrencia sí, y en varios de estos proyectos hace falta de verdad — mientras suena el S.O.S.
hay que seguir midiendo la luz, para que la alarma se corte en el acto al encenderse una lámpara.

Con `delay()` eso es imposible: el ciclo completo del S.O.S. dura unos cinco segundos y durante
todos ellos el micro estaría bloqueado.

Por eso **ninguno de los proyectos con sensores llama a `delay()`**. El `loop()` no hace trabajo:
es un planificador que ofrece el turno a cada tarea, y cada una decide si le toca mirando
`millis()`. El S.O.S. está escrito como máquina de estados que avanza un símbolo por vuelta.

> Matiz honesto: en los proyectos con HC-SR04, `pulseIn()` **sí bloquea** mientras espera el eco,
> hasta 25 ms por intento, y la medición filtrada lo llama tres veces. `delay()` no se usa en
> ninguna parte, pero eso no es lo mismo que decir que nada bloquea.

---

## Cómo ejecutarlos

### Con la placa

Abrir el `.ino` en el Arduino IDE, seleccionar *Arduino UNO* y el puerto, y pulsar **Subir**.

Para `telemetria_serie/`, además, del lado de la computadora:

```bash
python lector_distancia.py
```

o abrir `LectorDistancia.sln` en Visual Studio 2022 y pulsar **F5**.

> El puerto serie admite **un solo programa a la vez**. Si el Monitor Serie del IDE está abierto,
> el lector falla con *acceso denegado*.

### En el simulador

Crear un proyecto de Arduino UNO en [wokwi.com](https://wokwi.com), pegar el `.ino` en la pestaña
del sketch y el `diagram.json` en la suya. El montaje aparece ya cableado.

---

## Librerías

| Proyecto | Qué hace falta |
|---|---|
| `parpadeo_led`, `sos_morse`, `melodia_mario` | nada |
| `sensor_distancia`, `detector_oscuridad` | **nada** |
| `telemetria_serie` — la placa | **nada** |
| `telemetria_serie` — Python | `pip install pyserial` |
| `telemetria_serie` — C# | `System.IO.Ports` (ya en el `.csproj`) |
| `sensor_distancia_servo` | `Servo` |
| `estacion_ambiental` | `DHT sensor library` + `Adafruit Unified Sensor` |

---

## Notas de hardware que conviene saber

**`tone()` usa el Timer2 del ATmega328P**, que es el mismo que genera el PWM de los pines 3 y 11.
Por eso en `estacion_ambiental` el LED RGB va en los pines 5, 6 y 9: en el 3 o el 11 el color
cambiaría solo cada vez que sonara la alarma.

**En el UNO, `tone()` solo puede sonar en un pin a la vez.** Por eso los dos zumbadores de
`sensor_distancia` no pueden ser los dos pasivos: uno es activo y el otro pasivo.

**El LCD1602 del kit Elegoo es de pines paralelos, sin adaptador I2C.** Usa `LiquidCrystal`, no
`LiquidCrystal_I2C`, y necesita un potenciómetro de 10k para el contraste. Sin él la pantalla se
ve en blanco y parece que el código falla. Si no hay potenciómetro, sirve una resistencia fija
entre V0 y GND.

**Hay dos tipos de zumbador.** El pasivo necesita `tone()` para sonar; el activo trae su propio
oscilador y basta con darle corriente. Confundirlos hace perder tardes enteras: el código está
bien y no suena nada.

---

## Uso de inteligencia artificial

Usé **asistencia de IA (Claude)** para depurar y documentar. Son míos el montaje, la calibración
de los umbrales y las pruebas sobre la placa.

---

## Licencia

Código libre de usar. Los tutoriales de los que partieron algunos sketches están citados en la
cabecera de cada uno.
