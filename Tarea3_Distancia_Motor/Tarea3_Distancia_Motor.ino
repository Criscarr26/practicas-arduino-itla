/*
  ============================================================================
  TAREA 3 - Detector de distancia que gobierna un motor, un LED y un buzzer
  ============================================================================
  Asignatura : Inteligencia Artificial e Internet de las Cosas (2026-C-2)
  Profesor   : Luis Bessewell Feliz
  Estudiante : Cristian Carrera - Matricula 2024-1932
  Institucion: Instituto Tecnologico de Las Americas (ITLA)
  Placa      : Elegoo UNO R3 (compatible Arduino UNO)

  Declaracion de uso de IA: se uso asistencia de inteligencia artificial (Claude)
  para depurar, documentar y estructurar este codigo. El montaje fisico, la
  calibracion, las pruebas sobre la placa y la explicacion del video son propios.
  El profesor autorizo el uso de IA siempre que se declare y se sepa explicar.
  Valor      : 8 puntos

  ============================================================================
  INVESTIGACION PREVIA 1: ESTRUCTURAS DE CONTROL EN ARDUINO
  ============================================================================
  Arduino es C++, asi que dispone de las cuatro estructuras clasicas. Las
  cuatro se usan en este programa, y cada una donde corresponde:

  if / else if / else -> DECIDIR entre caminos segun una condicion.
      Se usa aqui para clasificar la distancia en zonas. Es la estructura
      correcta cuando las condiciones son RANGOS y no valores exactos.

  switch / case ------> ELEGIR entre valores concretos y discretos.
      Se usa aqui para actuar segun la zona ya calculada. Un switch sobre un
      enum se lee mucho mejor que una escalera de seis if encadenados, y el
      compilador avisa si se olvida un caso.
      Limitacion importante: switch NO admite rangos ni condiciones, solo
      valores constantes. Por eso primero se decide con if y luego se actua
      con switch.

  for ----------------> REPETIR un numero de veces CONOCIDO de antemano.
      Se usa aqui para tomar varias muestras del sensor y quedarse con la
      mediana.

  while --------------> REPETIR MIENTRAS se cumpla una condicion, sin saber
      cuantas vueltas seran. Se usa aqui en el ordenamiento de las muestras
      y, sobre todo, hay que saber que el propio loop() de Arduino es en el
      fondo un "while (true)".

  ============================================================================
  INVESTIGACION PREVIA 2: ALCANCE Y FUNCIONAMIENTO DEL HC-SR04
  ============================================================================
  El sensor del kit es el HC-SR04. Segun su hoja de datos:

      Alcance ....... de 2 cm a 400 cm  (4 metros)  <- LA DISTANCIA MAXIMA
      Precision ..... aproximadamente 3 mm
      Angulo ........ cono de unos 15 grados
      Alimentacion .. 5 V, unos 15 mA

  COMO FUNCIONA (eco por ultrasonido, el mismo principio del murcielago):

    1. Se envia un pulso de 10 microsegundos por el pin TRIG.
    2. El sensor emite 8 rafagas de ultrasonido a 40 kHz por su emisor.
    3. El sonido viaja, rebota en el obstaculo y vuelve al receptor.
    4. El pin ECHO se pone en alto exactamente el tiempo que tardo el viaje
       de ida y vuelta.
    5. Se mide ese tiempo con pulseIn() y se convierte a distancia.

  LA FORMULA Y DE DONDE SALE:

      El sonido viaja a unos 343 m/s en aire a 20 grados, que son
      0.0343 cm por microsegundo, o su inversa: 29.1 us por centimetro.

      distancia_cm = (tiempo_us / 29.1) / 2

      Se divide entre 2 porque el tiempo medido es de IDA Y VUELTA, y a
      nosotros solo nos interesa la ida.

  LIMITACIONES QUE HAY QUE CONOCER:
    - Superficies blandas (tela, espuma) absorben el ultrasonido y no
      devuelven eco: el sensor "no ve" una cortina.
    - Superficies en angulo desvian el eco hacia otro lado.
    - La velocidad del sonido depende de la temperatura, asi que en un dia
      muy caluroso la medida se desvia un poco.
    - Si no vuelve ningun eco, pulseIn() se queda esperando hasta agotar su
      tiempo limite y devuelve 0. Hay que tratar ese caso.

  ============================================================================
  INVESTIGACION PREVIA 3: SENTENCIA PARA ENCENDER Y APAGAR UN MOTOR
  ============================================================================
  Depende del tipo de motor. Los tres casos habituales:

  A) SERVOMOTOR (SG90, el del kit) - es el que usa este programa
       #include <Servo.h>
       Servo miServo;
       miServo.attach(pin);        // enciende: el servo pasa a responder
       miServo.write(angulo);      // lo posiciona entre 0 y 180 grados
       miServo.detach();           // APAGA: deja de enviarle pulsos

     El servo no se controla con voltaje sino con el ancho de un pulso que
     se repite cada 20 ms: 1 ms equivale a 0 grados y 2 ms a 180 grados.
     La libreria Servo se encarga de generar esos pulsos por debajo.
     Aqui el "rotor encendido" se representa barriendo el servo de lado a
     lado de forma continua, y "apagado" con detach() mas write() al centro.

  B) MOTOR DC con driver L298N o L293D
       digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // gira
       analogWrite(ENA, velocidad);                      // velocidad 0-255
       digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);  // se detiene

  C) MOTOR PASO A PASO (28BYJ-48 con ULN2003)
       #include <Stepper.h>
       Stepper motor(2048, 8, 10, 9, 11);
       motor.setSpeed(10);
       motor.step(2048);      // una vuelta completa (OJO: esto BLOQUEA)

  NUNCA se conecta un motor DC directamente a un pin de Arduino. Un pin
  entrega 40 mA como maximo absoluto y un motor pide cientos. Hace falta
  siempre un driver o al menos un transistor.

  ============================================================================
  INVESTIGACION PREVIA 4: ¿SOPORTA ARDUINO HILOS Y TAREAS?
  ============================================================================
  La respuesta corta es NO, y la larga es mas interesante.

  El ATmega328P del UNO tiene UN solo nucleo, no tiene sistema operativo y no
  tiene unidad de gestion de memoria. No existen hilos reales: en cada
  instante se ejecuta una sola instruccion.

  Pero SI se puede tener CONCURRENCIA, que no es lo mismo que paralelismo.
  Concurrencia es organizar el programa para que varias tareas progresen
  intercaladas; paralelismo es ejecutarlas literalmente a la vez, y eso
  requiere varios nucleos.

  Las tres vias reales en Arduino:

  1. MULTITAREA COOPERATIVA CON millis()  <- la que usa este programa
     Ninguna funcion bloquea. Cada tarea mira el reloj, se pregunta "¿ya me
     toca?", hace su trabajo en microsegundos y devuelve el control. El
     loop() se convierte en un planificador.

  2. INTERRUPCIONES (attachInterrupt, temporizadores)
     Es lo mas parecido a la expulsion de un sistema operativo: un evento
     externo detiene el programa principal y ejecuta otra rutina.

  3. UN RTOS DE VERDAD
     Existe un port de FreeRTOS para AVR, y placas como el ESP32 traen
     FreeRTOS integrado con DOS nucleos, donde si hay paralelismo real.

  POR QUE ESTA PRACTICA OBLIGA A ELLO:
  El programa tiene que hacer cuatro cosas con ritmos distintos AL MISMO
  TIEMPO: medir la distancia, mover el servo, parpadear el LED y pitar. Si
  se programara con delay(), mientras el LED espera su parpadeo el sensor
  estaria ciego y el servo congelado. Por eso aqui NO HAY UN SOLO delay()
  en todo el programa.

  ============================================================================
  LOS CUATRO CRITERIOS DEL ENUNCIADO, Y COMO SE RESUELVE SU SOLAPAMIENTO
  ============================================================================
  Con un maximo de 400 cm, los porcentajes del enunciado dan:

      95% = 380 cm      50% = 200 cm      26% = 104 cm
       5% =  20 cm      25% = 100 cm       4% =  16 cm

  Los rangos del enunciado se SOLAPAN y hay que decidir una interpretacion:

    - El criterio 3 (25%-50%) y el criterio 4 (4%-26%) se pisan entre 100 y
      104 cm.
    - El criterio 4 baja hasta el 4% (16 cm), por debajo del 5% (20 cm) que
      el criterio 1 fija como limite inferior de operacion.

  INTERPRETACION APLICADA, y su justificacion:
  se evalua de MAS URGENTE (mas cerca) a MENOS urgente. En un sistema de
  proximidad, cuanto mas cerca esta el obstaculo mas critica es la alarma,
  asi que la zona mas cercana manda sobre la mas lejana. Queda:

      d < 16 cm ............ TODO APAGADO (fuera de rango util, criterio 2)
      16 <= d <= 104 cm .... motor ON + LED parpadeo RAPIDO + sirena  (crit. 4)
      104 < d <= 200 cm .... motor ON + LED parpadeo LENTO  + pitido  (crit. 3)
      200 < d <= 380 cm .... motor ON + LED FIJO encendido            (crit. 1)
      d > 380 cm ........... TODO APAGADO (fuera de rango, criterio 2)

  ============================================================================
  MONTAJE
  ============================================================================
     HC-SR04:  VCC -> 5V     TRIG -> pin 9
               GND -> GND    ECHO -> pin 10
     Servo SG90: rojo -> 5V,  marron/negro -> GND,  naranja/amarillo -> pin 6
     Zumbador PASIVO: (+) -> pin 8,  (-) -> GND
     LED: el integrado del pin 13 (opcionalmente uno externo con 220 ohm)

  NOTA SOBRE TEMPORIZADORES: en el UNO la libreria Servo usa el Timer1 y la
  funcion tone() usa el Timer2. Son distintos, asi que servo y zumbador
  conviven sin problema. Si se usara la libreria Tone en el pin equivocado
  si habria conflicto.

  AVISO DE ALIMENTACION: el SG90 puede tirar picos de 500 mA al arrancar. Con
  un solo servo y USB suele bastar, pero si la placa se reinicia sola hay que
  alimentar el servo con una fuente externa de 5 V compartiendo el GND.
  ============================================================================
*/

#include <Servo.h>

// ---------------------------------------------------------------------------
// Mapa de pines
// ---------------------------------------------------------------------------
const int PIN_TRIG     = 9;
const int PIN_ECHO     = 10;
const int PIN_SERVO    = 6;
const int PIN_ZUMBADOR = 8;
const int PIN_LED      = 13;

// ---------------------------------------------------------------------------
// Parametros del sensor y umbrales derivados del enunciado
// ---------------------------------------------------------------------------
const float DISTANCIA_MAXIMA = 400.0;   // cm, hoja de datos del HC-SR04

const float UMBRAL_95 = DISTANCIA_MAXIMA * 0.95;   // 380 cm
const float UMBRAL_50 = DISTANCIA_MAXIMA * 0.50;   // 200 cm
const float UMBRAL_26 = DISTANCIA_MAXIMA * 0.26;   // 104 cm
const float UMBRAL_04 = DISTANCIA_MAXIMA * 0.04;   //  16 cm

// Tiempo limite de pulseIn. Ida y vuelta de 400 cm son 800 cm de recorrido:
// 800 * 29.1 = 23280 us. Se redondea a 25000 para dejar margen.
const unsigned long TIEMPO_LIMITE_ECO = 25000UL;

// ---------------------------------------------------------------------------
// Ritmos de cada tarea (en ms). Ningun delay(): todo se compara con millis().
// ---------------------------------------------------------------------------
const unsigned long PERIODO_MEDICION   = 60;    // el sensor necesita >= 50 ms
const unsigned long PERIODO_SERVO      = 15;    // un grado cada 15 ms
const unsigned long PARPADEO_LENTO     = 400;   // criterio 3
const unsigned long PARPADEO_RAPIDO    = 100;   // criterio 4
const unsigned long PERIODO_REPORTE    = 500;   // monitor serie

// Frecuencias del zumbador
const int TONO_LENTO   = 1000;   // pitido simple, zona media
const int TONO_SIRENA_A = 1800;  // sirena de dos tonos, zona cercana
const int TONO_SIRENA_B = 1200;

// ---------------------------------------------------------------------------
// Las cuatro zonas del enunciado, como tipo enumerado.
// Darles nombre en vez de usar 0, 1, 2, 3 hace el switch legible y evita
// errores tontos al comparar.
// ---------------------------------------------------------------------------
enum Zona {
  FUERA_DE_RANGO,   // criterio 2: todo apagado
  ZONA_LEJANA,      // criterio 1: motor ON, LED fijo
  ZONA_MEDIA,       // criterio 3: parpadeo lento + pitido
  ZONA_CERCANA      // criterio 4: parpadeo rapido + sirena
};

// ---------------------------------------------------------------------------
// Estado del programa
// ---------------------------------------------------------------------------
Servo rotor;

float  distancia   = 0;
Zona   zonaActual  = FUERA_DE_RANGO;
Zona   zonaAnterior = FUERA_DE_RANGO;

bool   motorEncendido = false;
int    anguloServo    = 0;
int    pasoServo      = 1;        // +1 o -1, para el barrido de ida y vuelta

bool   ledEncendido   = false;
bool   sirenaEnTonoA  = false;

// Marcas de tiempo de cada tarea concurrente
unsigned long tMedicion = 0;
unsigned long tServo    = 0;
unsigned long tParpadeo = 0;
unsigned long tReporte  = 0;

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  pinMode(PIN_TRIG,     OUTPUT);
  pinMode(PIN_ECHO,     INPUT);
  pinMode(PIN_LED,      OUTPUT);
  pinMode(PIN_ZUMBADOR, OUTPUT);

  digitalWrite(PIN_TRIG, LOW);

  Serial.println(F("== Tarea 3: detector de distancia con motor =="));
  Serial.println(F("Cristian Carrera - 2024-1932 - ITLA"));
  Serial.println();
  Serial.println(F("Umbrales calculados sobre 400 cm de alcance maximo:"));
  Serial.print(F("  95% = ")); Serial.print(UMBRAL_95); Serial.println(F(" cm"));
  Serial.print(F("  50% = ")); Serial.print(UMBRAL_50); Serial.println(F(" cm"));
  Serial.print(F("  26% = ")); Serial.print(UMBRAL_26); Serial.println(F(" cm"));
  Serial.print(F("   4% = ")); Serial.print(UMBRAL_04); Serial.println(F(" cm"));
  Serial.println();

  // El enunciado pide que el motor arranque encendido en cuanto se transfiere
  // el codigo, siempre que la distancia este en rango. Se enciende aqui y la
  // primera medicion decidira si sigue o no.
  encenderMotor();
}

// ---------------------------------------------------------------------------
// loop() como PLANIFICADOR cooperativo. Ofrece la CPU a cada tarea, una tras
// otra, miles de veces por segundo. Regla de oro: aqui no puede haber ni un
// solo delay().
// ---------------------------------------------------------------------------
void loop() {
  unsigned long ahora = millis();

  tareaMedirDistancia(ahora);
  tareaMoverMotor(ahora);
  tareaLuzYSonido(ahora);
  tareaReportar(ahora);
}

// ===========================================================================
// TAREA 1 - Medir la distancia y clasificarla en una zona
// ===========================================================================
void tareaMedirDistancia(unsigned long ahora) {
  if (ahora - tMedicion < PERIODO_MEDICION) return;   // todavia no toca
  tMedicion = ahora;

  distancia = medirDistanciaFiltrada();
  zonaActual = clasificarZona(distancia);

  // El motor solo se apaga si salimos del rango util (criterio 2).
  if (zonaActual == FUERA_DE_RANGO) {
    if (motorEncendido) apagarMotor();
  } else {
    if (!motorEncendido) encenderMotor();
  }
}

// ---------------------------------------------------------------------------
// Una sola lectura del sensor, en centimetros. Devuelve -1 si no hubo eco.
// ---------------------------------------------------------------------------
float medirDistanciaCruda() {
  // Pulso de disparo: 10 us en alto, como pide la hoja de datos.
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // pulseIn mide cuanto tiempo permanece ECHO en alto.
  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIEMPO_LIMITE_ECO);

  if (duracion == 0) return -1.0;      // no volvio eco: nada delante

  // La formula investigada: se divide entre 29.1 us/cm y entre 2 por el
  // viaje de ida y vuelta.
  return (duracion / 29.1) / 2.0;
}

// ---------------------------------------------------------------------------
// Toma varias muestras y devuelve la MEDIANA.
// Se usa mediana y no promedio a proposito: el HC-SR04 suelta lecturas
// disparatadas de vez en cuando, y un solo valor absurdo arruinaria el
// promedio, mientras que a la mediana no le afecta.
//
// Aqui aparecen el bucle for (numero de vueltas conocido) y el while
// (numero de vueltas desconocido, depende de como esten los datos).
// ---------------------------------------------------------------------------
float medirDistanciaFiltrada() {
  const int MUESTRAS = 5;
  float m[MUESTRAS];
  int validas = 0;

  // for: se sabe exactamente cuantas muestras se van a tomar.
  for (int i = 0; i < MUESTRAS; i++) {
    float d = medirDistanciaCruda();
    if (d > 0) {
      m[validas] = d;
      validas++;
    }
    delayMicroseconds(3000);   // el sensor necesita respirar entre disparos
  }

  if (validas == 0) return -1.0;

  // Ordenamiento por insercion. El while da vueltas MIENTRAS haga falta
  // mover el elemento hacia atras: no se sabe de antemano cuantas seran.
  for (int i = 1; i < validas; i++) {
    float clave = m[i];
    int j = i - 1;
    while (j >= 0 && m[j] > clave) {
      m[j + 1] = m[j];
      j--;
    }
    m[j + 1] = clave;
  }

  return m[validas / 2];   // el del medio
}

// ---------------------------------------------------------------------------
// Clasifica la distancia en una zona.
// Aqui va el if/else if porque las condiciones son RANGOS: switch no sabe
// comparar rangos, solo valores exactos.
// El orden es de MAS CERCA a MAS LEJOS, que es la interpretacion justificada
// arriba para resolver el solapamiento de los criterios.
// ---------------------------------------------------------------------------
Zona clasificarZona(float d) {
  if (d < 0) {
    return FUERA_DE_RANGO;              // sin eco: nada delante
  } else if (d < UMBRAL_04) {
    return FUERA_DE_RANGO;              // demasiado cerca (< 4%)
  } else if (d <= UMBRAL_26) {
    return ZONA_CERCANA;                // criterio 4: 4% a 26%
  } else if (d <= UMBRAL_50) {
    return ZONA_MEDIA;                  // criterio 3: 26% a 50%
  } else if (d <= UMBRAL_95) {
    return ZONA_LEJANA;                 // criterio 1: 50% a 95%
  } else {
    return FUERA_DE_RANGO;              // mas alla del 95%
  }
}

// ===========================================================================
// TAREA 2 - Mover el motor sin bloquear
// El servo barre de 0 a 180 y vuelve, avanzando un grado cada 15 ms. Asi el
// movimiento se ve continuo pero la funcion regresa en microsegundos.
// ===========================================================================
void tareaMoverMotor(unsigned long ahora) {
  if (!motorEncendido) return;
  if (ahora - tServo < PERIODO_SERVO) return;
  tServo = ahora;

  anguloServo += pasoServo;

  // Al llegar a un extremo se invierte el sentido del barrido.
  if (anguloServo >= 180) { anguloServo = 180; pasoServo = -1; }
  if (anguloServo <= 0)   { anguloServo = 0;   pasoServo =  1; }

  rotor.write(anguloServo);
}

void encenderMotor() {
  rotor.attach(PIN_SERVO);       // sentencia investigada: enciende el servo
  motorEncendido = true;
  Serial.println(F(">> MOTOR ENCENDIDO"));
}

void apagarMotor() {
  rotor.write(90);               // se deja centrado
  rotor.detach();                // sentencia investigada: lo apaga
  motorEncendido = false;
  Serial.println(F(">> MOTOR APAGADO"));
}

// ===========================================================================
// TAREA 3 - Luz y sonido segun la zona
// Aqui va el switch: la zona ya esta calculada y es un valor discreto, que es
// justo para lo que sirve switch. Se lee mucho mejor que seis if anidados.
// ===========================================================================
void tareaLuzYSonido(unsigned long ahora) {

  switch (zonaActual) {

    // --- Criterio 2: fuera del 5%-95%, todo apagado --------------------
    case FUERA_DE_RANGO:
      digitalWrite(PIN_LED, LOW);
      noTone(PIN_ZUMBADOR);
      ledEncendido = false;
      break;

    // --- Criterio 1: en rango, LED fijo encendido y sin sonido ---------
    case ZONA_LEJANA:
      digitalWrite(PIN_LED, HIGH);
      noTone(PIN_ZUMBADOR);
      ledEncendido = true;
      break;

    // --- Criterio 3: parpadeo lento + pitido al mismo ritmo ------------
    case ZONA_MEDIA:
      if (ahora - tParpadeo >= PARPADEO_LENTO) {
        tParpadeo = ahora;
        ledEncendido = !ledEncendido;
        digitalWrite(PIN_LED, ledEncendido);

        // El sonido sigue EXACTAMENTE el ritmo de la luz, como pide el
        // enunciado: suena cuando el LED se enciende y calla cuando se apaga.
        if (ledEncendido) tone(PIN_ZUMBADOR, TONO_LENTO);
        else              noTone(PIN_ZUMBADOR);
      }
      break;

    // --- Criterio 4: parpadeo rapido + sirena de dos tonos -------------
    case ZONA_CERCANA:
      if (ahora - tParpadeo >= PARPADEO_RAPIDO) {
        tParpadeo = ahora;
        ledEncendido = !ledEncendido;
        digitalWrite(PIN_LED, ledEncendido);

        if (ledEncendido) {
          // "Sonido particular": alterna dos frecuencias, lo que produce la
          // sirena tipica de alarma de proximidad y se distingue sin dudar
          // del pitido simple de la zona media.
          sirenaEnTonoA = !sirenaEnTonoA;
          tone(PIN_ZUMBADOR, sirenaEnTonoA ? TONO_SIRENA_A : TONO_SIRENA_B);
        } else {
          noTone(PIN_ZUMBADOR);
        }
      }
      break;
  }
}

// ===========================================================================
// TAREA 4 - Reportar por el monitor serie
// ===========================================================================
void tareaReportar(unsigned long ahora) {
  // Un cambio de zona se avisa al instante, sin esperar al reporte periodico.
  if (zonaActual != zonaAnterior) {
    Serial.print(F("*** CAMBIO DE ZONA -> "));
    Serial.println(nombreZona(zonaActual));
    zonaAnterior = zonaActual;
  }

  if (ahora - tReporte < PERIODO_REPORTE) return;
  tReporte = ahora;

  Serial.print(F("Distancia: "));
  if (distancia < 0) {
    Serial.print(F("sin eco"));
  } else {
    Serial.print(distancia, 1);
    Serial.print(F(" cm ("));
    Serial.print((distancia / DISTANCIA_MAXIMA) * 100.0, 1);
    Serial.print(F(" %)"));
  }
  Serial.print(F("  |  Zona: "));
  Serial.print(nombreZona(zonaActual));
  Serial.print(F("  |  Motor: "));
  Serial.println(motorEncendido ? F("ON") : F("OFF"));
}

// ---------------------------------------------------------------------------
// Segundo switch, esta vez para traducir el enum a texto legible.
// ---------------------------------------------------------------------------
const __FlashStringHelper *nombreZona(Zona z) {
  switch (z) {
    case FUERA_DE_RANGO: return F("FUERA DE RANGO");
    case ZONA_LEJANA:    return F("LEJANA (LED fijo)");
    case ZONA_MEDIA:     return F("MEDIA (parpadeo lento)");
    case ZONA_CERCANA:   return F("CERCANA (parpadeo rapido)");
  }
  return F("?");
}
