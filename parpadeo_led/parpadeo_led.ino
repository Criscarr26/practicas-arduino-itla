/*
  ============================================================================
  Parpadeo de un LED
  ============================================================================
  Autor : Cristian Carrera

  Se uso asistencia de IA (Claude) para depurar y documentar. El montaje, la
  calibracion y las pruebas sobre la placa son propios.
  Entorno : simulador Wokwi (placa Arduino UNO)

  ============================================================================
  LA SENTENCIA INVESTIGADA
  ============================================================================
  Encender y apagar un LED necesita DOS sentencias que trabajan juntas:

     pinMode(pin, OUTPUT);      // se declara UNA vez, dentro de setup()
                                // le dice a la placa que ese pin va a
                                // ENTREGAR corriente en lugar de leerla

     digitalWrite(pin, HIGH);   // ENCIENDE: pone el pin en 5 V
     digitalWrite(pin, LOW);    // APAGA:    pone el pin en 0 V

  EL DETALLE QUE MAS SE OLVIDA:
  digitalWrite() por si sola no basta. Si se omite el pinMode(), el pin queda
  configurado como ENTRADA y el LED apenas brilla, porque una entrada solo
  puede entregar unos pocos microamperios. No es que el codigo este mal: es
  que el pin no esta preparado para dar corriente.

  LA RESISTENCIA NO ES OPCIONAL:
  Un LED rojo cae unos 2 V y aguanta unos 20 mA. Sin resistencia, la
  diferencia entre los 5 V del pin y los 2 V del LED no la limita nadie, y
  circula tanta corriente que se quema el LED, el pin, o los dos.

     R = (5 V - 2 V) / 0.020 A = 150 ohmios

  Se usan 220 ohmios porque es el valor comercial mas cercano por arriba, lo
  que deja la corriente en unos 13 mA: mas seguro y el brillo apenas cambia.

  ============================================================================
  POR QUE HACE FALTA EL delay()
  ============================================================================
  El ATmega328P ejecuta 16 millones de instrucciones por segundo. Sin una
  pausa, el LED se encenderia y apagaria tan rapido que el ojo humano lo
  veria como una luz continua a media intensidad, no como un parpadeo.

  El ojo deja de distinguir parpadeos por encima de unos 25 Hz. Por eso la
  pausa tiene que ser de al menos unas decenas de milisegundos.

  NOTA IMPORTANTE PARA LOS PROYECTOS SIGUIENTES:
  delay() BLOQUEA. Mientras dura, el programa no hace absolutamente nada mas:
  no lee sensores, no atiende botones. Aqui es aceptable porque el programa
  tiene una sola tarea. En cuanto haya dos cosas que atender a la vez habra
  que sustituirlo por millis(), como se hace en los proyectos con sensores.

  ============================================================================
  MONTAJE
  ============================================================================
     Pin 13 ---- (+) LED (-) ---- Resistencia 220 ohm ---- GND

  El pin 13 del UNO ya trae un LED soldado en la propia placa, asi que el
  parpadeo se ve aunque no se conecte nada. El LED externo sirve para
  demostrar que se domina el montaje.
  ============================================================================
*/

// Se usa const int y no #define porque asi la variable tiene tipo y el
// compilador puede avisar si se usa mal.
const int PIN_LED = 13;

// Tiempos en milisegundos. Al estar con nombre, cambiar la velocidad del
// parpadeo es tocar un solo numero.
const int TIEMPO_ENCENDIDO = 1000;
const int TIEMPO_APAGADO   = 1000;

// ---------------------------------------------------------------------------
// setup() se ejecuta UNA sola vez, al encender la placa o al pulsar reset.
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  // La sentencia investigada: declarar el pin como SALIDA.
  pinMode(PIN_LED, OUTPUT);

  Serial.println(F("== Parpadeo de un LED =="));
  Serial.println(F("Cristian Carrera"));
  Serial.println();
}

// ---------------------------------------------------------------------------
// loop() se repite indefinidamente mientras la placa tenga corriente.
// Es, en el fondo, un while(true) que Arduino escribe por nosotros.
// ---------------------------------------------------------------------------
void loop() {
  digitalWrite(PIN_LED, HIGH);       // ENCENDER: el pin pasa a 5 V
  Serial.println(F("LED encendido"));
  delay(TIEMPO_ENCENDIDO);           // esperar con el LED encendido

  digitalWrite(PIN_LED, LOW);        // APAGAR: el pin vuelve a 0 V
  Serial.println(F("LED apagado"));
  delay(TIEMPO_APAGADO);             // esperar con el LED apagado
}
