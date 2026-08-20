/*
  ============================================================================
  Telemetria por puerto serie  -  firmware de la placa
  ============================================================================
  Autor : Cristian Carrera
  Placa : Elegoo UNO R3 (compatible Arduino UNO)

  Se uso asistencia de IA (Claude) para depurar y documentar. El montaje, la
  calibracion y las pruebas sobre la placa son propios.

  La parte 2 es el programa lector, escrito en Python, que esta en la carpeta
  de al lado: lector_distancia.py

  ============================================================================
  EL OBJETIVO
  ============================================================================
  La idea de partida era esta: "SALIR de
  Arduino... que desde otro lenguaje leas por el puerto lo que el sensa".
  Y en vez de empezar de cero, convertir uno de los proyectos ya hechos.

  Asi que aqui NO se monta nada nuevo. Se reutiliza tal cual el montaje de la
  proyecto del sensor de distancia -con sus dos zumbadores- y lo que se
  agrega es que la placa, ademas de avisar por sonido, PUBLIQUE cada lectura
  por el puerto serie para que otro programa la reciba.

  Ni un solo cable cambia de sitio.

  ============================================================================
  POR QUE ESTO TIENE SENTIDO Y NO ES UN CAPRICHO
  ============================================================================
  Un microcontrolador es buenisimo midiendo el mundo fisico en tiempo real,
  pero es pesimo guardando historicos y analizando: el UNO tiene 2 KB de RAM.
  Una computadora es justo al reves.

  La division natural del trabajo es entonces:

        Arduino  ->  sensa y avisa      (tiempo real, pegado al hardware)
        Python   ->  recibe, guarda, analiza y grafica

  Y ademas es un caso de PROGRAMACION DISTRIBUIDA: dos programas, en dos
  maquinas distintas, que no comparten memoria y se coordinan solo por
  mensajes. Es el tercer escalon del tema:

        Secuencial  -> una instruccion tras otra
        Concurrente -> varias tareas intercaladas en un nucleo   (el loop)
        Paralelo    -> varias a la vez en varios nucleos          (aqui no)
        Distribuido -> en MAQUINAS distintas, por mensajes        (esto)

  ============================================================================
  INVESTIGACION PREVIA 1: COMO SE COMUNICA ARDUINO CON UNA COMPUTADORA
  ============================================================================
  El UNO R3 no tiene WiFi ni Bluetooth. Su via es el PUERTO SERIE, el mismo
  cable USB con el que se le carga el programa.

  Por dentro del cable, el ATmega328P habla UART: manda los bits de uno en
  uno por un solo hilo. Una computadora moderna no tiene puerto serie fisico,
  asi que en la placa hay un segundo chip que traduce UART a USB. En el UNO
  original es un ATmega16U2; en los clones y en muchos Elegoo es un CH340,
  que suele necesitar su driver aparte.

  El sistema operativo ve el resultado como un PUERTO COM, y cualquier
  lenguaje que sepa abrir ese puerto puede leer lo que la placa escriba.

  LOS BAUDIOS. Serial.begin(9600) fija la velocidad en 9600 bits por segundo.
  No hay negociacion automatica: los dos lados TIENEN que usar el mismo
  numero. Si no coinciden, no llega basura parcial: llega basura entera.

  ============================================================================
  INVESTIGACION PREVIA 2: POR QUE CSV Y NO TEXTO BONITO
  ============================================================================
  La placa podria mandar "Distancia: 45.2 cm", que se lee muy bien. El
  problema es que del otro lado hay un programa, no una persona, y tendria
  que ponerse a recortar cadenas para sacar el numero.

  Por eso se manda CSV: los valores separados por comas, uno por linea.

        LECTURA,MILIS,DISTANCIA_CM,ZONA,ESTADO
        1,2431,45.2,CERCA,DETENIDA

  Es el formato mas simple que Python entiende de una, y ademas se abre tal
  cual en Excel. La primera linea es la CABECERA: dice como se llama cada
  columna, para que el lector no dependa del orden.

  Cuando no hay eco se manda -1 en la distancia. Se eligio un valor imposible
  en vez de dejar el campo vacio, porque un campo vacio descoloca las comas.

  ============================================================================
  INVESTIGACION PREVIA 3: EL AUTO-RESET, QUE SORPRENDE A TODO EL MUNDO
  ============================================================================
  Al abrir el puerto desde la computadora, la señal DTR REINICIA la placa.
  O sea que el Arduino arranca de cero justo cuando Python se conecta.

  Eso tiene dos consecuencias:

    1. El lector tiene que esperar un par de segundos antes de fiarse de lo
       que llega, o recibira lineas cortadas por la mitad.
    2. El contador de lecturas siempre empieza en 1 en cada sesion, y millis()
       tambien empieza en 0. Es lo esperado, no un fallo.

  ============================================================================
  INVESTIGACION PREVIA 4: ESTRUCTURAS DE CONTROL
  ============================================================================
    if / else if -> decide por RANGOS. Clasifica la distancia en zonas y decide
                    si toca enviar por el puerto.
    switch       -> elige entre VALORES discretos. Actua sobre la zona ya
                    calculada y la traduce al texto que viaja en el CSV.
    for          -> repite un numero CONOCIDO de veces: las tres lecturas del
                    sensor con las que se calcula la mediana.
    while        -> en el sketch no hay ninguno, a proposito: bloquearia la
                    placa. Donde si aparece, y es su sitio natural, es en los
                    dos lectores del otro lado: el bucle que escucha el puerto
                    es un while en Python y otro en C#. Tiene sentido, porque
                    alli no se sabe cuantas lineas van a llegar, y esperar no
                    cuesta nada: esa maquina no tiene un sensor que atender.

  ============================================================================
  INVESTIGACION PREVIA 5: SOPORTA ARDUINO HILOS O TAREAS?
  ============================================================================
  Hilos reales NO. El ATmega328P tiene un solo nucleo y no lleva sistema
  operativo ni planificador, asi que no hay nada que reparta el tiempo entre
  hilos.

  Concurrencia SI, y es lo que hace este programa. Hay que separar tres cosas:

    Secuencial  -> una instruccion tras otra, bloqueando
    Concurrente -> varias tareas PROGRESAN INTERCALADAS   (si, con millis)
    Paralelo    -> varias tareas se ejecutan A LA VEZ     (no: un solo nucleo)

  Aqui el loop() no hace trabajo: reparte turnos. Cada tarea mira el reloj y,
  si aun no le toca, devuelve el control enseguida. Asi la medicion, el sonido
  y el envio por el puerto avanzan como si fueran independientes.

  Y aqui aparece un cuarto escalon:

    Distribuido -> tareas en MAQUINAS distintas, coordinadas por mensajes

  Que es exactamente lo que son la placa y la computadora: dos programas que no
  comparten memoria y solo se hablan por el puerto serie.


  ============================================================================
  MONTAJE - el mismo de sensor_distancia, sin tocar nada
  ============================================================================
     HC-SR04                 Arduino
       VCC ---------------->   5V
       GND ---------------->   GND
       TRIG --------------->   pin 3
       ECHO --------------->   pin 2

     Zumbador de RITMO       Arduino
       + (larga) ---------->   pin 9
       - (corta) ---------->   GND

     Zumbador FIJO           Arduino
       + (larga) ---------->   pin 8
       - (corta) ---------->   GND

  Y el cable USB, que ademas de alimentar la placa es por donde viajan los
  datos hasta Python.
  ============================================================================
*/

// ---------------------------------------------------------------------------
// Mapa de pines - identico al de sensor_distancia
// ---------------------------------------------------------------------------
const int PIN_ECHO = 2;
const int PIN_TRIG = 3;

const int PIN_ZUMBADOR_RITMO = 9;   // pita mas rapido cuanto mas cerca
const int PIN_ZUMBADOR_FIJO  = 8;   // tono continuo cuando hay que PARAR

const int PIN_LED = 13;             // LED de la placa, con su resistencia

// ---------------------------------------------------------------------------
// Tipo de cada zumbador (ver sensor_distancia).
// En el UNO tone() solo suena en un pin a la vez: los dos no pueden ser
// pasivos.
// ---------------------------------------------------------------------------
const bool RITMO_ES_PASIVO = false;
const bool FIJO_ES_PASIVO  = true;

// Poner en false para capturar datos en silencio, si el pitido molesta al
// grabar el video. La medicion y el envio siguen igual.
const bool SONIDO_ACTIVO = true;

// ---------------------------------------------------------------------------
// Umbrales, derivados del alcance del sensor
// ---------------------------------------------------------------------------
const float DISTANCIA_MAXIMA = 400.0;                  // cm, hoja de datos
const float UMBRAL_LEJOS  = DISTANCIA_MAXIMA * 0.50;   // 200 cm
const float UMBRAL_MEDIO  = DISTANCIA_MAXIMA * 0.26;   // 104 cm
const float UMBRAL_MINIMO = 2.0;                       // el sensor no baja de 2

const float DISTANCIA_CONTINUO =  10.0;
const float DISTANCIA_AVISO    = 100.0;
const unsigned long INTERVALO_MIN =  60;
const unsigned long INTERVALO_MAX = 800;

const unsigned long TIEMPO_LIMITE_ECO = 25000UL;

// ---------------------------------------------------------------------------
// Ritmos. Ningun delay() dentro del loop.
// ---------------------------------------------------------------------------
const unsigned long PERIODO_MEDICION = 60;    // el HC-SR04 necesita >= 50 ms
const unsigned long PERIODO_ENVIO    = 200;   // 5 lineas por segundo

const int TONO_PITIDO = 1000;
const int TONO_PARADA = 1800;

// ---------------------------------------------------------------------------
enum Zona { FUERA_DE_RANGO, ZONA_LEJANA, ZONA_MEDIA, ZONA_CERCANA };

// ---------------------------------------------------------------------------
float distancia  = -1.0;
Zona  zonaActual = FUERA_DE_RANGO;
bool  enMarcha   = false;
bool  sonando    = false;

unsigned long intervaloActual = INTERVALO_MAX;
unsigned long numeroLectura   = 0;

unsigned long tMedicion = 0;
unsigned long tPitido   = 0;
unsigned long tEnvio    = 0;

// ===========================================================================
void setup() {
  Serial.begin(9600);

  pinMode(PIN_TRIG,           OUTPUT);
  pinMode(PIN_ECHO,           INPUT);
  pinMode(PIN_LED,            OUTPUT);
  pinMode(PIN_ZUMBADOR_RITMO, OUTPUT);
  pinMode(PIN_ZUMBADOR_FIJO,  OUTPUT);

  digitalWrite(PIN_TRIG, LOW);

  // La CABECERA va primero y una sola vez. Python la usa para saber como se
  // llama cada columna, en vez de fiarse del orden.
  Serial.println(F("LECTURA,MILIS,DISTANCIA_CM,ZONA,ESTADO"));
}

// ---------------------------------------------------------------------------
void loop() {
  unsigned long ahora = millis();

  tareaMedirDistancia(ahora);
  tareaSonido(ahora);
  tareaEnviar(ahora);
}

// ===========================================================================
// TAREA 1 - Medir y clasificar
// ===========================================================================
void tareaMedirDistancia(unsigned long ahora) {
  if (ahora - tMedicion < PERIODO_MEDICION) return;
  tMedicion = ahora;

  distancia  = medirDistanciaFiltrada();
  zonaActual = clasificarZona(distancia);
  intervaloActual = intervaloPorDistancia(distancia);

  enMarcha = (zonaActual == ZONA_LEJANA || zonaActual == ZONA_MEDIA);
}

// ---------------------------------------------------------------------------
float medirDistanciaCruda() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIEMPO_LIMITE_ECO);
  if (duracion == 0) return -1.0;

  return (duracion / 29.1) / 2.0;      // el /2 es por la ida y la vuelta
}

// ---------------------------------------------------------------------------
// Mediana de tres, no promedio: una lectura disparatada del sensor arruinaria
// el promedio, pero al ordenar tres valores el raro se queda en un borde.
// ---------------------------------------------------------------------------
float medirDistanciaFiltrada() {
  float m[3];
  for (int i = 0; i < 3; i++) {
    m[i] = medirDistanciaCruda();
    delayMicroseconds(500);
  }
  if (m[0] > m[1]) intercambiar(m[0], m[1]);
  if (m[1] > m[2]) intercambiar(m[1], m[2]);
  if (m[0] > m[1]) intercambiar(m[0], m[1]);
  return m[1];
}

void intercambiar(float &a, float &b) { float t = a; a = b; b = t; }

// ---------------------------------------------------------------------------
Zona clasificarZona(float d) {
  if (d < UMBRAL_MINIMO)  return FUERA_DE_RANGO;
  if (d > UMBRAL_LEJOS)   return ZONA_LEJANA;
  if (d > UMBRAL_MEDIO)   return ZONA_MEDIA;
  return ZONA_CERCANA;
}

// ---------------------------------------------------------------------------
unsigned long intervaloPorDistancia(float d) {
  if (d < UMBRAL_MINIMO)        return INTERVALO_MAX;
  if (d <= DISTANCIA_CONTINUO)  return 0;
  if (d >= DISTANCIA_AVISO)     return INTERVALO_MAX;
  float t = (d - DISTANCIA_CONTINUO) / (DISTANCIA_AVISO - DISTANCIA_CONTINUO);
  return INTERVALO_MIN + (unsigned long)(t * (INTERVALO_MAX - INTERVALO_MIN));
}

// ===========================================================================
// TAREA 2 - Los dos zumbadores y el LED (igual que en sensor_distancia)
// ===========================================================================
void tareaSonido(unsigned long ahora) {
  if (!SONIDO_ACTIVO) return;

  // Zumbador FIJO: la orden de parar. No pulsa nunca.
  if (enMarcha || zonaActual == FUERA_DE_RANGO) callarFijo(); else sonarFijo();

  // Zumbador de RITMO: a que distancia estamos.
  if (zonaActual == FUERA_DE_RANGO) {
    digitalWrite(PIN_LED, LOW);
    callarRitmo();
    sonando = false;
    return;
  }

  if (intervaloActual == 0) {          // tan cerca que ya no se interrumpe
    digitalWrite(PIN_LED, HIGH);
    sonarRitmo();
    sonando = true;
    return;
  }

  if (ahora - tPitido >= intervaloActual) {
    tPitido = ahora;
    sonando = !sonando;
    digitalWrite(PIN_LED, sonando);
    if (sonando) sonarRitmo(); else callarRitmo();
  }
}

void sonarEn(int pin, bool esPasivo, int frecuencia) {
  if (esPasivo) tone(pin, frecuencia); else digitalWrite(pin, HIGH);
}
void callarEn(int pin, bool esPasivo) {
  if (esPasivo) noTone(pin); else digitalWrite(pin, LOW);
}

void sonarRitmo()  { sonarEn(PIN_ZUMBADOR_RITMO, RITMO_ES_PASIVO, TONO_PITIDO); }
void callarRitmo() { callarEn(PIN_ZUMBADOR_RITMO, RITMO_ES_PASIVO); }
void sonarFijo()   { sonarEn(PIN_ZUMBADOR_FIJO, FIJO_ES_PASIVO, TONO_PARADA); }
void callarFijo()  { callarEn(PIN_ZUMBADOR_FIJO, FIJO_ES_PASIVO); }

// ===========================================================================
// TAREA 3 - Publicar la lectura por el puerto serie
//
// Esta es la tarea que da sentido a la practica: es la que hace que los datos
// salgan de la placa y esten disponibles para otro lenguaje.
// ===========================================================================
void tareaEnviar(unsigned long ahora) {
  if (ahora - tEnvio < PERIODO_ENVIO) return;
  tEnvio = ahora;

  numeroLectura++;

  Serial.print(numeroLectura);   Serial.print(',');
  Serial.print(ahora);           Serial.print(',');
  Serial.print(distancia, 1);    Serial.print(',');   // -1.0 si no hubo eco
  Serial.print(nombreZona(zonaActual));  Serial.print(',');
  Serial.println(enMarcha ? F("EN_MARCHA") : F("DETENIDA"));
}

// Sin espacios ni acentos: son valores de un CSV, no texto para leer.
const __FlashStringHelper *nombreZona(Zona z) {
  switch (z) {
    case ZONA_LEJANA:  return F("LEJOS");
    case ZONA_MEDIA:   return F("MEDIA");
    case ZONA_CERCANA: return F("CERCA");
    default:           return F("SIN_ECO");
  }
}
