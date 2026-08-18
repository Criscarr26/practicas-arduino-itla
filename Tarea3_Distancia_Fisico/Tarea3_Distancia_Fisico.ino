/*
  ============================================================================
  TAREA 3 (VERSION FISICA) - Detector de distancia que manda continuar o parar
  ============================================================================
  Asignatura : Inteligencia Artificial e Internet de las Cosas (2026-C-2)
  Profesor   : Luis Bessewell Feliz
  Estudiante : Cristian Carrera - Matricula 2024-1932
  Institucion: Instituto Tecnologico de Las Americas (ITLA)
  Placa      : Elegoo UNO R3 (compatible Arduino UNO)

  ============================================================================
  LOS PINES SON LOS DEL MONTAJE, NO AL REVES
  ============================================================================
  El circuito esta armado siguiendo el tutorial del sensor de aparcamiento,
  que usa esta conexion:

        ECHO -> pin 2        TRIG -> pin 3        zumbador -> pin 9

  El programa usa exactamente esos pines. Cambiar una linea de codigo cuesta
  nada; recablear cuesta tiempo y se sueltan los cables de al lado. Si algun
  dia se mueve el montaje, se ajusta abajo en el mapa de pines y ya.

  ============================================================================
  POR QUE ESTA VERSION NO LLEVA MOTOR
  ============================================================================
  No consegui el motor a tiempo y lo negocie con el profesor antes de la
  entrega. Lo que se evalua es el objetivo, no la pieza: medir la distancia y
  decidir cuando algo debe CONTINUAR y cuando debe DETENERSE. El profesor
  autorizo sustituir el motor por una señal sonora.

  Este sketch NO usa la libreria Servo. Si el IDE pide instalar algo de
  motores, es que esta abierto el sketch de la carpeta Tarea3_Distancia_Motor,
  que sirve solo para el simulador.

  ============================================================================
  EL PITIDO VA MAS RAPIDO CUANTO MAS CERCA
  ============================================================================
  Es el comportamiento de un sensor de aparcamiento de verdad, y es la parte
  mas util del aparato: el oido nota el CAMBIO DE RITMO mucho antes que un
  numero en pantalla, y sin tener que mirar.

  El ritmo no salta por escalones, va cambiando de forma continua:

        10 cm o menos ...  tono continuo, sin cortes   (maxima urgencia)
         15 cm .........  pitidos cada 101 ms          (muy rapido)
         20 cm .........  pitidos cada 142 ms
         30 cm .........  pitidos cada 224 ms
         50 cm .........  pitidos cada 388 ms
         75 cm .........  pitidos cada 594 ms
        100 cm o mas ...  pitidos cada 800 ms          (muy lento)
        sin eco .......   callado

  El intervalo se saca con una interpolacion lineal entre esos dos extremos,
  que es lo mismo que hace la funcion map() del tutorial, pero escrito a mano
  con numeros decimales para no perder precision en distancias cortas.

  POR QUE EL AVISO LLEGA SOLO HASTA 1 METRO Y NO HASTA 4
  -----------------------------------------------------
  El sensor mide hasta 400 cm, pero repartir el ritmo por todo ese rango no
  sirve de nada en la practica: al probarlo con la mano sobre la mesa, entre
  10 y 50 cm, TODO caia en el tramo mas rapido de la escala y no se notaba
  ninguna diferencia. El oido no distingue 56 ms de 112 ms.

  Concentrando el cambio en el primer metro, cada centimetro cuenta donde
  realmente se usa. La medicion sigue llegando a 4 m y se sigue viendo en el
  monitor serie; lo que se acota es el rango del AVISO, no el del sensor.
  Es una decision de diseño, no una limitacion del hardware.

  El LED del pin 13 parpadea AL MISMO RITMO que ese zumbador. Asi el video
  funciona aunque se vea sin sonido: se aprecia como el parpadeo se acelera
  segun se acerca la mano, y cuando se queda fijo es que hay que parar.

  ============================================================================
  DOS ZUMBADORES, DOS MENSAJES DISTINTOS
  ============================================================================
  Un solo zumbador tiene que contarlo todo con un unico ritmo, y eso se presta
  a confusion: un pitido muy rapido y un tono continuo se parecen demasiado
  cuando uno esta pendiente de otra cosa.

  Con dos, cada uno dice una cosa sola:

     ZUMBADOR DE RITMO (pin 9)   ->  A QUE DISTANCIA estas
        Pulsa siempre que haya algo delante, y va mas rapido cuanto mas cerca.
        Es informacion continua: te va guiando.

     ZUMBADOR FIJO (pin 8)       ->  PARA
        No pulsa nunca. O suena entero o esta callado. Se enciende justo
        cuando la maquina pasa a DETENIDA, y se apaga cuando vuelve a marchar.
        Es informacion binaria: una orden.

  Asi el operador distingue "me estoy acercando" de "detente" sin mirar nada.
  Es como funciona una marcha atras de verdad: el pitido te guia, y otra señal
  distinta te avisa del limite.

  UNA LIMITACION DEL HARDWARE QUE CONVIENE SABER
  ----------------------------------------------
  En el UNO, la funcion tone() solo puede sonar en UN pin a la vez: usa un
  unico temporizador interno, y al llamarla sobre un segundo pin el primero se
  calla. O sea que LOS DOS ZUMBADORES NO PUEDEN SER PASIVOS.

  Con dos zumbadores ACTIVOS no hay problema ninguno: cada uno se enciende con
  digitalWrite() y suenan a la vez sin estorbarse. Si uno es pasivo y el otro
  activo, tambien funciona. Los dos pasivos, no.

  ============================================================================
  INVESTIGACION PREVIA: COMO MIDE DISTANCIA EL HC-SR04
  ============================================================================
  El sensor tiene dos capsulas: una emite y otra escucha.

    1. Se le manda un pulso de 10 microsegundos por TRIG.
    2. El sensor dispara 8 rafagas de ultrasonido a 40 kHz.
    3. ECHO se pone en alto y se mantiene asi TODO el tiempo que tarda el
       sonido en ir hasta el obstaculo y volver.
    4. Se mide esa duracion con pulseIn().

  La formula:

        distancia_cm = (duracion_us / 29.1) / 2

  El 29.1 sale de la velocidad del sonido: 343 m/s son 0,0343 cm por
  microsegundo, y su inverso es 29,1 microsegundos por centimetro. El tutorial
  lo escribe como "duration * 0.034 / 2", que es la misma cuenta: 0,034 es el
  inverso de 29,1. Dos formas de escribir lo mismo.

  La division entre 2 es la parte que mas se olvida: el sonido recorre el
  camino DOS veces, ida y vuelta. Sin ese /2 todas las medidas salen al doble.

  Alcance util del sensor: de 2 a 400 cm.

  ============================================================================
  LAS CUATRO ZONAS
  ============================================================================
  El ritmo del pitido es continuo, pero la DECISION sigue siendo discreta:
  la maquina esta en marcha o no lo esta. Las zonas son las que deciden eso.

    Fuera de rango (sin eco)  ->  los dos callados,            DETENIDA
    Lejos   (mas de 200 cm)   ->  ritmo lento,  fijo callado,   EN MARCHA
    Media   (104 a 200 cm)    ->  ritmo medio,  fijo callado,   EN MARCHA
    Cerca   (menos de 104 cm) ->  ritmo rapido, FIJO SONANDO,   DETENIDA

  ============================================================================
  MONTAJE
  ============================================================================
     HC-SR04          Arduino
       VCC     ----->   5V
       GND     ----->   GND
       TRIG    ----->   pin 3
       ECHO    ----->   pin 2

     Zumbador de RITMO       Arduino
       + (larga) ----------->   pin 9
       - (corta) ----------->   GND

     Zumbador FIJO           Arduino
       + (larga) ----------->   pin 8
       - (corta) ----------->   GND

  No hace falta ninguna resistencia: el sensor no las lleva, los zumbadores se
  conectan directos, y el LED del pin 13 ya viene montado en la placa con la
  suya puesta.

  El de ritmo ya estaba puesto en el pin 9; solo se añade el segundo al 8.

  ============================================================================
  EN QUE SE DIFERENCIA DEL TUTORIAL
  ============================================================================
  El tutorial marca el ritmo con delay(), asi que mientras pita no puede
  medir. Si te acercas de golpe, sigue con el ritmo viejo hasta que termina el
  pitido en curso. Aqui no hay ningun delay() dentro del loop: se mide cada
  60 ms pase lo que pase, y el ritmo se recalcula en cada medicion, asi que
  responde al instante.

  El UNO tiene un solo nucleo y ningun sistema operativo, o sea que no hay
  hilos de verdad. Lo que se hace es multitarea COOPERATIVA: loop() ofrece el
  turno a cada tarea miles de veces por segundo y cada una decide si le toca
  mirando millis(). Esa es la diferencia entre concurrencia y ejecucion
  secuencial, que es justamente el tema de la asignatura.

  El tutorial tampoco filtra las lecturas; aqui se toma la mediana de tres,
  para que un rebote raro no acelere el pitido sin motivo.
  ============================================================================
*/

// ---------------------------------------------------------------------------
// Mapa de pines - los del montaje real
// ---------------------------------------------------------------------------
const int PIN_ECHO = 2;
const int PIN_TRIG = 3;

// Dos zumbadores, cada uno con un trabajo distinto:
const int PIN_ZUMBADOR_RITMO = 9;   // pita mas rapido cuanto mas cerca
const int PIN_ZUMBADOR_FIJO  = 8;   // tono continuo cuando hay que PARAR

const int PIN_LED = 13;             // LED de la placa, con su resistencia

// ---------------------------------------------------------------------------
// QUE TIPO ES CADA ZUMBADOR
//
// Se sabe con el sketch Prueba_Zumbador, mirando en que fase suena:
//
//     sono en la FASE A (tone) ......  es PASIVO   -> ponlo en true
//     sono en la FASE C (5V fijo) ...  es ACTIVO   -> dejalo en false
//
// AVISO IMPORTANTE: en el UNO, tone() solo puede sonar en UN pin a la vez.
// Usa un unico temporizador, y al llamarlo sobre un segundo pin el primero se
// calla. Por eso LOS DOS ZUMBADORES NO PUEDEN SER PASIVOS: como mucho uno.
//
// Si los dos son activos, no hay ningun problema: cada uno se maneja con
// digitalWrite() y suenan a la vez sin estorbarse.
// ---------------------------------------------------------------------------
const bool RITMO_ES_PASIVO = false;
const bool FIJO_ES_PASIVO  = true;

// ---------------------------------------------------------------------------
// Umbrales, derivados del alcance del sensor
// ---------------------------------------------------------------------------
const float DISTANCIA_MAXIMA = 400.0;                  // cm, hoja de datos
const float UMBRAL_LEJOS  = DISTANCIA_MAXIMA * 0.50;   // 200 cm
const float UMBRAL_MEDIO  = DISTANCIA_MAXIMA * 0.26;   // 104 cm
const float UMBRAL_MINIMO = 2.0;                       // el sensor no baja de 2

// ---------------------------------------------------------------------------
// El ritmo del pitido en funcion de la distancia
//
// Por debajo de DISTANCIA_CONTINUO el pitido deja de cortarse: se queda fijo.
// Entre ahi y DISTANCIA_AVISO el intervalo crece de forma lineal, desde
// INTERVALO_MIN (muy rapido, cerca) hasta INTERVALO_MAX (muy lento, lejos).
// Mas alla de DISTANCIA_AVISO se queda en el ritmo mas lento.
//
// ESTAS SON LAS CUATRO CIFRAS QUE HAY QUE TOCAR PARA AJUSTAR EL COMPORTAMIENTO:
//
//   ¿suena continuo demasiado pronto?  ->  baja DISTANCIA_CONTINUO
//   ¿no se nota el cambio de ritmo?    ->  baja DISTANCIA_AVISO
//   ¿va todo demasiado rapido?         ->  sube INTERVALO_MIN
// ---------------------------------------------------------------------------
const float DISTANCIA_CONTINUO =  10.0;         // cm; por debajo, tono fijo
const float DISTANCIA_AVISO    = 100.0;         // cm; alcance util del aviso
const unsigned long INTERVALO_MIN =  60;        // ms, a 10 cm
const unsigned long INTERVALO_MAX = 800;        // ms, a 100 cm o mas

// Ida y vuelta de 400 cm son 800 cm de recorrido: 800 * 29.1 = 23280 us.
// Se redondea a 25000 para dejar margen.
const unsigned long TIEMPO_LIMITE_ECO = 25000UL;

// ---------------------------------------------------------------------------
// Ritmo de las otras dos tareas, en milisegundos
// ---------------------------------------------------------------------------
const unsigned long PERIODO_MEDICION = 60;    // el HC-SR04 necesita >= 50 ms
const unsigned long PERIODO_REPORTE  = 500;

const int TONO_PITIDO = 1000;   // Hz del zumbador de ritmo
const int TONO_PARADA = 1800;   // Hz del zumbador fijo, mas agudo para
                                // distinguirlo si los dos son pasivos.
                                // Solo se usan si el zumbador es pasivo.

// ---------------------------------------------------------------------------
// Las cuatro zonas, con nombre en vez de 0, 1, 2, 3
// ---------------------------------------------------------------------------
enum Zona {
  FUERA_DE_RANGO,
  ZONA_LEJANA,
  ZONA_MEDIA,
  ZONA_CERCANA
};

// ---------------------------------------------------------------------------
// Estado
// ---------------------------------------------------------------------------
float distancia  = -1.0;
Zona  zonaActual = FUERA_DE_RANGO;

bool  enMarcha   = false;     // ¿la "maquina" esta funcionando?
bool  sonando    = false;     // fase actual del pitido intermitente

unsigned long intervaloActual = INTERVALO_MAX;   // ms entre pitido y pitido

unsigned long tMedicion = 0;
unsigned long tPitido   = 0;
unsigned long tReporte  = 0;

// ===========================================================================
void setup() {
  Serial.begin(9600);

  pinMode(PIN_TRIG,           OUTPUT);
  pinMode(PIN_ECHO,           INPUT);
  pinMode(PIN_LED,            OUTPUT);
  pinMode(PIN_ZUMBADOR_RITMO, OUTPUT);
  pinMode(PIN_ZUMBADOR_FIJO,  OUTPUT);

  digitalWrite(PIN_TRIG, LOW);

  Serial.println(F("== Tarea 3: detector de distancia (version fisica) =="));
  Serial.println(F("Cristian Carrera - 2024-1932 - ITLA"));
  Serial.println();

  // Se imprimen los pines al arrancar: si algun dia algo no responde, lo
  // primero que se ve es que espera el programa.
  Serial.print(F("Pines -> TRIG: "));   Serial.print(PIN_TRIG);
  Serial.print(F("   ECHO: "));         Serial.print(PIN_ECHO);
  Serial.print(F("   RITMO: "));        Serial.print(PIN_ZUMBADOR_RITMO);
  Serial.print(F("   FIJO: "));         Serial.println(PIN_ZUMBADOR_FIJO);

  Serial.println(F("El pitido va mas rapido cuanto mas cerca este el obstaculo."));
  Serial.print(F("Menos de ")); Serial.print(DISTANCIA_CONTINUO, 0);
  Serial.println(F(" cm: tono continuo."));
  Serial.println();
}

// ---------------------------------------------------------------------------
// loop() es el planificador: ofrece el turno a cada tarea, sin bloquear nunca.
// ---------------------------------------------------------------------------
void loop() {
  unsigned long ahora = millis();

  tareaMedirDistancia(ahora);
  tareaLuzYSonido(ahora);
  tareaReportar(ahora);
}

// ===========================================================================
// TAREA 1 - Medir, clasificar y recalcular el ritmo
// ===========================================================================
void tareaMedirDistancia(unsigned long ahora) {
  if (ahora - tMedicion < PERIODO_MEDICION) return;
  tMedicion = ahora;

  distancia  = medirDistanciaFiltrada();
  zonaActual = clasificarZona(distancia);

  // El ritmo se recalcula en CADA medicion, no una vez por zona. Por eso
  // responde enseguida cuando la mano se acerca de golpe.
  intervaloActual = intervaloPorDistancia(distancia);

  // Aqui esta la decision que pide el enunciado: continuar o detenerse.
  enMarcha = (zonaActual == ZONA_LEJANA || zonaActual == ZONA_MEDIA);
}

// ---------------------------------------------------------------------------
// Cuantos milisegundos entre un pitido y el siguiente.
// Devuelve 0 cuando esta tan cerca que el tono ya no debe cortarse.
// ---------------------------------------------------------------------------
unsigned long intervaloPorDistancia(float d) {
  if (d < UMBRAL_MINIMO)        return INTERVALO_MAX;   // sin eco: da igual
  if (d <= DISTANCIA_CONTINUO)  return 0;               // continuo
  if (d >= DISTANCIA_AVISO)     return INTERVALO_MAX;   // lejos: ritmo minimo

  // Regla de tres entre los dos extremos: 0 justo encima del umbral continuo,
  // 1 al llegar al alcance del aviso.
  float t = (d - DISTANCIA_CONTINUO) / (DISTANCIA_AVISO - DISTANCIA_CONTINUO);
  return INTERVALO_MIN + (unsigned long)(t * (INTERVALO_MAX - INTERVALO_MIN));
}

// ---------------------------------------------------------------------------
// Una lectura suelta, en centimetros. Devuelve -1 si no volvio eco.
// ---------------------------------------------------------------------------
float medirDistanciaCruda() {
  // Pulso de disparo de 10 us, como pide la hoja de datos.
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIEMPO_LIMITE_ECO);
  if (duracion == 0) return -1.0;          // no hay nada delante

  return (duracion / 29.1) / 2.0;          // el /2 es por la ida y la vuelta
}

// ---------------------------------------------------------------------------
// Tres lecturas y se devuelve la MEDIANA, no el promedio.
//
// El HC-SR04 suelta de vez en cuando una lectura disparatada. Una sola medida
// absurda arruinaria el promedio; a la mediana no le afecta, porque un valor
// extremo se queda en un borde al ordenar y nunca cae en el medio.
// ---------------------------------------------------------------------------
float medirDistanciaFiltrada() {
  float m[3];
  for (int i = 0; i < 3; i++) {
    m[i] = medirDistanciaCruda();
    delayMicroseconds(500);
  }

  // Ordenar tres valores a mano sale mas barato que llamar a un algoritmo.
  if (m[0] > m[1]) intercambiar(m[0], m[1]);
  if (m[1] > m[2]) intercambiar(m[1], m[2]);
  if (m[0] > m[1]) intercambiar(m[0], m[1]);

  return m[1];
}

void intercambiar(float &a, float &b) {
  float t = a; a = b; b = t;
}

// ---------------------------------------------------------------------------
Zona clasificarZona(float d) {
  if (d < UMBRAL_MINIMO)  return FUERA_DE_RANGO;   // -1 o demasiado cerca
  if (d > UMBRAL_LEJOS)   return ZONA_LEJANA;
  if (d > UMBRAL_MEDIO)   return ZONA_MEDIA;
  return ZONA_CERCANA;
}

// ===========================================================================
// TAREA 2 - El pitido, con el LED parpadeando a su mismo ritmo
// ===========================================================================
void tareaLuzYSonido(unsigned long ahora) {

  // --- Zumbador FIJO: la orden de parar -----------------------------------
  // No pulsa nunca. O suena entero o esta callado, y eso lo hace inconfundible
  // frente al otro. Suena exactamente cuando la maquina esta detenida.
  if (enMarcha || zonaActual == FUERA_DE_RANGO) {
    callarFijo();
  } else {
    sonarFijo();
  }

  // --- Zumbador de RITMO: a que distancia estamos -------------------------

  // Nada delante: silencio total.
  if (zonaActual == FUERA_DE_RANGO) {
    digitalWrite(PIN_LED, LOW);
    callarRitmo();
    sonando = false;
    return;
  }

  // Tan cerca que el aviso ya no se interrumpe.
  if (intervaloActual == 0) {
    digitalWrite(PIN_LED, HIGH);
    sonarRitmo();
    sonando = true;
    return;
  }

  // Ritmo normal: se alterna sonido y silencio cada intervaloActual ms.
  if (ahora - tPitido >= intervaloActual) {
    tPitido = ahora;
    sonando = !sonando;
    digitalWrite(PIN_LED, sonando);
    if (sonando) sonarRitmo(); else callarRitmo();
  }
}

// ---------------------------------------------------------------------------
// Una sola puerta para el sonido. Recibe QUE zumbador y de que tipo es, asi
// el resto del programa no tiene que saber nada de tone() ni de digitalWrite.
// ---------------------------------------------------------------------------
void sonarEn(int pin, bool esPasivo, int frecuencia) {
  if (esPasivo) {
    tone(pin, frecuencia);
  } else {
    digitalWrite(pin, HIGH);            // el activo se enciende y ya
  }
}

void callarEn(int pin, bool esPasivo) {
  if (esPasivo) {
    noTone(pin);
  } else {
    digitalWrite(pin, LOW);
  }
}

// Atajos para no repetir los parametros en cada llamada.
void sonarRitmo() { sonarEn(PIN_ZUMBADOR_RITMO, RITMO_ES_PASIVO, TONO_PITIDO); }
void callarRitmo() { callarEn(PIN_ZUMBADOR_RITMO, RITMO_ES_PASIVO); }

void sonarFijo()  { sonarEn(PIN_ZUMBADOR_FIJO, FIJO_ES_PASIVO, TONO_PARADA); }
void callarFijo() { callarEn(PIN_ZUMBADOR_FIJO, FIJO_ES_PASIVO); }

// ===========================================================================
// TAREA 3 - Informe por el monitor serie (9600 baudios)
// ===========================================================================
void tareaReportar(unsigned long ahora) {
  if (ahora - tReporte < PERIODO_REPORTE) return;
  tReporte = ahora;

  Serial.print(F("Distancia: "));
  if (distancia < 0) {
    Serial.print(F("sin eco"));
  } else {
    Serial.print(distancia, 1);
    Serial.print(F(" cm"));
  }

  Serial.print(F("  |  Zona: "));
  Serial.print(nombreZona(zonaActual));

  // Se muestra el ritmo en numeros: asi se ve en el video que el pitido no
  // va por escalones, sino cambiando de forma continua con la distancia.
  Serial.print(F("  |  Pitido: "));
  if (zonaActual == FUERA_DE_RANGO) {
    Serial.print(F("callado"));
  } else if (intervaloActual == 0) {
    Serial.print(F("CONTINUO"));
  } else {
    Serial.print(F("cada "));
    Serial.print(intervaloActual);
    Serial.print(F(" ms"));
  }

  Serial.print(F("  |  Maquina: "));
  Serial.println(enMarcha ? F("EN MARCHA") : F("DETENIDA"));
}

const __FlashStringHelper *nombreZona(Zona z) {
  switch (z) {
    case ZONA_LEJANA:  return F("LEJOS (via libre)");
    case ZONA_MEDIA:   return F("MEDIA (precaucion)");
    case ZONA_CERCANA: return F("CERCA (parada)");
    default:           return F("FUERA DE RANGO");
  }
}
