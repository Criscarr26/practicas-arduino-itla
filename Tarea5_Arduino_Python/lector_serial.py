"""
================================================================================
PRACTICA 5 - Lectura de los datos sensados de Arduino desde Python
             (parte 2 de 2: el programa lector)
================================================================================
Asignatura : Inteligencia Artificial e Internet de las Cosas (2026-C-2)
Profesor   : Luis Bessewell Feliz
Estudiante : Cristian Carrera - Matricula 2024-1932
Institucion: Instituto Tecnologico de Las Americas (ITLA)

Este programa se conecta al Arduino por el puerto serie, lee las lecturas que
la placa va enviando en formato CSV, las muestra en pantalla en una tabla, las
guarda en un archivo .csv y, al cerrar, dibuja una grafica con matplotlib.

--------------------------------------------------------------------------------
POR QUE ESTA PRACTICA TIENE SENTIDO
--------------------------------------------------------------------------------
Un microcontrolador es buenisimo midiendo el mundo fisico en tiempo real, pero
es pesimo guardando historicos y haciendo analisis: el UNO tiene 2 KB de RAM y
32 KB de programa. Una computadora es justo al reves.

La division natural del trabajo es entonces:
    Arduino  -> sensa y envia (tiempo real, contacto con el hardware)
    Python   -> recibe, almacena, analiza y visualiza

Esto es exactamente la arquitectura de un sistema IoT en pequeño, y ademas es
un caso de PROGRAMACION DISTRIBUIDA: dos programas, en dos maquinas distintas,
que no comparten memoria y se coordinan solo por mensajes.

--------------------------------------------------------------------------------
LOS TRES PROBLEMAS QUE HAY QUE RESOLVER (y que casi nadie explica)
--------------------------------------------------------------------------------
1. EL AUTO-RESET. Al abrir el puerto serie, la señal DTR reinicia la placa.
   Arduino arranca de cero justo cuando Python se conecta. Por eso hay que
   esperar unos segundos antes de leer: si no, llegan lineas cortadas.

2. LAS LINEAS INCOMPLETAS. El puerto entrega bytes, no lineas. Una lectura
   puede llegar partida por la mitad. Por eso se usa readline() y ademas se
   valida que la linea tenga el numero correcto de campos.

3. LOS BAUDIOS. Los dos extremos tienen que usar el mismo numero. Si Arduino
   manda a 9600 y Python escucha a 115200, se reciben simbolos raros. No es un
   error del programa: es que se estan leyendo los bits a destiempo.

--------------------------------------------------------------------------------
INSTALACION
--------------------------------------------------------------------------------
    pip install pyserial matplotlib

--------------------------------------------------------------------------------
USO
--------------------------------------------------------------------------------
    python lector_serial.py                 # detecta el puerto automaticamente
    python lector_serial.py COM5            # o se le indica cual
    python lector_serial.py COM5 115200     # puerto y baudios

Para detener: Ctrl+C. Al detenerse guarda el CSV y dibuja la grafica.

IMPORTANTE: cierra el Monitor Serie del Arduino IDE antes de ejecutar esto.
El puerto solo admite UN programa conectado a la vez; si el IDE lo tiene
abierto, Python dara "Acceso denegado".
================================================================================
"""

import csv
import sys
import time
from datetime import datetime

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    print("Falta la libreria pyserial. Instalala con:  pip install pyserial")
    sys.exit(1)

BAUDIOS_POR_DEFECTO = 9600
ARCHIVO_SALIDA = "datos_arduino.csv"

# Valor que Arduino envia cuando un sensor no dio lectura valida
SIN_DATO = -1


# =============================================================================
# 1. Encontrar la placa
# =============================================================================
def detectar_puerto():
    """Busca un puerto que parezca un Arduino.

    Se apoya en el nombre del chip conversor USB-serie. El UNO original usa un
    ATmega16U2 y los clones (y muchos Elegoo) usan un CH340. Si no reconoce
    ninguno, muestra la lista para que el usuario elija a mano.
    """
    puertos = list(serial.tools.list_ports.comports())

    if not puertos:
        print("No se detecto ningun puerto serie.")
        print("Revisa que la placa este conectada y que el driver este instalado.")
        print("Con placas clonicas suele faltar el driver del CH340.")
        return None

    print("Puertos disponibles:")
    for p in puertos:
        print(f"   {p.device:10}  {p.description}")

    pistas = ("arduino", "ch340", "ch341", "usb-serial", "usb serial", "wch")
    for p in puertos:
        if any(x in p.description.lower() for x in pistas):
            print(f"\nPlaca detectada en {p.device}  ({p.description})")
            return p.device

    print("\nNo se reconocio ninguna placa. Indica el puerto a mano, por ejemplo:")
    print("   python lector_serial.py COM5")
    return None


# =============================================================================
# 2. Procesar una linea CSV
# =============================================================================
def parsear(linea, columnas):
    """Convierte una linea de texto en un diccionario de valores.

    Devuelve None si la linea no sirve. Esto NO es paranoia: al abrir el
    puerto siempre llegan bytes sueltos del arranque, y una linea partida por
    la mitad reventaria el programa si se procesara a ciegas.
    """
    linea = linea.strip()
    if not linea:
        return None

    partes = linea.split(",")
    if len(partes) != len(columnas):
        return None                      # linea incompleta o cortada

    fila = {}
    for nombre, valor in zip(columnas, partes):
        try:
            fila[nombre] = float(valor)
        except ValueError:
            return None                  # habia texto donde iba un numero
    return fila


def formatear(valor, unidad, decimales=1):
    """Muestra un guion cuando el sensor no dio lectura, en vez de un -1 feo."""
    if valor is None or valor == SIN_DATO:
        return "   --   "
    return f"{valor:6.{decimales}f}{unidad}"


# =============================================================================
# 3. Grafica final
# =============================================================================
def graficar(filas, columnas):
    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("\n(matplotlib no esta instalado; se omite la grafica)")
        print("Para tenerla:  pip install matplotlib")
        return

    if len(filas) < 2:
        print("\n(Muy pocas lecturas para graficar)")
        return

    # Solo se grafican las columnas que tienen algun dato real.
    magnitudes = [c for c in columnas if c not in ("LECTURA", "MILIS")]
    utiles = [c for c in magnitudes if any(f[c] != SIN_DATO for f in filas)]

    if not utiles:
        print("\n(Ningun sensor entrego datos validos; no hay nada que graficar)")
        return

    segundos = [f["MILIS"] / 1000.0 for f in filas]

    fig, ejes = plt.subplots(len(utiles), 1, figsize=(10, 2.4 * len(utiles)),
                             sharex=True)
    if len(utiles) == 1:
        ejes = [ejes]

    for eje, col in zip(ejes, utiles):
        # Los -1 se sustituyen por None para que la linea se corte ahi en vez
        # de dibujar un pico falso hasta abajo.
        y = [f[col] if f[col] != SIN_DATO else None for f in filas]
        eje.plot(segundos, y, marker=".", linewidth=1)
        eje.set_ylabel(col.capitalize())
        eje.grid(alpha=0.3)

    ejes[-1].set_xlabel("Tiempo desde el arranque de Arduino (s)")
    fig.suptitle("Datos sensados por Arduino - Cristian Carrera 2024-1932",
                 fontsize=12)
    fig.tight_layout()

    nombre = "grafica_arduino.png"
    fig.savefig(nombre, dpi=120)
    print(f"Grafica guardada en: {nombre}")
    plt.show()


# =============================================================================
# 4. Programa principal
# =============================================================================
def main():
    puerto = sys.argv[1] if len(sys.argv) > 1 else detectar_puerto()
    if not puerto:
        sys.exit(1)

    baudios = int(sys.argv[2]) if len(sys.argv) > 2 else BAUDIOS_POR_DEFECTO

    print("=" * 72)
    print(" LECTURA DE DATOS DE ARDUINO DESDE PYTHON")
    print(" Cristian Carrera - 2024-1932 - ITLA - IA e IoT 2026-C-2")
    print("=" * 72)
    print(f"Puerto: {puerto}   Baudios: {baudios}")

    try:
        arduino = serial.Serial(puerto, baudios, timeout=2)
    except serial.SerialException as e:
        print(f"\nNo se pudo abrir {puerto}: {e}")
        print("Causas mas frecuentes:")
        print("  - El Monitor Serie del Arduino IDE esta abierto (cierralo)")
        print("  - El puerto no es ese; ejecuta sin argumentos para ver la lista")
        sys.exit(1)

    # Al abrir el puerto, la señal DTR REINICIA la placa. Hay que darle tiempo
    # a que arranque y envie su cabecera, o llegaran lineas cortadas.
    print("Esperando a que la placa arranque (auto-reset por DTR)...")
    time.sleep(2.5)
    arduino.reset_input_buffer()

    columnas = None
    filas = []

    print("\nLeyendo. Pulsa Ctrl+C para detener y guardar.\n")

    try:
        while True:
            crudo = arduino.readline().decode("utf-8", errors="replace").strip()
            if not crudo:
                continue

            # La primera linea util es la cabecera que manda Arduino. Se usa
            # para nombrar las columnas, en vez de repetirlas aqui a mano.
            if columnas is None:
                if "," in crudo and crudo.replace(",", "").replace("_", "").isalpha():
                    columnas = crudo.split(",")
                    print("Cabecera recibida:", " | ".join(columnas))
                    print("-" * 72)
                    print(f"{'#':>4}  {'Tiempo':>9}  {'Temp':>9}  "
                          f"{'Humedad':>9}  {'Luz':>7}  {'Distancia':>10}")
                    print("-" * 72)
                continue

            fila = parsear(crudo, columnas)
            if fila is None:
                continue                       # linea cortada, se descarta

            filas.append(fila)

            print(f"{int(fila['LECTURA']):>4}  "
                  f"{fila['MILIS'] / 1000.0:>8.1f}s  "
                  f"{formatear(fila['TEMPERATURA'], 'C')}  "
                  f"{formatear(fila['HUMEDAD'], '%', 0)}  "
                  f"{formatear(fila['LUZ'], '', 0)}  "
                  f"{formatear(fila['DISTANCIA'], 'cm')}")

    except KeyboardInterrupt:
        print("\n\nDetenido por el usuario.")
    finally:
        arduino.close()

    # ---- Guardar el CSV ----------------------------------------------------
    if filas and columnas:
        with open(ARCHIVO_SALIDA, "w", newline="", encoding="utf-8") as f:
            escritor = csv.DictWriter(f, fieldnames=columnas)
            escritor.writeheader()
            escritor.writerows(filas)

        print(f"\n{len(filas)} lecturas guardadas en: {ARCHIVO_SALIDA}")
        print(f"Fecha de la sesion: {datetime.now():%d/%m/%Y %H:%M}")

        # ---- Resumen estadistico ------------------------------------------
        print("\nResumen:")
        for col in columnas:
            if col in ("LECTURA", "MILIS"):
                continue
            validos = [f[col] for f in filas if f[col] != SIN_DATO]
            if validos:
                print(f"  {col:<12} min {min(validos):7.1f}   "
                      f"max {max(validos):7.1f}   "
                      f"promedio {sum(validos) / len(validos):7.1f}")
            else:
                print(f"  {col:<12} sin lecturas validas")

        graficar(filas, columnas)
    else:
        print("\nNo se recibio ninguna lectura valida.")
        print("Comprueba que la placa tenga cargado Tarea5_Fisico_Serial.ino")
        print("y que los baudios coincidan en los dos extremos.")


if __name__ == "__main__":
    main()
