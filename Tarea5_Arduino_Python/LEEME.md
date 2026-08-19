# Lectura de datos sensados vía otro lenguaje

**Tarea 5ta** · Inteligencia Artificial e Internet de las Cosas — 2026-C-2
Cristian Carrera — Matrícula 2024-1932 · Prof. Luis Bessewell Feliz

---

## Qué hace

Usted pidió salir de Arduino y leer por el puerto desde otro lenguaje, tomando una práctica ya
hecha y convirtiéndola. Reutilicé **el montaje de la Tarea 3 sin mover un cable**.

La placa, además de avisar por sonido, publica cada lectura en **CSV por el puerto serie**. Del
otro lado hay **dos lectores**: uno en Python y otro en C#. Los dos reciben, tabulan en vivo,
guardan un CSV y sacan un informe.

**El punto de la práctica:** resolví lo mismo dos veces, en dos lenguajes, y no cambié ni una
línea del programa de Arduino. La placa publica y sigue; no sabe ni le importa quién la escucha.

El informe mide lo que usted planteó con el ejemplo de la máquina de coser —*cuánto se detiene,
qué tan rápido trabaja*—: tiempo en cada zona, tiempo en marcha frente a detenida, cuántas veces
tuvo que parar y cuál fue la parada más larga.

## Qué hay en esta carpeta

| Archivo | Qué es |
|---|---|
| `Tarea5_Distancia_Serial/` | El sketch de la placa: mide, avisa y publica CSV |
| `lector_distancia.py` | El lector en **Python** |
| `LectorDistancia/` + `LectorDistancia.sln` | El lector en **C#**, para Visual Studio 2022 |
| `datos_distancia_*.csv` | Resultados reales de una sesión de captura |
| `grafica_distancia_*.png` | La gráfica de esa sesión |

## Los videos

| Video | Duración | Qué muestra |
|---|---|---|
| `Tarea5_video.mp4` | 8:18 | Funcionando y yo explicando el código |

## Cómo ejecutarlo

**1. La placa:** abrir `Tarea5_Distancia_Serial/Tarea5_Distancia_Serial.ino` en el Arduino IDE,
seleccionar **Arduino UNO** y el puerto, y pulsar **Subir**. No necesita librerías.

**2. La computadora**, cualquiera de los dos:

```
python lector_distancia.py
```

o abrir `LectorDistancia.sln` en **Visual Studio 2022** y pulsar **F5**.

> ⚠️ El puerto serie admite **un solo programa a la vez**. Si el Monitor Serie del IDE de Arduino
> está abierto, el lector falla con *acceso denegado*.

Los dos aceptan un número de segundos para que la captura se detenga sola —`dotnet run 30`— y si
no se les pasa nada, corren hasta pulsar **Ctrl+C**.

### Dos detalles del puerto serie que costaron encontrar

- **`SerialPort` de .NET no levanta la señal DTR por su cuenta, y pyserial sí.** Sin
  `DtrEnable = true` la placa no se reinicia al conectarse, y la numeración de lecturas continúa
  donde la dejó la sesión anterior en vez de empezar en 1.
- **Hay que limpiar el buffer *antes* de esperar el arranque, no después.** Al revés se pierde
  la línea de cabecera, que la placa manda una sola vez y no repite nunca.

---

## Declaración de uso de inteligencia artificial

Usé asistencia de IA (Claude) para depurar, documentar y estructurar el código. Son míos el
montaje físico, la calibración, las pruebas sobre la placa y la explicación grabada en el video.
La declaración aparece también en la cabecera del código.
