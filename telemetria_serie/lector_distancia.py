"""
================================================================================
Telemetria por puerto serie  -  lector en Python
================================================================================
Autor : Cristian Carrera

Se uso asistencia de IA (Claude) para depurar y documentar. El montaje, la
calibracion y las pruebas sobre la placa son propios.

Se conecta al Arduino por el puerto serie, recibe las lecturas del sensor de
distancia, las muestra en vivo, las guarda en un CSV y, al terminar, saca un
informe con estadisticas y una grafica.

La parte 1 es el sketch de la placa, en la carpeta de al lado:
    firmware_arduino/firmware_arduino.ino

--------------------------------------------------------------------------------
COMO SE USA
--------------------------------------------------------------------------------
    pip install pyserial matplotlib

    python lector_distancia.py            <- busca la placa sola
    python lector_distancia.py COM3       <- o se le dice el puerto

    Ctrl+C para terminar: ahi guarda, calcula e imprime el informe.

IMPORTANTE: el Monitor Serie del IDE de Arduino tiene que estar CERRADO. El
puerto solo admite un programa a la vez; si el IDE lo tiene abierto, este
programa no puede entrar.

--------------------------------------------------------------------------------
QUE APORTA PYTHON QUE EL ARDUINO NO PUEDE HACER
--------------------------------------------------------------------------------
Esta es la razon de ser del proyecto. El UNO tiene 2 KB de RAM: no puede
guardar un historico ni dibujar nada. Lo que sabe hacer es medir el mundo
fisico en tiempo real, y hacerlo bien.

Del otro lado, esta computadora no tiene forma de tocar el mundo fisico, pero
le sobra memoria y disco.

    Arduino  ->  sensa y avisa
    Python   ->  recibe, guarda, analiza y grafica

La idea viene del ejemplo de una maquina de coser: poner
un sensor y medir "cuanto se detiene, que tan rapido hace". Eso es justo lo
que calcula el informe del final: cuanto tiempo estuvo la maquina en marcha,
cuanto detenida, y cuantas veces tuvo que parar.

--------------------------------------------------------------------------------
LOS TRES PROBLEMAS QUE HAY QUE RESOLVER
--------------------------------------------------------------------------------
1. EL AUTO-RESET. Al abrir el puerto, la señal DTR reinicia la placa: arranca
   de cero justo cuando Python se conecta. Por eso se esperan unos segundos
   antes de fiarse de lo que llega.

2. LAS LINEAS PARTIDAS. El puerto entrega bytes, no lineas. Una lectura puede
   llegar cortada por la mitad, sobre todo la primera. Por eso cada linea se
   valida antes de aceptarla, y las malas se descartan sin romper nada.

3. LOS DATOS QUE NO SON DATOS. Cuando no hay eco, la placa manda -1. No es una
   distancia: es "no vi nada". Se guarda igual, pero se excluye de las medias,
   porque si no ensucia las estadisticas.
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
    print("Falta pyserial. Instalalo con:")
    print("    pip install pyserial")
    sys.exit(1)


BAUDIOS = 9600                  # tiene que coincidir con el Serial.begin()
SIN_ECO = -1.0                  # lo que manda la placa cuando no ve nada

# El sketch envia una linea cada 200 ms. Se guarda aqui para poder calcular
# el tiempo real que representa cada lectura.
PERIODO_ENVIO_MS = 200

# La placa manda su cabecera UNA sola vez, al arrancar. Si por lo que sea no
# se llega a ver -porque el programa entro cuando ya estaba enviando, o porque
# se perdio en el arranque- se usan estos nombres y se sigue adelante.
COLUMNAS_POR_DEFECTO = ["LECTURA", "MILIS", "DISTANCIA_CM", "ZONA", "ESTADO"]


# =============================================================================
# 1. Encontrar la placa
# =============================================================================
def detectar_puerto():
    """Busca un puerto que parezca un Arduino.

    Se apoya en el nombre del chip conversor USB-serie. El UNO original usa un
    ATmega16U2 y los clones -y muchos Elegoo- usan un CH340.
    """
    puertos = list(serial.tools.list_ports.comports())

    if not puertos:
        print("No se detecto ningun puerto serie.")
        print("Revisa que la placa este conectada por USB.")
        print("Con placas clonicas suele faltar el driver del CH340.")
        return None

    print("Puertos disponibles:")
    for p in puertos:
        print(f"   {p.device:10}  {p.description}")

    pistas = ("arduino", "ch340", "ch341", "usb-serial", "usb serial", "wch")
    for p in puertos:
        if any(x in p.description.lower() for x in pistas):
            print(f"\nPlaca detectada en {p.device}")
            return p.device

    # Con un solo puerto, lo mas probable es que sea ese.
    if len(puertos) == 1:
        print(f"\nSolo hay un puerto; se usara {puertos[0].device}")
        return puertos[0].device

    print("\nNo se reconocio la placa. Indica el puerto a mano:")
    print("   python lector_distancia.py COM3")
    return None


# =============================================================================
# 2. Interpretar una linea
# =============================================================================
def parsear(linea, columnas):
    """Convierte una linea CSV en un diccionario. Devuelve None si no sirve.

    Se comprueba el numero de campos ANTES de convertir nada: asi una linea
    partida por la mitad se descarta en vez de reventar el programa.
    """
    partes = [p.strip() for p in linea.split(",")]
    if len(partes) != len(columnas):
        return None

    fila = dict(zip(columnas, partes))
    try:
        fila["LECTURA"] = int(fila["LECTURA"])
        fila["MILIS"] = int(fila["MILIS"])
        fila["DISTANCIA_CM"] = float(fila["DISTANCIA_CM"])
    except (ValueError, KeyError):
        return None

    return fila


# =============================================================================
# 3. El informe: que se puede sacar de los datos
# =============================================================================
def informe(filas):
    """Estadisticas de la sesion.

    Cada lectura representa PERIODO_ENVIO_MS de tiempo real, asi que contar
    lecturas por estado equivale a medir tiempo en cada estado.
    """
    if not filas:
        print("No se recibio ninguna lectura valida.")
        return

    total = len(filas)
    segundos = total * PERIODO_ENVIO_MS / 1000

    # Las distancias reales, sin los -1 de "no vi nada".
    validas = [f["DISTANCIA_CM"] for f in filas if f["DISTANCIA_CM"] != SIN_ECO]

    print("\n" + "=" * 66)
    print("INFORME DE LA SESION")
    print("=" * 66)
    print(f"  Lecturas recibidas ....... {total}")
    print(f"  Duracion ................. {segundos:.1f} s")

    if validas:
        print(f"  Distancia minima ......... {min(validas):.1f} cm")
        print(f"  Distancia maxima ......... {max(validas):.1f} cm")
        print(f"  Distancia media .......... {sum(validas)/len(validas):.1f} cm")
    sin_eco = total - len(validas)
    print(f"  Lecturas sin eco ......... {sin_eco}  ({sin_eco/total*100:.0f} %)")

    # --- Tiempo en cada zona ------------------------------------------------
    print("\n  Tiempo en cada zona:")
    zonas = {}
    for f in filas:
        zonas[f["ZONA"]] = zonas.get(f["ZONA"], 0) + 1
    for zona, n in sorted(zonas.items(), key=lambda x: -x[1]):
        seg = n * PERIODO_ENVIO_MS / 1000
        barra = "#" * int(round(n / total * 34))
        print(f"     {zona:<9} {seg:6.1f} s  {n/total*100:5.1f} %  {barra}")

    # --- La maquina: en marcha o detenida -----------------------------------
    # Esto es lo del ejemplo de la maquina de coser:
    # medir cuanto trabaja y cuanto se detiene.
    en_marcha = sum(1 for f in filas if f["ESTADO"] == "EN_MARCHA")
    detenida = total - en_marcha

    print("\n  Estado de la maquina:")
    print(f"     EN MARCHA {en_marcha * PERIODO_ENVIO_MS/1000:6.1f} s  "
          f"({en_marcha/total*100:.0f} %)")
    print(f"     DETENIDA  {detenida * PERIODO_ENVIO_MS/1000:6.1f} s  "
          f"({detenida/total*100:.0f} %)")

    # --- Cuantas veces tuvo que parar ---------------------------------------
    # Se cuentan las TRANSICIONES, no las lecturas: pasar de EN_MARCHA a
    # DETENIDA es una parada, aunque dure veinte lecturas seguidas.
    paradas = 0
    racha = 0
    racha_larga = 0
    anterior = None
    for f in filas:
        actual = f["ESTADO"]
        if actual == "DETENIDA":
            if anterior != "DETENIDA":
                paradas += 1
                racha = 0
            racha += 1
            racha_larga = max(racha_larga, racha)
        anterior = actual

    print(f"\n     Veces que se detuvo ... {paradas}")
    if paradas:
        print(f"     Parada mas larga ...... "
              f"{racha_larga * PERIODO_ENVIO_MS/1000:.1f} s")
    print("=" * 66)


# =============================================================================
# 4. La grafica
# =============================================================================
def graficar(filas):
    """Dibuja la distancia en el tiempo, marcando cuando la maquina paro."""
    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("\n(Sin grafica: falta matplotlib. Instalalo con "
              "'pip install matplotlib')")
        return

    # Los -1 se pasan a None para que la linea se corte ahi en vez de bajar a
    # -1 y deformar toda la escala.
    t = [f["MILIS"] / 1000 for f in filas]
    d = [f["DISTANCIA_CM"] if f["DISTANCIA_CM"] != SIN_ECO else None
         for f in filas]

    fig, ax = plt.subplots(figsize=(11, 5))
    ax.plot(t, d, linewidth=1.6, color="#0C6E70", label="Distancia")

    # Se sombrea el fondo donde la maquina estuvo detenida.
    inicio = None
    for i, f in enumerate(filas):
        parado = f["ESTADO"] == "DETENIDA"
        if parado and inicio is None:
            inicio = t[i]
        elif not parado and inicio is not None:
            ax.axvspan(inicio, t[i], color="#C4392C", alpha=0.13)
            inicio = None
    if inicio is not None:
        ax.axvspan(inicio, t[-1], color="#C4392C", alpha=0.13)

    ax.set_xlabel("Tiempo (s)")
    ax.set_ylabel("Distancia (cm)")
    ax.set_title("Distancia leida desde Python\n"
                 "Las franjas rojas son los tramos con la maquina detenida",
                 fontsize=11)
    ax.grid(alpha=0.25)
    ax.legend(loc="upper right")
    fig.tight_layout()

    nombre = f"grafica_distancia_{datetime.now():%Y%m%d_%H%M%S}.png"
    fig.savefig(nombre, dpi=130)
    print(f"\nGrafica guardada en  {nombre}")
    plt.show()


# =============================================================================
# 5. Programa principal
# =============================================================================
def main():
    print("=" * 66)
    print("Telemetria por puerto serie - lector en Python")
    print("Cristian Carrera")
    print("=" * 66 + "\n")

    puerto = sys.argv[1] if len(sys.argv) > 1 else detectar_puerto()
    if not puerto:
        sys.exit(1)

    try:
        arduino = serial.Serial(puerto, BAUDIOS, timeout=2)
    except serial.SerialException as e:
        print(f"\nNo se pudo abrir {puerto}: {e}")
        print("Causas mas frecuentes:")
        print("  - El Monitor Serie del IDE de Arduino esta abierto. Cierralo.")
        print("  - Ese no es el puerto; ejecuta sin argumentos para ver la lista.")
        sys.exit(1)

    # Al abrir el puerto, DTR reinicia la placa. Se limpia lo que haya en el
    # buffer AHORA -restos sueltos del arranque- y solo despues se espera.
    #
    # El orden importa y es facil equivocarse: limpiar DESPUES de la espera
    # borra la cabecera, que para entonces ya llego. Y como la placa solo la
    # manda una vez, el programa se quedaria esperando una linea que no va a
    # volver, descartando todos los datos.
    arduino.reset_input_buffer()
    print("Esperando a que la placa arranque (auto-reset por DTR)...")
    time.sleep(2.5)

    columnas = None
    filas = []
    descartadas = 0

    print("\nLeyendo. Mueve la mano delante del sensor.")
    print("Ctrl+C para terminar y ver el informe.\n")
    print(f"{'#':>5}  {'tiempo':>8}  {'distancia':>10}  {'zona':<9}  estado")
    print("-" * 60)

    try:
        while True:
            cruda = arduino.readline().decode("utf-8", errors="ignore").strip()
            if not cruda:
                continue

            # La primera linea util es la cabecera: da los nombres de columna.
            if columnas is None:
                if cruda.startswith("LECTURA"):
                    columnas = [c.strip() for c in cruda.split(",")]
                    print(f"(cabecera recibida: {', '.join(columnas)})\n")
                    continue

                # No es la cabecera. Si aun asi parece un dato completo, se
                # sigue con los nombres conocidos en vez de esperar para
                # siempre: la placa manda la cabecera UNA sola vez y, si no se
                # llego a ver, no la va a repetir.
                if parsear(cruda, COLUMNAS_POR_DEFECTO) is not None:
                    columnas = COLUMNAS_POR_DEFECTO
                    print("(no se vio la cabecera; se usan los nombres conocidos)\n")
                else:
                    continue

            fila = parsear(cruda, columnas)
            if fila is None:
                descartadas += 1
                continue

            filas.append(fila)

            d = fila["DISTANCIA_CM"]
            texto_d = "sin eco" if d == SIN_ECO else f"{d:.1f} cm"
            print(f"{fila['LECTURA']:>5}  {fila['MILIS']/1000:>7.1f}s  "
                  f"{texto_d:>10}  {fila['ZONA']:<9}  {fila['ESTADO']}")

    except KeyboardInterrupt:
        print("\n\nDetenido.")
    finally:
        arduino.close()

    if descartadas:
        print(f"({descartadas} lineas incompletas descartadas, es normal al "
              f"arrancar)")

    if not filas:
        print("No llego ningun dato valido. Comprueba que subiste el sketch "
              "firmware_arduino a la placa.")
        return

    # --- Guardar ------------------------------------------------------------
    nombre = f"datos_distancia_{datetime.now():%Y%m%d_%H%M%S}.csv"
    with open(nombre, "w", newline="", encoding="utf-8") as f:
        escritor = csv.DictWriter(f, fieldnames=columnas)
        escritor.writeheader()
        escritor.writerows(filas)
    print(f"\n{len(filas)} lecturas guardadas en  {nombre}")

    informe(filas)
    graficar(filas)


if __name__ == "__main__":
    main()
