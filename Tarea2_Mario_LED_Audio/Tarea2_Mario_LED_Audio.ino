/*
  ============================================================================
  TAREA 2 - Melodia introductoria de Super Mario Bros con luz y sonido
  ============================================================================
  Asignatura : Inteligencia Artificial e Internet de las Cosas (2026-C-2)
  Profesor   : Luis Bessewell Feliz
  Estudiante : Cristian Carrera - Matricula 2024-1932
  Institucion: Instituto Tecnologico de Las Americas (ITLA)
  Placa      : Elegoo UNO R3 (compatible Arduino UNO)
  Duracion   : aproximadamente 30 segundos

  ============================================================================
  INVESTIGACION PREVIA (lo que pide el enunciado)
  ============================================================================

  1) SENTENCIA PARA ENCENDER Y APAGAR UN LED
  ------------------------------------------
  Son dos sentencias que trabajan juntas:

     pinMode(pin, OUTPUT);        // se declara UNA vez, dentro de setup()
                                  // le dice a la placa que ese pin va a
                                  // ENTREGAR corriente en lugar de leerla

     digitalWrite(pin, HIGH);     // ENCIENDE: pone el pin en 5 V
     digitalWrite(pin, LOW);      // APAGA:    pone el pin en 0 V

  Detalle importante: digitalWrite() por si sola no basta. Si se olvida el
  pinMode(), el pin queda como entrada y el LED apenas brilla, porque una
  entrada solo puede entregar unos pocos microamperios.

  El LED SIEMPRE lleva una resistencia en serie (220 ohmios en este montaje).
  Sin ella circula demasiada corriente y se quema el LED, el pin, o ambos.

  2) SENTENCIA PARA EMITIR UN TONO CON DURACION Y PARA SILENCIARLO
  ----------------------------------------------------------------
     tone(pin, frecuencia);              // suena INDEFINIDAMENTE
     tone(pin, frecuencia, duracion);    // suena "duracion" milisegundos
     noTone(pin);                        // SILENCIA el pin

  Como funciona por dentro: tone() no reproduce audio. Genera una onda
  cuadrada, es decir, enciende y apaga el pin miles de veces por segundo. Si
  lo hace 440 veces por segundo, el zumbador vibra a 440 Hz y oimos un LA.
  Por eso hace falta un ZUMBADOR PASIVO: el activo trae su propio oscilador
  y siempre suena igual, no puede cambiar de nota.

  Tres detalles que hay que conocer:

    a) tone() con duracion NO BLOQUEA. La sentencia devuelve el control de
       inmediato y el tono sigue sonando por debajo, gracias a un temporizador
       del microcontrolador. Por eso hay que poner un delay() detras si se
       quiere esperar a que termine.

    b) tone() usa el Timer2 del ATmega328P, el mismo que usa la funcion
       analogWrite() en los pines 3 y 11. Mientras suena un tono, esos dos
       pines pierden su capacidad de PWM.

    c) Solo puede sonar UN tono a la vez en toda la placa. Llamar a tone()
       en otro pin corta el anterior.

  ============================================================================
  MONTAJE
  ============================================================================
     Pin 8  ---- (+) Zumbador PASIVO (-) ---- GND
     Pin 13 ---- (+) LED (-) ---- Resistencia 220 ohm ---- GND

  El pin 13 del UNO ya trae un LED soldado en la placa, asi que la melodia se
  ve aunque no se conecte el LED externo.

  ============================================================================
  COMO ESTA HECHA LA MELODIA
  ============================================================================
  Una melodia son dos listas que se recorren en paralelo:

     notas[]     -> la frecuencia de cada nota, en hercios (0 = silencio)
     duraciones[]-> cuanto dura cada nota, como divisor de una redonda
                    (4 = negra, 8 = corchea, 16 = semicorchea)

  El calculo del enunciado clasico de Arduino es:

     duracionNota = 1000 / duraciones[i];

  o sea: una negra (4) dura 1000/4 = 250 ms.
  ============================================================================
*/

// ---------------------------------------------------------------------------
// Frecuencias de las notas, en hercios. Se definen solo las que usa la
// melodia, para no arrastrar un archivo pitches.h completo.
// La letra S significa sostenido (#). El numero es la octava.
// ---------------------------------------------------------------------------
#define NOTE_C4   262
#define NOTE_E4   330
#define NOTE_G4   392
#define NOTE_A4   440
#define NOTE_AS4  466
#define NOTE_B4   494
#define NOTE_C5   523
#define NOTE_D5   587
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_G5   784
#define NOTE_A5   880
#define NOTE_AS5  932
#define NOTE_B5   988
#define NOTE_C6  1047
#define NOTE_D6  1175
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_G6  1568
#define NOTE_A6  1760
#define REPOSO      0    // silencio

// ---------------------------------------------------------------------------
// Mapa de pines
// ---------------------------------------------------------------------------
const int PIN_ZUMBADOR = 8;
const int PIN_LED      = 13;

// ---------------------------------------------------------------------------
// La melodia. Es el tema principal de Super Mario Bros (Koji Kondo, 1985),
// transcrito nota por nota. Suena unos 30 segundos con el tempo de abajo.
// ---------------------------------------------------------------------------
const int notas[] = {
  // ---- Frase inicial: el "ta-ta-tan" que todo el mundo reconoce ----
  NOTE_E6, NOTE_E6, REPOSO,  NOTE_E6,
  REPOSO,  NOTE_C6, NOTE_E6, REPOSO,
  NOTE_G6, REPOSO,  REPOSO,  REPOSO,
  NOTE_G5, REPOSO,  REPOSO,  REPOSO,

  // ---- Segunda frase ----
  NOTE_C6, REPOSO,  REPOSO,  NOTE_G5,
  REPOSO,  REPOSO,  NOTE_E5, REPOSO,
  REPOSO,  NOTE_A5, REPOSO,  NOTE_B5,
  REPOSO,  NOTE_AS5, NOTE_A5, REPOSO,

  // ---- Tercera frase: la parte que sube ----
  NOTE_G5, NOTE_E6, NOTE_G6,
  NOTE_A6, REPOSO,  NOTE_F6, NOTE_G6,
  REPOSO,  NOTE_E6, REPOSO,  NOTE_C6,
  NOTE_D6, NOTE_B5, REPOSO,  REPOSO,

  // ---- Repeticion de la segunda frase ----
  NOTE_C6, REPOSO,  REPOSO,  NOTE_G5,
  REPOSO,  REPOSO,  NOTE_E5, REPOSO,
  REPOSO,  NOTE_A5, REPOSO,  NOTE_B5,
  REPOSO,  NOTE_AS5, NOTE_A5, REPOSO,

  // ---- Cierre ----
  NOTE_G5, NOTE_E6, NOTE_G6,
  NOTE_A6, REPOSO,  NOTE_F6, NOTE_G6,
  REPOSO,  NOTE_E6, REPOSO,  NOTE_C6,
  NOTE_D6, NOTE_B5, REPOSO,  REPOSO
};

// Duracion de cada nota, como divisor: 12 equivale a una semicorchea rapida,
// 9 a una figura algo mas larga. Debe tener EXACTAMENTE el mismo numero de
// elementos que el arreglo de notas.
const int duraciones[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

   9,  9,  9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

   9,  9,  9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12
};

// El compilador calcula solo cuantas notas hay. Asi, si mañana se agrega o
// se quita una nota, no hay que acordarse de actualizar ningun numero.
const int TOTAL_NOTAS = sizeof(notas) / sizeof(notas[0]);

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  // Aqui se aplica la sentencia investigada en el punto 1: declarar el pin
  // como salida ANTES de intentar encender nada.
  pinMode(PIN_LED,      OUTPUT);
  pinMode(PIN_ZUMBADOR, OUTPUT);

  Serial.println(F("== Tarea 2: Super Mario Bros con luz y sonido =="));
  Serial.println(F("Cristian Carrera - 2024-1932 - ITLA"));
  Serial.print(F("Notas en la melodia: "));
  Serial.println(TOTAL_NOTAS);
  Serial.println();

  // Comprobacion de seguridad: si los dos arreglos no miden lo mismo, el
  // programa leeria memoria que no le pertenece. Mejor detenerse y avisar.
  if (TOTAL_NOTAS != (int)(sizeof(duraciones) / sizeof(duraciones[0]))) {
    Serial.println(F("ERROR: notas[] y duraciones[] no tienen el mismo tamano"));
    while (true) {          // parpadeo de error, el programa no continua
      digitalWrite(PIN_LED, HIGH);
      delay(100);
      digitalWrite(PIN_LED, LOW);
      delay(100);
    }
  }
}

// ---------------------------------------------------------------------------
void loop() {
  reproducirMelodia();

  // Silencio de 2 segundos antes de repetir, para que se note donde termina.
  noTone(PIN_ZUMBADOR);
  digitalWrite(PIN_LED, LOW);
  delay(2000);
}

// ---------------------------------------------------------------------------
// Recorre la melodia con un bucle for: la estructura de control adecuada
// cuando se sabe de antemano cuantas repeticiones hay que dar.
// ---------------------------------------------------------------------------
void reproducirMelodia() {
  Serial.println(F("Reproduciendo el tema de Mario Bros..."));

  for (int i = 0; i < TOTAL_NOTAS; i++) {

    // 1000 ms dividido entre el tipo de figura da la duracion en ms.
    int duracionNota = 1000 / duraciones[i];

    if (notas[i] == REPOSO) {
      // Silencio: ni suena ni se ilumina. Un silencio tambien es musica.
      noTone(PIN_ZUMBADOR);
      digitalWrite(PIN_LED, LOW);
    } else {
      // Aqui estan las dos sentencias investigadas, trabajando juntas:
      // el tono con su duracion, y el LED encendido mientras suena.
      tone(PIN_ZUMBADOR, notas[i], duracionNota);
      digitalWrite(PIN_LED, HIGH);
    }

    // Como tone() no bloquea, hay que esperar aqui a que la nota termine.
    // Se deja un 30 % extra de silencio entre notas; sin esa separacion las
    // notas se pegan y la melodia se vuelve irreconocible.
    int pausaEntreNotas = duracionNota * 1.30;
    delay(pausaEntreNotas);

    // Se apagan las dos salidas antes de pasar a la siguiente nota.
    noTone(PIN_ZUMBADOR);
    digitalWrite(PIN_LED, LOW);
  }

  Serial.println(F("Melodia terminada."));
}
