/*
  ============================================================================
  TAREA 4 - Temperatura, humedad, luz y display
               Opciones A + B + C + D del enunciado, las cuatro
  ============================================================================
  Asignatura : Inteligencia Artificial e Internet de las Cosas (2026-C-2)
  Profesor   : Luis Bessewell Feliz
  Estudiante : Cristian Carrera - Matricula 2024-1932
  Institucion: Instituto Tecnologico de Las Americas (ITLA)
  Entorno    : simulador Wokwi (Arduino Cloud NO tiene simulador; ver el .md)
  Valor      : 8 puntos

  ============================================================================
  QUE CUBRE (las cuatro opciones a la vez)
  ============================================================================
    A) Temperatura -> LED verde/rojo, zumbido continuo, S.O.S. en critico
    B) Humedad     -> misma logica de tres estados, con umbrales propios
    C) Luz         -> S.O.S. cuando NO hay luz en el ambiente
    D) Display     -> LCD 16x2 mostrando valores y estado, actualizado solo

  ============================================================================
  INVESTIGACION PREVIA 1: ESTRUCTURAS DE CONTROL
  ============================================================================
    if / else if -> decide por RANGOS. Clasifica temperatura y humedad.
    switch       -> elige entre VALORES discretos. Actua segun el estado ya
                    calculado y traduce el estado a texto para el display.
                    OJO: switch no admite rangos, solo constantes. Por eso
                    primero se decide con if y luego se actua con switch.
    for          -> repite un numero CONOCIDO de veces. Recorre los tres
                    canales del LED RGB y los pasos del patron S.O.S.
    while        -> repite MIENTRAS se cumpla algo. El propio loop() de
                    Arduino es en el fondo un while(true).

  ============================================================================
  INVESTIGACION PREVIA 2: MEDIDOR DE TEMPERATURA Y HUMEDAD (DHT)
  ============================================================================
  Un solo componente mide las dos magnitudes.

     DHT11: temperatura 0 a 50 C (+/-2 C), humedad 20-90 % (+/-5 %)
     DHT22: temperatura -40 a 80 C (+/-0.5 C), humedad 0-100 % (+/-2 %)

  Por dentro lleva dos sensores en la misma capsula: para la humedad, un
  sustrato entre dos electrodos cuya CAPACIDAD cambia al absorber vapor de
  agua; para la temperatura, un termistor NTC, una resistencia que baja su
  valor al calentarse.

  Se comunica por UN SOLO CABLE con protocolo propio (no es I2C ni SPI): el
  micro tira la linea a cero unos 18 ms, la suelta, y el sensor responde con
  40 bits. Cada bit se codifica por la DURACION del pulso alto: ~26 us es un
  0 y ~70 us es un 1. Por eso se usa una libreria.

  LIMITE IMPORTANTE: no admite mas de una lectura por segundo. Consultarlo
  mas rapido devuelve NaN.

  ============================================================================
  INVESTIGACION PREVIA 3: DETECTOR DE AUSENCIA DE LUZ (LDR)
  ============================================================================
  Una fotorresistencia (LDR) es una resistencia que CAMBIA su valor con la
  luz que recibe: a oscuras sube a megaohmios y con luz baja a cientos de
  ohmios.

  Como Arduino solo mide VOLTAJE y no resistencia, se monta un DIVISOR DE
  TENSION: la LDR en serie con una resistencia fija de 10k. El punto medio
  va a una entrada analogica. Al cambiar la LDR, cambia el reparto del
  voltaje y analogRead() lo detecta.

  El modulo del simulador ya trae el divisor incorporado y entrega la señal
  lista en su pin AO. Devuelve 0-1023: valores BAJOS son oscuridad.

  ============================================================================
  INVESTIGACION PREVIA 4: LA PANTALLA LCD 1602
  ============================================================================
  1602 = 16 columnas x 2 filas, con controlador HD44780, estandar desde los
  años ochenta. Se usa en MODO DE 4 BITS: cada byte se manda en dos mitades,
  gastando 4 pines de datos en vez de 8.

  Pines de control:
     RS -> indica si lo enviado es COMANDO (0) o CARACTER (1)
     E  -> pulso de habilitacion; el dato se lee en su flanco

  EL POTENCIOMETRO ES OBLIGATORIO: el pin V0 controla el contraste. Sin el,
  la pantalla se ve toda blanca o toda negra y parece que el codigo falla.
  Es el error numero uno con este display.

  ============================================================================
  INVESTIGACION PREVIA 5: ¿SOPORTA ARDUINO HILOS Y TAREAS?
  ============================================================================
  El ATmega328P del UNO tiene UN nucleo, sin sistema operativo: hilos reales
  NO hay. Pero si hay CONCURRENCIA, que no es lo mismo que paralelismo.

     Secuencial  -> una instruccion tras otra, bloqueando
     Concurrente -> varias tareas PROGRESAN INTERCALADAS      (si, con millis)
     Paralelo    -> varias tareas se ejecutan A LA VEZ        (no: 1 nucleo)
     Distribuido -> tareas en MAQUINAS distintas, por mensajes (si, con red)

  Esta practica lo EXIGE: hay que leer el DHT cada 2 s, leer la luz cada
  200 ms, refrescar la pantalla cada 500 ms y hacer sonar un S.O.S. cuyos
  simbolos duran 150 o 450 ms. Todo a la vez. Con delay() la pantalla se
  congelaria mientras suena la alarma.

  Por eso NO HAY UN SOLO delay() en este programa, y hasta el S.O.S. esta
  escrito como maquina de estados que avanza paso a paso.

  ============================================================================
  UMBRALES: DE DONDE SALEN (no son inventados)
  ============================================================================
  TEMPERATURA CORPORAL - rangos clinicos de uso comun:
     < 36.0 C ........ HIPOTERMIA
     36.0 - 37.2 C ... NORMAL      (la media aceptada es 36.5-37.0)
     37.3 - 38.9 C ... FIEBRE      (precaucion)
     >= 39.0 C ....... CRITICO     (fiebre alta)

  HUMEDAD RELATIVA DE INTERIORES - la EPA de EE.UU. y la norma ASHRAE 55
  recomiendan mantener el interior entre 30 % y 60 % para limitar moho y
  acaros; por debajo de 20 % aparece irritacion respiratoria:
     30 - 60 % ....... NORMAL
     20-30 % / 60-70 % PRECAUCION
     < 20 % o > 70 % . CRITICO

  ADVERTENCIA HONESTA: el DHT11 es un sensor de AMBIENTE, no un termometro
  clinico. Su precision de +/-2 C es enorme para medir fiebre, donde un grado
  decide. Aqui se usa para DEMOSTRAR la logica de umbrales. En un producto
  real haria falta un MLX90614 infrarrojo o un DS18B20.

  ============================================================================
  PRIORIDAD DE LA ALARMA (tres subsistemas piden el mismo zumbador)
  ============================================================================
  Solo hay un buzzer y tres opciones pueden querer usarlo. Se resuelve con
  una jerarquia explicita, de mas grave a menos grave:

     1. Temperatura CRITICA .... S.O.S.
     2. Humedad CRITICA ........ S.O.S.
     3. Oscuridad (opcion C) ... S.O.S.
     4. Temperatura o humedad en PRECAUCION ... zumbido continuo
     5. Todo normal ............ silencio

  ============================================================================
  MONTAJE (verificado en Wokwi: 7 piezas, 0 errores)
  ============================================================================
     DHT      DATA -> D2      VCC -> 5V    GND -> GND
     Buzzer   (+)  -> D8      (-)  -> GND
     LED RGB  R -> D5   G -> D6   B -> D9   COM -> GND
     LDR      AO   -> A0      VCC -> 5V    GND -> GND
     LCD1602  RS -> D12   E -> D11   D4 -> D7   D5 -> D4   D6 -> D3   D7 -> D10
              VSS/RW/K -> GND    VDD/A -> 5V    V0 -> potenciometro 10k

  POR QUE ESOS PINES: el RGB necesita PWM de verdad, y se le reservan 5, 6 y
  9. Se evitan los pines 3 y 11 para PWM porque tone() usa el Timer2, que es
  el mismo del PWM en esos dos pines: el color cambiaria solo al sonar la
  alarma. El LCD si puede ir en 3 y 11 porque solo usa digitalWrite.

  ============================================================================
  LIBRERIAS
  ============================================================================
     "DHT sensor library" de Adafruit + "Adafruit Unified Sensor"
     LiquidCrystal viene incluida con el IDE.
  ============================================================================
*/

#include <LiquidCrystal.h>
#include <DHT.h>

// ---------------------------------------------------------------------------
// Hardware
// ---------------------------------------------------------------------------
#define PIN_DHT   2
#define TIPO_DHT  DHT22        // en Wokwi la pieza es DHT22; con el kit fisico
                               // del Elegoo cambiar a DHT11

const int PIN_ZUMBADOR = 8;
const int PIN_ROJO     = 5;    // PWM
const int PIN_VERDE    = 6;    // PWM
const int PIN_AZUL     = 9;    // PWM
const int PIN_LDR      = A0;

const bool CATODO_COMUN = true;   // false si el RGB es de anodo comun

// LiquidCrystal(RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 7, 4, 3, 10);
DHT dht(PIN_DHT, TIPO_DHT);

// ---------------------------------------------------------------------------
// Umbrales justificados arriba
// ---------------------------------------------------------------------------
const float T_HIPOTERMIA = 36.0;
const float T_NORMAL_MAX = 37.2;
const float T_FIEBRE_MAX = 38.9;

const float H_CRITICA_BAJA = 20.0;
const float H_NORMAL_MIN   = 30.0;
const float H_NORMAL_MAX   = 60.0;
const float H_CRITICA_ALTA = 70.0;

// analogRead: por debajo de esto se considera que NO hay luz
const int UMBRAL_OSCURIDAD = 300;

// ---------------------------------------------------------------------------
// Ritmos de las tareas concurrentes (ms). Ningun delay() en todo el programa.
// ---------------------------------------------------------------------------
const unsigned long PERIODO_DHT      = 2000;   // el DHT no admite mas rapido
const unsigned long PERIODO_LUZ      = 200;
const unsigned long PERIODO_PANTALLA = 500;
const unsigned long PERIODO_SERIE    = 2000;
const unsigned long UNIDAD_MORSE     = 150;

// ---------------------------------------------------------------------------
// Estados
// ---------------------------------------------------------------------------
enum Nivel { SIN_LECTURA, NORMAL, PRECAUCION, CRITICO };

// Que alarma esta sonando ahora mismo
enum Alarma { SIN_ALARMA, ZUMBIDO, SOS };

float temperatura = 0, humedad = 0;
int   luz = 0;
bool  hayLuz = true;
bool  lecturaOk = false;

Nivel nivelTemp = SIN_LECTURA;
Nivel nivelHum  = SIN_LECTURA;
Alarma alarmaActual = SIN_ALARMA, alarmaAnterior = SIN_ALARMA;

unsigned long tDht = 0, tLuz = 0, tPantalla = 0, tSerie = 0, tMorse = 0;
bool alternaPantalla = false;

// ---------------------------------------------------------------------------
// Patron S.O.S. en Morse, en unidades. Los indices PARES suenan y los
// IMPARES callan, porque el patron alterna siempre: guardar solo las
// duraciones ahorra la mitad de la tabla.
//    S = . . .    O = _ _ _    S = . . .
// ---------------------------------------------------------------------------
const uint8_t PATRON_SOS[] = {
  1, 1, 1, 1, 1, 3,     // S y separacion de letra
  3, 1, 3, 1, 3, 3,     // O y separacion de letra
  1, 1, 1, 1, 1, 7      // S y separacion de mensaje
};
const int PASOS_SOS = sizeof(PATRON_SOS) / sizeof(PATRON_SOS[0]);
int  pasoMorse = 0;
bool morseArrancado = false;

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  // for: recorre los tres canales del RGB. Numero de vueltas conocido.
  const int canales[] = { PIN_ROJO, PIN_VERDE, PIN_AZUL };
  for (int i = 0; i < 3; i++) pinMode(canales[i], OUTPUT);
  pinMode(PIN_ZUMBADOR, OUTPUT);

  lcd.begin(16, 2);
  lcd.print(F("SmartSalud ITLA"));
  lcd.setCursor(0, 1);
  lcd.print(F("2024-1932"));

  dht.begin();

  Serial.println(F("== Tarea 4: temperatura, humedad, luz y display =="));
  Serial.println(F("Cristian Carrera - 2024-1932 - ITLA"));
  Serial.println(F("Opciones A + B + C + D del enunciado"));
  Serial.println();

  // Prueba de los tres colores: comprueba el cableado del RGB de un vistazo.
  ponerColor(255, 0, 0); delay(300);
  ponerColor(0, 255, 0); delay(300);
  ponerColor(0, 0, 255); delay(300);
  ponerColor(0, 0, 0);

  lcd.clear();
}

// ---------------------------------------------------------------------------
// loop() = planificador cooperativo. Sin un solo delay().
// ---------------------------------------------------------------------------
void loop() {
  unsigned long ahora = millis();

  tareaLeerDHT(ahora);        // opciones A y B
  tareaLeerLuz(ahora);        // opcion C
  tareaDecidirAlarma();       // jerarquia entre las tres
  tareaLuzYSonido(ahora);
  tareaPantalla(ahora);       // opcion D
  tareaSerie(ahora);
}

// ===========================================================================
// TAREA 1 - Temperatura y humedad (opciones A y B)
// ===========================================================================
void tareaLeerDHT(unsigned long ahora) {
  if (ahora - tDht < PERIODO_DHT) return;
  tDht = ahora;

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // isnan detecta lectura fallida: cable suelto o consulta demasiado rapida.
  // Hay que comprobarlo SIEMPRE.
  if (isnan(h) || isnan(t)) {
    lecturaOk = false;
    nivelTemp = nivelHum = SIN_LECTURA;
    Serial.println(F("ERROR: no se pudo leer el sensor DHT"));
    return;
  }

  lecturaOk   = true;
  temperatura = t;
  humedad     = h;
  nivelTemp   = clasificarTemperatura(t);
  nivelHum    = clasificarHumedad(h);
}

// if/else if porque las condiciones son RANGOS.
Nivel clasificarTemperatura(float t) {
  if (t < T_HIPOTERMIA)      return PRECAUCION;   // hipotermia tambien alerta
  else if (t <= T_NORMAL_MAX) return NORMAL;
  else if (t <= T_FIEBRE_MAX) return PRECAUCION;
  else                        return CRITICO;
}

Nivel clasificarHumedad(float h) {
  if (h < H_CRITICA_BAJA)      return CRITICO;
  else if (h < H_NORMAL_MIN)   return PRECAUCION;
  else if (h <= H_NORMAL_MAX)  return NORMAL;
  else if (h <= H_CRITICA_ALTA) return PRECAUCION;
  else                          return CRITICO;
}

// ===========================================================================
// TAREA 2 - Detector de ausencia de luz (opcion C)
// ===========================================================================
void tareaLeerLuz(unsigned long ahora) {
  if (ahora - tLuz < PERIODO_LUZ) return;
  tLuz = ahora;

  luz    = analogRead(PIN_LDR);
  hayLuz = (luz >= UMBRAL_OSCURIDAD);
}

// ===========================================================================
// TAREA 3 - Jerarquia de la alarma
// Un solo zumbador y tres subsistemas que lo quieren. Se decide aqui, en un
// solo sitio, en vez de que cada tarea escriba por su cuenta y se pisen.
// ===========================================================================
void tareaDecidirAlarma() {
  if (nivelTemp == CRITICO || nivelHum == CRITICO) {
    alarmaActual = SOS;                       // 1 y 2: lo mas grave
  } else if (!hayLuz) {
    alarmaActual = SOS;                       // 3: oscuridad (opcion C)
  } else if (nivelTemp == PRECAUCION || nivelHum == PRECAUCION) {
    alarmaActual = ZUMBIDO;                   // 4: precaucion
  } else {
    alarmaActual = SIN_ALARMA;                // 5
  }

  // Al cambiar de alarma se reinicia el S.O.S. para que empiece por el
  // primer punto y no a mitad del mensaje.
  if (alarmaActual != alarmaAnterior) {
    pasoMorse = 0;
    morseArrancado = false;
    noTone(PIN_ZUMBADOR);
    alarmaAnterior = alarmaActual;
  }
}

// ===========================================================================
// TAREA 4 - Color del LED y sonido
// switch porque la alarma ya es un valor discreto.
// ===========================================================================
void tareaLuzYSonido(unsigned long ahora) {

  // --- Color: manda el nivel mas grave entre temperatura y humedad -------
  Nivel peor = (nivelTemp > nivelHum) ? nivelTemp : nivelHum;

  if (!hayLuz && peor == NORMAL) {
    ponerColor(0, 0, 120);              // azul tenue: oscuridad, sin fiebre
  } else {
    switch (peor) {
      case SIN_LECTURA: ponerColor(60, 60, 60); break;   // blanco tenue
      case NORMAL:      ponerColor(0, 255, 0);  break;   // VERDE
      case PRECAUCION:  ponerColor(255, 90, 0); break;   // NARANJA
      case CRITICO:     ponerColor(255, 0, 0);  break;   // ROJO
    }
  }

  // --- Sonido -----------------------------------------------------------
  switch (alarmaActual) {
    case SIN_ALARMA:
      noTone(PIN_ZUMBADOR);
      break;

    case ZUMBIDO:
      // Tono CONTINUO mientras dure el estado, como pide el enunciado.
      // Llamar a tone() repetidamente con la misma frecuencia no reinicia
      // nada, asi que es seguro hacerlo en cada vuelta.
      tone(PIN_ZUMBADOR, 800);
      break;

    case SOS:
      alarmaSOS(ahora);
      break;
  }
}

// ---------------------------------------------------------------------------
// S.O.S. como MAQUINA DE ESTADOS: no puede usar delay() porque la pantalla y
// los sensores tienen que seguir funcionando mientras suena.
// ---------------------------------------------------------------------------
void alarmaSOS(unsigned long ahora) {
  // Al entrar hay que hacer sonar YA el primer simbolo. Sin esta
  // comprobacion se esperaria la duracion del paso 0 antes de emitir nada
  // y el primer punto se perderia.
  if (!morseArrancado) {
    morseArrancado = true;
    tMorse = ahora;
    aplicarPasoMorse();
    return;
  }

  unsigned long duracion = (unsigned long)PATRON_SOS[pasoMorse] * UNIDAD_MORSE;
  if (ahora - tMorse < duracion) return;
  tMorse = ahora;

  pasoMorse++;
  if (pasoMorse >= PASOS_SOS) pasoMorse = 0;
  aplicarPasoMorse();
}

void aplicarPasoMorse() {
  if (pasoMorse % 2 == 0) tone(PIN_ZUMBADOR, 1000);
  else                    noTone(PIN_ZUMBADOR);
}

// ===========================================================================
// TAREA 5 - Display (opcion D)
// El LCD es lento, por eso tiene su propia tarea y su propio ritmo.
// Alterna dos pantallas para caber en 16x2 sin recortar informacion.
// ===========================================================================
void tareaPantalla(unsigned long ahora) {
  if (ahora - tPantalla < PERIODO_PANTALLA) return;
  tPantalla = ahora;

  // Cada dos refrescos cambia de pantalla (1 segundo por pantalla)
  static uint8_t contador = 0;
  if (++contador >= 2) { contador = 0; alternaPantalla = !alternaPantalla; }

  lcd.setCursor(0, 0);
  if (!lecturaOk) {
    lcd.print(F("Sensor sin dato "));
  } else if (!alternaPantalla) {
    lcd.print(F("T:"));
    lcd.print(temperatura, 1);
    lcd.print(F("C H:"));
    lcd.print(humedad, 0);
    lcd.print(F("% "));
  } else {
    lcd.print(F("Luz:"));
    lcd.print(luz);
    lcd.print(hayLuz ? F(" CLARO  ") : F(" OSCURO "));
    lcd.print(F("   "));
  }

  lcd.setCursor(0, 1);
  lcd.print(textoEstado());
}

// ---------------------------------------------------------------------------
// Texto de 16 caracteres justos. El relleno con espacios evita llamar a
// lcd.clear(), que hace parpadear la pantalla de forma molesta.
// ---------------------------------------------------------------------------
const __FlashStringHelper *textoEstado() {
  if (!lecturaOk) return F("Esperando...    ");

  switch (alarmaActual) {
    case SOS:
      if (nivelTemp == CRITICO) return F("TEMP CRITICA SOS");
      if (nivelHum  == CRITICO) return F("HUM. CRITICA SOS");
      return F("SIN LUZ - S.O.S.");
    case ZUMBIDO:
      if (nivelTemp == PRECAUCION) return F("PRECAUCION TEMP ");
      return F("PRECAUCION HUM. ");
    case SIN_ALARMA:
      return F("NORMAL - TODO OK");
  }
  return F("                ");
}

// ===========================================================================
// TAREA 6 - Monitor serie
// ===========================================================================
void tareaSerie(unsigned long ahora) {
  if (ahora - tSerie < PERIODO_SERIE) return;
  tSerie = ahora;
  if (!lecturaOk) return;

  Serial.print(F("T: "));   Serial.print(temperatura, 1);
  Serial.print(F(" C | H: ")); Serial.print(humedad, 0);
  Serial.print(F(" % | Luz: ")); Serial.print(luz);
  Serial.print(hayLuz ? F(" (claro)") : F(" (OSCURO)"));
  Serial.print(F(" | Estado: "));
  Serial.println(textoEstado());
}

// ---------------------------------------------------------------------------
// Escribe un color en el LED RGB. Si es de anodo comun hay que invertir los
// valores, porque en esa variante el color se enciende con MENOS voltaje.
// ---------------------------------------------------------------------------
void ponerColor(int r, int g, int b) {
  if (!CATODO_COMUN) { r = 255 - r; g = 255 - g; b = 255 - b; }
  analogWrite(PIN_ROJO,  r);
  analogWrite(PIN_VERDE, g);
  analogWrite(PIN_AZUL,  b);
}
