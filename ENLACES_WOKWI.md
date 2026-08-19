# Enlaces de Wokwi — listos para entregar

**Cristian Carrera — 2024-1932** · IA e IoT 2026-C-2 · Prof. Luis Bessewell Feliz

Los seis proyectos están guardados y son públicos. Se abren sin iniciar sesión.

> ### ⚠️ AVISO DE ALCANCE — leer antes de usar estos enlaces
>
> Este documento es de **agosto 13**, de cuando todo se resolvía en simulador. Ya no describe
> lo que se entrega. En concreto:
>
> - **La Tarea 5 de aquí es la versión ESP32 + MQTT, que quedó descartada.** Lo que se entrega
>   publica CSV por el puerto serie y se lee desde Python y desde C#. Esa versión archivada
>   está en `_Material_de_apoyo/Tarea5_versiones_anteriores/`.
> - **La Tarea 3 de aquí registra «Motor: ON»**, que solo existe en la versión con servo. La
>   que se graba sobre la placa no lleva motor: el profesor autorizó sustituirlo.
> - Las prácticas **3, 4 y 5 que se entregan corren sobre la placa real**, no en Wokwi. Estos
>   enlaces sirven solo para las dos versiones de simulador, que van como aporte adicional.
>
> El estado real de cada entrega está en [`MATRIZ_CUMPLIMIENTO.md`](MATRIZ_CUMPLIMIENTO.md).

---

## Los enlaces

| Entrega en el aula virtual | Enlace de Wokwi |
|---|---|
| **Reto 1era** — Encender y Apagar Luz LED | https://wokwi.com/projects/472297519185086465 |
| **Reto 2da** — S.O.S con luz LED y audio | *(usa el mismo de la Tarea 1era)* |
| **Tarea 1era** — S.O.S con luz LED y audio | https://wokwi.com/projects/472297584037424129 |
| **Tarea 2da** — Música Mario Bros con luz LED y audio | https://wokwi.com/projects/472297616494006273 |
| **Tarea 3era** — Detector o Medidor de Distancia | https://wokwi.com/projects/472297684799316993 |
| **Tarea 4ta** — Distancia, Humedad, Temperatura, Colores LED | https://wokwi.com/projects/472297861760201729 |
| **Tarea 5ta** — Lectura de Datos vía otro lenguaje (Python) | https://wokwi.com/projects/472297907441424385 |

---

## Qué se comprobó de verdad

No es «tiene código». Cada uno se **ejecutó** y se leyó su salida real.

| Proyecto | Compila | Salida observada |
|---|---|---|
| Reto 1 | ✅ | `LED encendido` / `LED apagado` alternando, LED rojo visible |
| Tarea 1 | ✅ | `S ( . . . )` `O ( _ _ _ )` `S ( . . . )` + `Mensaje completo` |
| Tarea 2 | ✅ | `Notas en la melodia: 78` + `Reproduciendo el tema de Mario Bros...` |
| Tarea 3 | ✅ | `Distancia: 150.5 cm (37.6 %) \| Zona: MEDIA (parpadeo lento) \| Motor: ON` |
| Tarea 4 | ✅ | `T: 24.0 C \| H: 40 % \| Luz: 250 (OSCURO) \| Estado: SIN LUZ - S.O.S.` |
| Tarea 5 | ✅ | `[ENVIADO] {"nodo":"esp32-2024-1932","seq":3,...}` |

**Librerías instaladas y guardadas en cada proyecto:** Servo (Tarea 3), LiquidCrystal y DHT
sensor library (Tarea 4), PubSubClient y DHT sensor library (Tarea 5).

---

## La prueba más importante: la Tarea 5 extremo a extremo

El ESP32 corriendo **dentro del navegador** publicó por MQTT, y un programa de Python en la PC
recibió los mensajes:

```
Conectado al broker (rc=0); suscribiendo a itla/2024-1932/practica5/sensores
  #  9  T=24.0C  H=40.0%  luz=1001  dist=121.2cm
  # 10  T=24.0C  H=40.0%  luz=1001  dist=121.2cm

=== RESULTADO: 2 mensajes recibidos ===
```

Eso demuestra el requisito completo del enunciado: **los datos sensados por Arduino leídos desde
otro lenguaje de programación**, sin necesidad de hardware físico ni de puerto serie.

---

## Cómo demostrarlo en el video

| Tarea | Qué mover en el simulador |
|---|---|
| Reto 1 | Nada: el LED parpadea solo |
| Tarea 1 | Nada: observar el patrón `. . . _ _ _ . . .` en LED y buzzer |
| Tarea 2 | Nada: escuchar la melodía y ver el LED al ritmo |
| Tarea 3 | Clic en el **HC-SR04** → deslizador de distancia. Recorrer 400 → 10 cm |
| Tarea 4 | Clic en el **DHT** → subir temperatura a 39. Clic en el **LDR** → bajar la luz |
| Tarea 5 | Abrir dos ventanas: Wokwi corriendo y `python receptor_mqtt.py` en la terminal |

---

## Nota sobre la Tarea 5

El receptor de Python está en [`Tarea5_Arduino_Python/receptor_mqtt.py`](Tarea5_Arduino_Python/receptor_mqtt.py).

```bash
pip install paho-mqtt matplotlib
python receptor_mqtt.py
```

**Arranca primero la simulación de Wokwi**, después el Python. Y ojo: el tópico lleva tu
matrícula porque `test.mosquitto.org` es un broker público.
