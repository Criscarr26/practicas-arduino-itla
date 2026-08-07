/*
  ============================================================================
  PRACTICA 4 - Medidor de temperatura y humedad con LED RGB, alarma y display
               (Opcion D del enunciado: opciones A y B, mas pantalla)
  ============================================================================
  Asignatura : Inteligencia Artificial e Internet de las Cosas (2026-C-2)
  Profesor   : Luis Bessewell Feliz
  Estudiante : Cristian Carrera - Matricula 2024-1932
  Institucion: Instituto Tecnologico de Las Americas (ITLA)
  Placa      : Elegoo UNO R3 (compatible Arduino UNO)
  Valor      : 8 puntos

  ============================================================================
  INVESTIGACION PREVIA 1: ESTRUCTURAS DE CONTROL
  ============================================================================
  if / else if -> decidir por RANGOS. Aqui clasifica la temperatura en
                  estados clinicos. switch no sabe comparar rangos.
  switch       -> elegir entre valores discretos. Aqui actua segun el estado
                  ya calculado, y ademas traduce el estado a texto.
  for          -> repetir un numero conocido de veces. Aqui recorre los tres
                  canales del LED RGB y los pasos de la alarma S.O.S.
  while        -> repetir mientras se cumpla una condicion. El propio loop()
                  de Arduino es en el fondo un while(true).

  ============================================================================
  INVESTIGACION PREVIA 2: EL MEDIDOR DE TEMPERATURA Y HUMEDAD (DHT11)
  ============================================================================
  El kit trae el DHT11, que mide las DOS magnitudes con un solo componente:

      Temperatura ... de 0 a 50 grados C, precision +/- 2 grados
      Humedad ....... de 20 % a 90 % HR,  precision +/- 5 %
      Muestreo ...... 1 lectura por segundo COMO MAXIMO
      Alimentacion .. 3.3 V a 5 V

  COMO FUNCIONA POR DENTRO:
  Lleva dos sensores en la misma capsula. Para la humedad, un sustrato entre
  dos electrodos cuya CAPACIDAD cambia al absorber vapor de agua. Para la
  temperatura, un termistor NTC, una resistencia que baja su valor al
  calentarse. Un pequeño chip lee ambos, los digitaliza y los envia.

  EL PROTOCOLO DE UN SOLO HILO:
  Lo interesante del DHT11 es que se comunica por UN SOLO CABLE con un
  protocolo propio, no es I2C ni SPI. El microcontrolador tira la linea a
  cero unos 18 ms para pedir datos, la suelta, y el sensor responde con 40
  bits. Cada bit se codifica por la DURACION del pulso en alto: unos 26
  microsegundos es un 0 y unos 70 microsegundos es un 1.
  Leer eso a mano seria un dolor de cabeza, por eso se usa una libreria.

  LIMITACION HONESTA E IMPORTANTE:
  El DHT11 es un sensor de AMBIENTE, no un termometro clinico. Su precision
  de +/- 2 grados es enorme para medir fiebre, donde la diferencia entre
  normal y grave es de un grado. En esta practica se usa para DEMOSTRAR la
  logica de umbrales, calentando el sensor con los dedos o con el aliento.
  En un producto real haria falta un MLX90614 infrarrojo o un DS18B20.

  ============================================================================
  INVESTIGACION PREVIA 3: LA PANTALLA LCD 1602
  ============================================================================
  El display del kit Elegoo es el LCD1602 de PINES PARALELOS, sin adaptador
  I2C. Eso importa: se maneja con la libreria LiquidCrystal (que ya viene con
  el IDE) y NO con LiquidCrystal_I2C.

  1602 significa 16 columnas por 2 filas. Lleva el controlador HD44780, que
  es el estandar de la industria desde los años ochenta.

  MODO DE 4 BITS:
  El controlador admite 8 bits de datos, pero se usa casi siempre en modo de
  4 bits: se manda cada byte en dos mitades. Se gastan 4 pines en vez de 8 y
  la velocidad sigue sobrando para texto.

  Los pines de control son dos:
     RS -> dice si lo que se envia es un COMANDO (0) o un CARACTER (1)
     E  -> pulso de habilitacion; el dato se lee en su flanco

  EL POTENCIOMETRO ES OBLIGATORIO:
  El pin V0 controla el contraste y necesita un potenciometro de 10k. Sin el,
  la pantalla se ve completamente en blanco o completamente en negro, y
  parece que el codigo esta mal cuando en realidad solo falta girar la
  perilla. Es el error numero uno con este display.

  ============================================================================
  INVESTIGACION PREVIA 4: EL LED RGB
  ============================================================================
  Un LED RGB son tres LEDs en una sola capsula, con una pata comun. Mezclando
  las tres intensidades con analogWrite() se consigue cualquier color.

  Hay dos variantes y hay que saber cual se tiene:
     CATODO COMUN -> la pata larga va a GND.  255 = brillo maximo
     ANODO COMUN  -> la pata larga va a 5 V.  Los valores van INVERTIDOS
  Si los colores salen al reves, se cambia la constante CATODO_COMUN de
  abajo y listo.

  Cada color lleva su propia resistencia (220 ohm). El verde y el azul
  suelen ser mas brillantes que el rojo a igualdad de corriente.

  ============================================================================
  INVESTIGACION PREVIA 5: ¿SOPORTA ARDUINO HILOS Y TAREAS?
  ============================================================================
  El ATmega328P del UNO tiene UN nucleo, sin sistema operativo: no hay hilos
  reales. Pero si hay CONCURRENCIA, que no es lo mismo que paralelismo.
  Concurrencia es que varias tareas progresen intercaladas; paralelismo es
  que se ejecuten literalmente a la vez, y eso exige varios nucleos.

  En Arduino se logra con multitarea cooperativa basada en millis(). Esta
  practica lo NECESITA: hay que leer el sensor cada segundo, refrescar la
  pantalla cada medio segundo, y hacer sonar un S.O.S. cuyos simbolos duran
  150 o 450 ms, todo a la vez. Con delay() la pantalla se congelaria mientras
  suena la alarma.

  Por eso en este programa NO HAY UN SOLO delay(), y hasta la alarma S.O.S.
  esta escrita como una maquina de estados que avanza paso a paso.

  ============================================================================
  CRITERIOS CLINICOS APLICADOS
  ============================================================================
      menos de 36.0 C ...... HIPOTERMIA  -> LED AZUL,   sin alarma
      36.0 a 37.2 C ........ NORMAL      -> LED VERDE,  sin alarma
      37.3 a 38.9 C ........ FIEBRE      -> LED ROJO,   zumbido CONTINUO
      39.0 C o mas ......... CRITICO     -> LED ROJO,   alarma S.O.S. en Morse

  ============================================================================
  MONTAJE
  ============================================================================
     DHT11:      DATA -> pin 2,  VCC -> 5V,  GND -> GND
     Zumbador pasivo: (+) -> pin 8,  (-) -> GND
     LED RGB (catodo comun): R -> pin 5, G -> pin 6, B -> pin 9
                             comun -> GND, cada color con 220 ohm

     LCD1602 (paralelo, modo 4 bits):
        VSS -> GND        VDD -> 5V
        V0  -> patilla central del potenciometro de 10k
               (los extremos del potenciometro van a 5V y GND)
        RS  -> A0         RW  -> GND        E   -> A1
        D4  -> A2         D5  -> A3         D6  -> A4      D7 -> A5
        A   -> 5V (con 220 ohm)             K   -> GND     (retroiluminacion)

  POR QUE EL LCD VA EN LOS PINES ANALOGICOS:
  Los pines A0 a A5 funcionan perfectamente como digitales (son D14 a D19).
  Poniendo ahi la pantalla quedan libres los pines PWM (5, 6, 9) para el LED
  RGB, que si necesita PWM de verdad para mezclar colores.

  CUIDADO CON LOS TEMPORIZADORES: tone() usa el Timer2, que es el mismo del
  PWM de los pines 3 y 11. Por eso el LED RGB usa los pines 5, 6 y 9 y no
  esos dos: si no, el color cambiaria solo al sonar la alarma.

  ============================================================================
  LIBRERIAS A INSTALAR (Programa -> Incluir Libreria -> Administrar)
  ============================================================================
     "DHT sensor library"      de Adafruit
     "Adafruit Unified Sensor" de Adafruit  (la pide la anterior)
     LiquidCrystal ya viene incluida con el IDE.
  ============================================================================
*/

#include <LiquidCrystal.h>
#include <DHT.h>

// ---------------------------------------------------------------------------
// Configuracion del hardware
// ---------------------------------------------------------------------------
#define PIN_DHT       2
#define TIPO_DHT      DHT11      // cambiar a DHT22 si se usa el sensor azul

const int PIN_ZUMBADOR = 8;
const int PIN_ROJO     = 5;      // PWM
const int PIN_VERDE    = 6;      // PWM
const int PIN_AZUL     = 9;      // PWM

// Poner en false si el LED RGB es de anodo comun (colores invertidos).
const bool CATODO_COMUN = true;

// LiquidCrystal(RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
DHT dht(PIN_DHT, TIPO_DHT);

// ---------------------------------------------------------------------------
// Umbrales clinicos, en grados Celsius
// ---------------------------------------------------------------------------
const float T_HIPOTERMIA = 36.0;
const float T_NORMAL_MAX = 37.2;
const float T_FIEBRE_MAX = 38.9;

// ---------------------------------------------------------------------------
// Ritmos de las tareas concurrentes (ms)
// ---------------------------------------------------------------------------
const unsigned long PERIODO_SENSOR   = 2000;  // el DHT11 no admite mas rapido
const unsigned long PERIODO_PANTALLA = 500;
const unsigned long PERIODO_SERIE    = 2000;
const unsigned long UNIDAD_MORSE     = 150;   // unidad basica del S.O.S.

// ---------------------------------------------------------------------------
// Estados clinicos
// ---------------------------------------------------------------------------
enum Estado { HIPOTERMIA, NORMAL, FIEBRE, CRITICO, SIN_LECTURA };

Estado estado         = SIN_LECTURA;
Estado estadoAnterior = SIN_LECTURA;

float temperatura = 0;
float humedad     = 0;
bool  lecturaOk   = false;

// Marcas de tiempo de cada tarea
unsigned long tSensor   = 0;
unsigned long tPantalla = 0;
unsigned long tSerie    = 0;
unsigned long tMorse    = 0;

// ---------------------------------------------------------------------------
// Patron del S.O.S. en Morse, en unidades.
// Los indices PARES suenan y los IMPARES callan, porque el patron alterna
// siempre. Guardar solo las duraciones ahorra la mitad de la tabla.
//   S = . . .   O = _ _ _   S = . . .
// ---------------------------------------------------------------------------
const uint8_t PATRON_SOS[] = {
  1, 1, 1, 1, 1, 3,     // S: tres puntos y separacion de letra
  3, 1, 3, 1, 3, 3,     // O: tres rayas y separacion de letra
  1, 1, 1, 1, 1, 7      // S: tres puntos y separacion de mensaje
};
const int PASOS_SOS = sizeof(PATRON_SOS) / sizeof(PATRON_SOS[0]);
int  pasoMorse     = 0;
bool morseArrancado = false;   // false = todavia no ha sonado el primer punto

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  // for: recorre los tres canales del LED RGB. Numero de vueltas conocido.
  const int canales[] = { PIN_ROJO, PIN_VERDE, PIN_AZUL };
  for (int i = 0; i < 3; i++) {
    pinMode(canales[i], OUTPUT);
  }
  pinMode(PIN_ZUMBADOR, OUTPUT);

  lcd.begin(16, 2);
  lcd.print(F("SmartSalud ITLA"));
  lcd.setCursor(0, 1);
  lcd.print(F("2024-1932"));

  dht.begin();

  Serial.println(F("== Practica 4: temperatura, humedad y display =="));
  Serial.println(F("Cristian Carrera - 2024-1932 - ITLA"));
  Serial.println(F("Calienta el sensor con los dedos para ver los estados."));
  Serial.println();

  // Prueba de los tres colores al arrancar, para comprobar el cableado.
  ponerColor(255, 0, 0); delay(300);
  ponerColor(0, 255, 0); delay(300);
  ponerColor(0, 0, 255); delay(300);
  ponerColor(0, 0, 0);

  lcd.clear();
}

// ---------------------------------------------------------------------------
// loop() como planificador cooperativo. Sin un solo delay().
// ---------------------------------------------------------------------------
void loop() {
  unsigned long ahora = millis();

  tareaLeerSensor(ahora);
  tareaLuzYAlarma(ahora);
  tareaPantalla(ahora);
  tareaSerie(ahora);
}

// ===========================================================================
// TAREA 1 - Leer el DHT11 y clasificar el estado
// ===========================================================================
void tareaLeerSensor(unsigned long ahora) {
  if (ahora - tSensor < PERIODO_SENSOR) return;
  tSensor = ahora;

  float h = dht.readHumidity();
  float t = dht.readTemperature();      // sin argumento devuelve Celsius

  // isnan() detecta lectura fallida: cable suelto o sensor consultado
  // demasiado rapido. Hay que comprobarlo SIEMPRE.
  if (isnan(h) || isnan(t)) {
    lecturaOk = false;
    estado = SIN_LECTURA;
    Serial.println(F("ERROR: no se pudo leer el DHT11"));
    return;
  }

  lecturaOk   = true;
  humedad     = h;
  temperatura = t;
  estado      = clasificar(t);
}

// ---------------------------------------------------------------------------
// if/else if porque las condiciones son RANGOS.
// ---------------------------------------------------------------------------
Estado clasificar(float t) {
  if (t < T_HIPOTERMIA) {
    return HIPOTERMIA;
  } else if (t <= T_NORMAL_MAX) {
    return NORMAL;
  } else if (t <= T_FIEBRE_MAX) {
    return FIEBRE;
  } else {
    return CRITICO;
  }
}

// ===========================================================================
// TAREA 2 - Color del LED y alarma sonora
// switch porque el estado ya es un valor discreto.
// ===========================================================================
void tareaLuzYAlarma(unsigned long ahora) {

  switch (estado) {

    case SIN_LECTURA:
      ponerColor(60, 60, 60);            // blanco tenue: no hay dato
      noTone(PIN_ZUMBADOR);
      reiniciarMorse();
      break;

    case HIPOTERMIA:
      ponerColor(0, 0, 255);             // AZUL
      noTone(PIN_ZUMBADOR);
      reiniciarMorse();
      break;

    case NORMAL:
      ponerColor(0, 255, 0);             // VERDE: todo bien
      noTone(PIN_ZUMBADOR);
      reiniciarMorse();
      break;

    case FIEBRE:
      ponerColor(255, 0, 0);             // ROJO
      // Zumbido CONTINUO hasta que baje, como pide el enunciado.
      // tone() sin duracion suena indefinidamente; llamarlo repetidamente
      // con la misma frecuencia no reinicia nada, asi que es seguro.
      tone(PIN_ZUMBADOR, 800);
      reiniciarMorse();
      break;

    case CRITICO:
      ponerColor(255, 0, 0);             // ROJO
      alarmaSOS(ahora);                  // S.O.S. en Morse, sin bloquear
      break;
  }
}

// ---------------------------------------------------------------------------
// Alarma S.O.S. como MAQUINA DE ESTADOS.
// No se puede usar delay() aqui: la pantalla y el sensor tienen que seguir
// funcionando mientras suena. En cada llamada se comprueba si ya toca
// avanzar al siguiente simbolo, y si no, se devuelve el control enseguida.
// ---------------------------------------------------------------------------
void alarmaSOS(unsigned long ahora) {
  // Al entrar en estado critico hay que hacer sonar YA el primer simbolo.
  // Sin esta comprobacion el programa esperaria la duracion del paso 0 antes
  // de emitir nada, y el primer punto del S.O.S. se perderia.
  if (!morseArrancado) {
    morseArrancado = true;
    tMorse = ahora;
    aplicarPasoMorse();
    return;
  }

  unsigned long duracionPaso = (unsigned long)PATRON_SOS[pasoMorse] * UNIDAD_MORSE;
  if (ahora - tMorse < duracionPaso) return;    // el paso actual sigue vigente
  tMorse = ahora;

  pasoMorse++;
  if (pasoMorse >= PASOS_SOS) pasoMorse = 0;    // el mensaje se repite
  aplicarPasoMorse();
}

// Indice par -> suena; impar -> silencio. El patron alterna siempre.
void aplicarPasoMorse() {
  if (pasoMorse % 2 == 0) {
    tone(PIN_ZUMBADOR, 1000);
  } else {
    noTone(PIN_ZUMBADOR);
  }
}

void reiniciarMorse() {
  pasoMorse      = 0;
  morseArrancado = false;
  tMorse         = millis();
}

// ===========================================================================
// TAREA 3 - Refrescar la pantalla
// El LCD es lento, por eso tiene su propia tarea y su propio ritmo.
// ===========================================================================
void tareaPantalla(unsigned long ahora) {
  if (ahora - tPantalla < PERIODO_PANTALLA) return;
  tPantalla = ahora;

  lcd.setCursor(0, 0);
  if (!lecturaOk) {
    lcd.print(F("Sensor sin dato "));
  } else {
    lcd.print(F("T:"));
    lcd.print(temperatura, 1);
    lcd.print(F("C H:"));
    lcd.print(humedad, 0);
    lcd.print(F("%  "));
  }

  lcd.setCursor(0, 1);
  lcd.print(textoEstado(estado));
}

// ---------------------------------------------------------------------------
// Segundo switch: traduce el estado a un texto de 16 caracteres justos.
// El relleno con espacios evita tener que llamar a lcd.clear(), que hace
// parpadear la pantalla de forma molesta.
// ---------------------------------------------------------------------------
const __FlashStringHelper *textoEstado(Estado e) {
  switch (e) {
    case HIPOTERMIA:  return F("HIPOTERMIA      ");
    case NORMAL:      return F("NORMAL - OK     ");
    case FIEBRE:      return F("FIEBRE! cuidado ");
    case CRITICO:     return F("CRITICO! S.O.S. ");
    case SIN_LECTURA: return F("Esperando...    ");
  }
  return F("                ");
}

// ===========================================================================
// TAREA 4 - Monitor serie
// ===========================================================================
void tareaSerie(unsigned long ahora) {
  if (estado != estadoAnterior) {
    Serial.print(F("*** CAMBIO DE ESTADO -> "));
    Serial.println(textoEstado(estado));
    estadoAnterior = estado;
  }

  if (ahora - tSerie < PERIODO_SERIE) return;
  tSerie = ahora;

  if (!lecturaOk) return;

  Serial.print(F("Temperatura: "));
  Serial.print(temperatura, 1);
  Serial.print(F(" C  |  Humedad: "));
  Serial.print(humedad, 0);
  Serial.print(F(" %  |  Estado: "));
  Serial.println(textoEstado(estado));
}

// ---------------------------------------------------------------------------
// Escribe un color en el LED RGB.
// Si el LED es de anodo comun hay que invertir los valores, porque en esa
// variante el color se enciende poniendo el pin a menos voltaje.
// ---------------------------------------------------------------------------
void ponerColor(int r, int g, int b) {
  if (!CATODO_COMUN) {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }
  analogWrite(PIN_ROJO,  r);
  analogWrite(PIN_VERDE, g);
  analogWrite(PIN_AZUL,  b);
}
