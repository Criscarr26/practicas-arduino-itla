"""
================================================================================
PRACTICA 5 - Lectura de los datos sensados de Arduino desde Python
             VERSION VIRTUAL (parte 2 de 2: el receptor)
================================================================================
Asignatura : Inteligencia Artificial e Internet de las Cosas (2026-C-2)
Profesor   : Luis Bessewell Feliz
Estudiante : Cristian Carrera - Matricula 2024-1932
Institucion: Instituto Tecnologico de Las Americas (ITLA)

Recibe por MQTT los datos que publica el ESP32 desde el simulador Wokwi, los
muestra en una tabla, los guarda en CSV y al cerrar dibuja una grafica.

--------------------------------------------------------------------------------
POR QUE MQTT Y NO EL PUERTO SERIE
--------------------------------------------------------------------------------
La forma clasica seria pyserial sobre el USB. Con el simulador NO se puede:
el ESP32 corre dentro del navegador y no crea ningun puerto COM en el sistema
operativo. No es un problema del codigo: el cable no existe.

La alternativa es hacer que la placa hable por WiFi, que el simulador si
permite de verdad:

    [ESP32 en el navegador] --WiFi--> [broker MQTT] --> [este programa]

El objetivo academico se cumple igual: los datos sensados por Arduino se leen
y procesan desde otro lenguaje. Y de paso la arquitectura es mas parecida a un
sistema IoT real, porque no exige que las dos maquinas esten conectadas por
cable.

--------------------------------------------------------------------------------
POR QUE JSON Y NO CSV
--------------------------------------------------------------------------------
JSON es AUTODESCRIPTIVO: cada valor viaja con su nombre. Si mañana el ESP32
agrega un sensor, este programa no se rompe, porque busca los campos por
nombre y no por posicion. Con CSV bastaria cambiar el orden de dos columnas
para que todo se corrompa en silencio.

--------------------------------------------------------------------------------
INSTALACION
--------------------------------------------------------------------------------
    pip install paho-mqtt matplotlib

--------------------------------------------------------------------------------
USO
--------------------------------------------------------------------------------
    1. Arranca PRIMERO la simulacion en Wokwi y espera a ver "ENVIADO".
    2. Ejecuta:   python receptor_mqtt.py
    3. Ctrl+C para detener: guarda el CSV y dibuja la grafica.

IMPORTANTE: el topico lleva tu matricula. Si dos personas usan el mismo, se
mezclan los datos, porque test.mosquitto.org es un broker publico.
================================================================================
"""

import csv
import json
import sys
import time
from datetime import datetime

try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("Falta la libreria paho-mqtt. Instalala con:")
    print("    pip install paho-mqtt")
    sys.exit(1)

BROKER = "test.mosquitto.org"
PUERTO = 1883
TOPICO = "itla/2024-1932/practica5/sensores"
ARCHIVO = "datos_practica5.csv"

CAMPOS = ["seq", "ms", "temperatura", "humedad", "luz", "distancia"]
SIN_DATO = -1

filas = []
primera = True


# =============================================================================
# Callbacks de MQTT
# =============================================================================
def al_conectar(cliente, datos, banderas, rc, propiedades=None):
    if rc == 0:
        print(f"Conectado al broker {BROKER}")
        cliente.subscribe(TOPICO)
        print(f"Suscrito a: {TOPICO}")
        print("\nEsperando datos del ESP32… (arranca la simulacion en Wokwi)\n")
    else:
        print(f"No se pudo conectar. Codigo: {rc}")


def al_recibir(cliente, datos, mensaje):
    """Se ejecuta en el hilo de red que crea paho-mqtt."""
    global primera

    texto = mensaje.payload.decode("utf-8", errors="replace")

    # Validar SIEMPRE lo que llega de la red. Un mensaje mal formado no debe
    # tumbar el programa: se descarta y se avisa.
    try:
        d = json.loads(texto)
    except json.JSONDecodeError:
        print(f"[AVISO] Mensaje ilegible, descartado: {texto[:60]!r}")
        return

    if not all(c in d for c in CAMPOS):
        print(f"[AVISO] Faltan campos en el mensaje, descartado")
        return

    fila = {c: d[c] for c in CAMPOS}
    filas.append(fila)

    if primera:
        print(f"{'#':>4} {'Tiempo':>9} {'Temp':>9} {'Humedad':>9} "
              f"{'Luz':>7} {'Distancia':>10}")
        print("-" * 54)
        primera = False

    print(f"{int(fila['seq']):>4} "
          f"{fila['ms']/1000:>8.1f}s "
          f"{fmt(fila['temperatura'], 'C')} "
          f"{fmt(fila['humedad'], '%', 0)} "
          f"{fmt(fila['luz'], '', 0)} "
          f"{fmt(fila['distancia'], 'cm')}")


def fmt(valor, unidad, decimales=1):
    """Muestra un guion cuando el sensor no dio lectura, en vez de un -1 feo."""
    if valor is None or valor == SIN_DATO:
        return "   --   "
    return f"{valor:6.{decimales}f}{unidad}"


# =============================================================================
# Grafica final
# =============================================================================
def graficar():
    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("\n(matplotlib no esta instalado; se omite la grafica)")
        return

    if len(filas) < 2:
        print("\n(Muy pocas lecturas para graficar)")
        return

    magnitudes = ["temperatura", "humedad", "luz", "distancia"]
    utiles = [m for m in magnitudes if any(f[m] != SIN_DATO for f in filas)]
    if not utiles:
        print("\n(Ningun sensor entrego datos validos)")
        return

    segundos = [f["ms"] / 1000.0 for f in filas]

    fig, ejes = plt.subplots(len(utiles), 1, figsize=(10, 2.4 * len(utiles)),
                             sharex=True)
    if len(utiles) == 1:
        ejes = [ejes]

    for eje, col in zip(ejes, utiles):
        # Los -1 se sustituyen por None para que la linea se CORTE ahi, en vez
        # de dibujar un pico falso hasta abajo. Dibujar el pico seria mentir
        # sobre lo que midio el sensor.
        y = [f[col] if f[col] != SIN_DATO else None for f in filas]
        eje.plot(segundos, y, marker=".", linewidth=1)
        eje.set_ylabel(col.capitalize())
        eje.grid(alpha=0.3)

    ejes[-1].set_xlabel("Tiempo desde el arranque del ESP32 (s)")
    fig.suptitle("Datos recibidos por MQTT desde Wokwi\n"
                 "Cristian Carrera 2024-1932 - ITLA", fontsize=12)
    fig.tight_layout()
    fig.savefig("grafica_practica5.png", dpi=120)
    print("Grafica guardada en: grafica_practica5.png")
    plt.show()


# =============================================================================
def main():
    print("=" * 62)
    print(" PRACTICA 5 - RECEPCION DE DATOS DE ARDUINO EN PYTHON")
    print(" Cristian Carrera - 2024-1932 - ITLA - IA e IoT 2026-C-2")
    print("=" * 62)
    print(f"Broker: {BROKER}:{PUERTO}")

    cliente = mqtt.Client(client_id=f"receptor-2024-1932-{int(time.time())}")
    cliente.on_connect = al_conectar
    cliente.on_message = al_recibir

    try:
        cliente.connect(BROKER, PUERTO, keepalive=60)
    except Exception as e:
        print(f"\nNo se pudo conectar al broker: {e}")
        print("Comprueba tu conexion a internet.")
        sys.exit(1)

    try:
        cliente.loop_forever()
    except KeyboardInterrupt:
        print("\n\nDetenido por el usuario.")
    finally:
        cliente.disconnect()

    if not filas:
        print("\nNo se recibio ninguna lectura.")
        print("Comprueba que:")
        print("  - La simulacion de Wokwi este CORRIENDO")
        print("  - El monitor serie del ESP32 muestre 'ENVIADO'")
        print("  - El topico coincida en los dos programas")
        return

    with open(ARCHIVO, "w", newline="", encoding="utf-8") as f:
        escritor = csv.DictWriter(f, fieldnames=CAMPOS)
        escritor.writeheader()
        escritor.writerows(filas)

    print(f"\n{len(filas)} lecturas guardadas en: {ARCHIVO}")
    print(f"Fecha de la sesion: {datetime.now():%d/%m/%Y %H:%M}")

    print("\nResumen:")
    for col in CAMPOS:
        if col in ("seq", "ms"):
            continue
        validos = [f[col] for f in filas if f[col] != SIN_DATO]
        if validos:
            print(f"  {col:<12} min {min(validos):7.1f}   "
                  f"max {max(validos):7.1f}   "
                  f"promedio {sum(validos)/len(validos):7.1f}")
        else:
            print(f"  {col:<12} sin lecturas validas")

    graficar()


if __name__ == "__main__":
    main()
