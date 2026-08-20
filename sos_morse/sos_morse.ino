/*
  ============================================================================
  S.O.S. en codigo Morse, con luz y sonido
  ============================================================================
  Autor : Cristian Carrera
  Placa : Elegoo UNO R3 (compatible Arduino UNO)

  Se uso asistencia de IA (Claude) para depurar y documentar. El montaje, la
  calibracion y las pruebas sobre la placa son propios.

  ============================================================================
  INVESTIGACION PREVIA
  ============================================================================

  1) SENTENCIA PARA ENCENDER Y APAGAR UN LED
  ------------------------------------------
     pinMode(pin, OUTPUT);      // en setup(): el pin pasa a ser salida
     digitalWrite(pin, HIGH);   // ENCIENDE (5 V)
     digitalWrite(pin, LOW);    // APAGA    (0 V)

  Sin el pinMode() el pin queda como entrada y el LED apenas brilla, porque
  una entrada solo entrega microamperios. Y el LED siempre lleva resistencia
  en serie (220 ohmios aqui); sin ella se quema el LED o el pin.

  2) SENTENCIA PARA EMITIR UN TONO CON DURACION Y SILENCIARLO
  -----------------------------------------------------------
     tone(pin, frecuencia);              // suena hasta que algo lo pare
     tone(pin, frecuencia, duracion);    // suena "duracion" milisegundos
     noTone(pin);                        // silencia

  tone() genera una onda cuadrada: enciende y apaga el pin a la frecuencia
  pedida. Necesita un ZUMBADOR PASIVO (el activo trae oscilador propio e
  ignora la frecuencia). Importante: tone() con duracion NO bloquea, devuelve
  el control enseguida y el tono sigue por debajo con un temporizador.

  ============================================================================
  EL CODIGO MORSE Y POR QUE EL S.O.S. ES ASI
  ============================================================================
  El S.O.S. no son las iniciales de ninguna frase. Se eligio en 1906
  precisamente porque su patron en Morse es el mas simple e inconfundible:

        S = tres puntos    . . .
        O = tres rayas     _ _ _
        S = tres puntos    . . .

  El estandar internacional fija las proporciones respecto a la unidad basica,
  que es la duracion de un punto:

        punto ................... 1 unidad
        raya .................... 3 unidades
        espacio entre simbolos .. 1 unidad
        espacio entre letras .... 3 unidades
        espacio entre palabras .. 7 unidades

  En este programa la unidad son 200 ms, asi que el mensaje completo dura
  algo mas de 6 segundos y se repite. Cambiando UNIDAD se acelera o se
  ralentiza todo el mensaje sin tocar nada mas: esa es la ventaja de expresar
  los tiempos en proporciones y no en numeros sueltos.

  ============================================================================
  MONTAJE
  ============================================================================
     Pin 8  ---- (+) Zumbador PASIVO (-) ---- GND
     Pin 13 ---- (+) LED (-) ---- Resistencia 220 ohm ---- GND

  El pin 13 del UNO ya trae un LED en la placa, asi que la señal se ve aunque
  no se conecte el LED externo.
  ============================================================================
*/

// ---------------------------------------------------------------------------
// Mapa de pines
// ---------------------------------------------------------------------------
const int PIN_ZUMBADOR = 8;
const int PIN_LED      = 13;

// ---------------------------------------------------------------------------
// Parametros de la señal.
// Toda la temporizacion se deriva de UNIDAD, siguiendo el estandar Morse.
// ---------------------------------------------------------------------------
const int UNIDAD = 200;                  // duracion de un punto, en ms

const int PUNTO           = UNIDAD;      // 1 unidad
const int RAYA            = UNIDAD * 3;  // 3 unidades
const int PAUSA_SIMBOLO   = UNIDAD;      // entre puntos y rayas de una letra
const int PAUSA_LETRA     = UNIDAD * 3;  // entre S, O y S
const int PAUSA_MENSAJE   = UNIDAD * 7;  // antes de repetir el S.O.S.

const int FRECUENCIA = 1000;             // 1000 Hz, el pitido clasico de radio

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  // Sentencia investigada en el punto 1: declarar los pines como salida.
  pinMode(PIN_LED,      OUTPUT);
  pinMode(PIN_ZUMBADOR, OUTPUT);

  Serial.println(F("== S.O.S. en codigo Morse =="));
  Serial.println(F("Cristian Carrera"));
  Serial.print(F("Unidad basica: "));
  Serial.print(UNIDAD);
  Serial.println(F(" ms"));
  Serial.println();
}

// ---------------------------------------------------------------------------
void loop() {
  Serial.println(F("--- Enviando S.O.S. ---"));

  emitirLetraS();
  delay(PAUSA_LETRA);

  emitirLetraO();
  delay(PAUSA_LETRA);

  emitirLetraS();

  Serial.println(F("Mensaje completo. Esperando para repetir."));
  Serial.println();
  delay(PAUSA_MENSAJE);
}

// ---------------------------------------------------------------------------
// Letra S: tres puntos.
// Se usa un bucle for porque se sabe de antemano cuantas repeticiones son.
// ---------------------------------------------------------------------------
void emitirLetraS() {
  Serial.println(F("  S  ( . . . )"));
  for (int i = 0; i < 3; i++) {
    emitirSimbolo(PUNTO);
  }
}

// ---------------------------------------------------------------------------
// Letra O: tres rayas.
// ---------------------------------------------------------------------------
void emitirLetraO() {
  Serial.println(F("  O  ( _ _ _ )"));
  for (int i = 0; i < 3; i++) {
    emitirSimbolo(RAYA);
  }
}

// ---------------------------------------------------------------------------
// Emite UN simbolo: enciende luz y sonido a la vez durante el tiempo pedido,
// los apaga, y deja la pausa que corresponde entre simbolos.
//
// Todo el programa se reduce a esta funcion: las dos sentencias investigadas,
// sincronizadas. Al recibir la duracion como parametro, la misma funcion
// sirve para el punto y para la raya; lo unico que cambia es el numero.
// ---------------------------------------------------------------------------
void emitirSimbolo(int duracion) {
  // --- Encender: luz y sonido juntos -------------------------------------
  digitalWrite(PIN_LED, HIGH);              // sentencia del punto 1
  tone(PIN_ZUMBADOR, FRECUENCIA, duracion); // sentencia del punto 2

  // tone() no bloquea, asi que hay que esperar aqui a que termine el simbolo.
  delay(duracion);

  // --- Apagar -------------------------------------------------------------
  digitalWrite(PIN_LED, LOW);
  noTone(PIN_ZUMBADOR);                     // silenciar, del punto 2

  // --- Separacion antes del siguiente simbolo -----------------------------
  delay(PAUSA_SIMBOLO);
}
