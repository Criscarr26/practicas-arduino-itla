/*
  ============================================================================
  Detector de oscuridad que emite S.O.S.
  ============================================================================
  Autor : Cristian Carrera
  Placa : Elegoo UNO R3 (compatible Arduino UNO)

  Se uso asistencia de IA (Claude) para depurar y documentar. El montaje, la
  calibracion y las pruebas sobre la placa son propios.

  ============================================================================
  EL OBJETIVO, TEXTUAL
  ============================================================================
  "Medidor de ausencia o no de luz: Solo emitir el sonido del S.O.S. cuando no
   exista luz en el entorno o ambiente en donde se pruebe o ejecute la
   funcionalidad de dicho detector."

  Eso es todo: una magnitud que se mide, una condicion que se evalua, y un
  S.O.S. que suena mientras esa condicion se cumpla.

  El planteamiento original ofrecia cuatro variantes -A temperatura, B humedad, C luz, y D
  cualquiera de esas mas un display- y pide realizar UNA. Se eligio la C.

  ============================================================================
  CODIGO DE PARTIDA (tutorial "night light" tal cual se dio, sin modificar)
  ============================================================================
  Este es el sketch base del que se partio. A partir de aqui se hicieron las
  cuatro investigaciones que siguen, hasta llegar a la version final que
  esta mas abajo en este mismo archivo.

  // arduino tutorials:
  // https://www.sciencebuddies.org/science-fair-projects/references/how-to-use-an-arduino
  //
  // automatic "night light"
  // turn LED on when light levels drop too low
  //
  // const int led = 8;          // led pin
  // const int sensor_pin = A0;  // sensor pin
  // int sensor;                 // sensor reading
  // const int threshold = 500;  // threshold to turn LED on
  //
  // void setup(){  // setup code that only runs once
  //   pinMode(led, OUTPUT);  // set LED pin as output
  //   Serial.begin(9600);    // initialize serial communication
  // }
  //
  // void loop(){   // code that loops forever
  //   sensor = analogRead(sensor_pin);   // read sensor value
  //   Serial.println(sensor);           // print sensor value
  //   if(sensor<threshold){  // if sensor reading is less than threshold
  //     digitalWrite(led,HIGH);  // turn LED on
  //   }
  //   else{  // else, if sensor reading is greater than threshold
  //     digitalWrite(led,LOW);    // turn LED off
  //   }
  // }

  ============================================================================
  INVESTIGACION PREVIA 1: COMO SE MIDE LA LUZ CON UNA LDR
  ============================================================================
  Una LDR (fotorresistencia) es una resistencia que CAMBIA de valor segun la
  luz que le llega:

        a oscuras ....... cientos de miles de ohmios, hasta megaohmios
        con luz ......... unos cientos de ohmios

  El problema es que Arduino no sabe medir resistencia. Sus entradas
  analogicas solo miden VOLTAJE, de 0 a 5 V, y lo convierten a un numero de
  0 a 1023.

  Se resuelve con un DIVISOR DE TENSION: se pone la LDR en serie con una
  resistencia fija de 10k y se lee el punto intermedio.

        5V ---- LDR ----+---- 10k ---- GND
                        |
                        A0

  Al cambiar la LDR cambia el reparto del voltaje entre las dos, y ese punto
  medio sube o baja. Con este orden:

        hay luz  -> la LDR baja de valor -> A0 sube  -> numero ALTO
        oscuro   -> la LDR sube de valor -> A0 baja  -> numero BAJO

  Si al montarlo sale al reves -tapar la LDR sube el numero- no hay que
  recablear: se cambia OSCURO_ES_VALOR_BAJO aqui abajo y listo.

  ============================================================================
  INVESTIGACION PREVIA 2: POR QUE DOS UMBRALES Y NO UNO
  ============================================================================
  Lo natural seria un solo numero: por debajo de 300, oscuro.

  El problema es lo que pasa JUSTO en ese punto. La lectura nunca es estable
  del todo, y con una sola frontera el detector se pone a entrar y salir de la
  alarma varias veces por segundo. El S.O.S. arrancaria y se cortaria sin
  parar. Eso se llama rebote o parpadeo de estado.

  La solucion se llama HISTERESIS: se usan DOS umbrales separados.

        entra en alarma cuando baja de 280
        sale de la alarma cuando sube de 380

  Entre 280 y 380 no pasa nada: se mantiene el estado en el que estuviera. Asi
  hace falta un cambio de luz de verdad para que conmute, y no el ruido de la
  lectura. Es el mismo principio del termostato de una nevera.

  ============================================================================
  LOS UMBRALES HAY QUE CALIBRARLOS, NO COPIARLOS
  ============================================================================
  Los numeros que salen de la LDR dependen de tres cosas: el modelo concreto de
  fotorresistencia, el valor de la resistencia fija, y la luz que haya en el
  sitio donde se prueba. Un tutorial cualquiera pone 500 porque es lo que le
  salia a el en su cuarto; no tiene por que servir aqui.

  Por eso el programa muestra en el monitor serie el valor MINIMO y el MAXIMO
  que ha visto desde que arranco. Calibrarlo son treinta segundos:

     1. Subir el sketch y abrir el monitor serie a 9600.
     2. Dejar el ambiente con la luz normal unos segundos: ese es el maximo.
     3. Tapar la LDR con el dedo: ese es el minimo.
     4. Poner los dos umbrales entre esos dos valores, separados entre si.
        Con min=90 y max=700, por ejemplo, van bien 280 y 380.

  Si al tapar la LDR el numero SUBE en vez de bajar, no hay que recablear: se
  cambia OSCURO_ES_VALOR_BAJO a false.

  ============================================================================
  INVESTIGACION PREVIA 3: EL CODIGO MORSE DEL S.O.S.
  ============================================================================
  El S.O.S. no son las iniciales de ninguna frase. Se eligio en 1906 porque su
  patron es el mas simple e inconfundible:

        S = tres puntos    . . .
        O = tres rayas     _ _ _
        S = tres puntos    . . .

  El estandar fija todas las duraciones como multiplos de la unidad basica,
  que es lo que dura un punto:

        punto ................... 1 unidad
        raya .................... 3 unidades
        espacio entre simbolos .. 1 unidad
        espacio entre letras .... 3 unidades
        espacio entre palabras .. 7 unidades

  Por eso el patron se guarda como una lista de duraciones en unidades, no en
  milisegundos: cambiando UNIDAD se acelera o se ralentiza el mensaje entero
  sin tocar nada mas.

  ============================================================================
  INVESTIGACION PREVIA 4: SOPORTA ARDUINO HILOS O TAREAS?
  ============================================================================
  El ATmega328P del UNO tiene UN nucleo y ningun sistema operativo: hilos
  reales NO hay. Pero si hay CONCURRENCIA, que no es lo mismo que paralelismo.

     Secuencial  -> una instruccion tras otra, bloqueando
     Concurrente -> varias tareas PROGRESAN INTERCALADAS      (si, con millis)
     Paralelo    -> varias tareas se ejecutan A LA VEZ        (no: 1 nucleo)
     Distribuido -> tareas en MAQUINAS distintas, por mensajes (si, con red)

  Aqui hace falta de verdad: mientras suena el S.O.S. hay que seguir midiendo
  la luz, porque si se enciende la lampara la alarma tiene que callarse en el
  acto. Con delay() eso es imposible: el ciclo del S.O.S. dura 5,1 segundos y
  durante todos ellos el micro estaria bloqueado, sordo y ciego.

  Por eso en este programa NO HAY UN SOLO delay(). El S.O.S. esta escrito como
  MAQUINA DE ESTADOS: en cada vuelta mira el reloj, y si al simbolo actual ya
  se le acabo el tiempo, pasa al siguiente. Entre simbolo y simbolo el
  programa queda libre para hacer lo demas.

  ============================================================================
  INVESTIGACION PREVIA 5: ESTRUCTURAS DE CONTROL
  ============================================================================
    if / else if -> decide por RANGOS. Aqui compara la luz medida contra los
                    dos umbrales de la histeresis, y decide si a cada simbolo
                    del Morse le toca sonar o callar.
    switch       -> elige entre VALORES discretos. Aqui NO se usa, y conviene
                    decir por que: solo hay dos estados posibles -hay luz o no
                    la hay- y para dos casos un if se lee mejor que un switch.
                    En la version de simulador, que maneja varios estados de
                    temperatura y humedad, si esta justificado y si se usa.
    for          -> repite un numero CONOCIDO de veces. Aqui tampoco se usa, y
                    tambien es una decision: recorrer el patron del S.O.S. con
                    un for obligaria a esperar dentro del bucle, y entonces el
                    programa dejaria de medir la luz mientras suena la alarma.
                    En su lugar el patron se recorre con un indice (pasoMorse)
                    que avanza una posicion por vuelta del loop, sin bloquear.
    while        -> repite MIENTRAS se cumpla algo. No se usa por la misma
                    razon que el for. El unico while del programa es el que
                    Arduino tiene por dentro, que llama a loop() sin parar.

  La conclusion es la interesante: en un programa que no puede bloquearse, los
  bucles clasicos se sustituyen por una maquina de estados que avanza un paso
  en cada vuelta. Es el mismo trabajo, repartido en el tiempo.


  ============================================================================
  MONTAJE
  ============================================================================
     LDR (fotorresistencia)          Arduino
       una pata  ----------------->   5V
       otra pata ----------------->   A0   y ademas
                                      A0 ---- 10k ---- GND

     La LDR no tiene polaridad: da igual cual pata va a cada lado.

     Zumbador                        Arduino
       + (pata larga) ------------>   pin 9
       - (pata corta) ------------>   GND

  El LED del pin 13 ya viene soldado en la placa, con su resistencia. No hay
  que conectar nada para usarlo.

     LED externo                     Arduino
       anodo (pata larga) -------->   pin 8, a traves de una resistencia
       cátodo (pata corta) ------->   GND                   (220-330 ohm)

  Este LED externo queda ENCENDIDO FIJO mientras dure la oscuridad (no
  parpadea el S.O.S.); es solo un indicador visual aparte del zumbador y del
  LED 13, que si siguen el patron Morse.

  Total: cuatro cables y dos resistencias. El zumbador ya estaba en el pin 9
  de otro montaje, asi que solo se añaden la LDR y el LED del pin 8.

  Ninguna libreria. Nada que instalar.
  ============================================================================
*/

// ---------------------------------------------------------------------------
// Mapa de pines
// ---------------------------------------------------------------------------
const int PIN_LDR          = A0;
const int PIN_ZUMBADOR     = 9;
const int PIN_LED          = 13;  // el de la placa, con su resistencia incluida
const int PIN_LED_EXTERNO  = 8;   // LED aparte, como en el sketch original

// ---------------------------------------------------------------------------
// QUE TIPO DE ZUMBADOR HAY CONECTADO
//
// Pasivo : no suena solo, hay que darle la frecuencia con tone().
// Activo : trae su propio oscilador; basta con darle corriente.
//
// El del montaje es ACTIVO, comprobado con el sketch Prueba_Zumbador: sono en
// la fase de 5V fijo y no en la de tone(). Por eso esto va en false.
//
// El S.O.S. se distingue igual con los dos, porque lo que lo identifica es el
// RITMO de los simbolos, no la altura del tono.
// ---------------------------------------------------------------------------
const bool ZUMBADOR_PASIVO = false;

// ---------------------------------------------------------------------------
// Sentido de la lectura.
//
// Con el montaje de arriba, oscuro da numeros BAJOS. Si al probarlo sale al
// reves -tapar la LDR sube el numero- se cambia esto a false y no hay que
// tocar ni un cable.
// ---------------------------------------------------------------------------
const bool OSCURO_ES_VALOR_BAJO = true;

// ---------------------------------------------------------------------------
// Los dos umbrales de la histeresis (ver la investigacion previa 2)
// ---------------------------------------------------------------------------
const int UMBRAL_ENTRA_OSCURO = 280;   // por debajo de esto, arranca el S.O.S.
const int UMBRAL_SALE_OSCURO  = 380;   // por encima de esto, se calla

// ---------------------------------------------------------------------------
// El patron del S.O.S., en unidades Morse.
// Posiciones pares = suena; impares = silencio.
//
//    . . .        _ _ _        . . .
// ---------------------------------------------------------------------------
const int PATRON_SOS[] = {
  1,1, 1,1, 1,3,      // S: tres puntos, y espacio de letra
  3,1, 3,1, 3,3,      // O: tres rayas,  y espacio de letra
  1,1, 1,1, 1,7       // S: tres puntos, y espacio de palabra
};
const int PASOS_SOS = sizeof(PATRON_SOS) / sizeof(PATRON_SOS[0]);

const unsigned long UNIDAD = 150;   // ms que dura un punto
const int TONO_SOS = 1000;          // Hz; solo se usa si el zumbador es pasivo

// ---------------------------------------------------------------------------
// Ritmo de las tareas. Ningun delay() en todo el programa.
// ---------------------------------------------------------------------------
const unsigned long PERIODO_MEDICION = 100;
const unsigned long PERIODO_REPORTE  = 500;

// ---------------------------------------------------------------------------
// Estado
// ---------------------------------------------------------------------------
int  lecturaLuz   = 0;
bool hayOscuridad = false;      // ¿estamos en alarma?

// Extremos vistos desde que arranco, para poder calibrar los umbrales.
int  luzMinima = 1023;
int  luzMaxima = 0;

int  pasoMorse      = 0;        // en que simbolo del patron vamos
bool morseArrancado = false;

unsigned long tMedicion = 0;
unsigned long tMorse    = 0;
unsigned long tReporte  = 0;

// ===========================================================================
void setup() {
  Serial.begin(9600);

  pinMode(PIN_ZUMBADOR,    OUTPUT);
  pinMode(PIN_LED,         OUTPUT);
  pinMode(PIN_LED_EXTERNO, OUTPUT);
  // Las entradas analogicas no necesitan pinMode para leerlas.

  Serial.println(F("== Detector de oscuridad =="));
  Serial.println(F("Cristian Carrera"));
  Serial.println();
  Serial.print(F("Entra en alarma por debajo de "));   Serial.println(UMBRAL_ENTRA_OSCURO);
  Serial.print(F("Sale de la alarma por encima de ")); Serial.println(UMBRAL_SALE_OSCURO);
  Serial.println(F("Entre esos dos valores se mantiene el estado (histeresis)."));
  Serial.println(F("Usa el [min/max] del final de cada linea para calibrarlos."));
  Serial.println();
}

// ---------------------------------------------------------------------------
// loop() es el planificador: le ofrece el turno a cada tarea, sin bloquear.
// ---------------------------------------------------------------------------
void loop() {
  unsigned long ahora = millis();

  tareaMedirLuz(ahora);
  tareaAlarma(ahora);
  tareaReportar(ahora);
}

// ===========================================================================
// TAREA 1 - Medir la luz y decidir si hay oscuridad
// ===========================================================================
void tareaMedirLuz(unsigned long ahora) {
  if (ahora - tMedicion < PERIODO_MEDICION) return;
  tMedicion = ahora;

  lecturaLuz = analogRead(PIN_LDR);
  int claridad = claridadNormalizada();

  if (claridad < luzMinima) luzMinima = claridad;
  if (claridad > luzMaxima) luzMaxima = claridad;

  // Aqui esta la histeresis: cada frontera se comprueba solo en el sentido en
  // el que puede cruzarse. Estando a oscuras no se mira el umbral de entrada,
  // y estando en claro no se mira el de salida.
  if (!hayOscuridad && claridad < UMBRAL_ENTRA_OSCURO) {
    hayOscuridad = true;
    digitalWrite(PIN_LED_EXTERNO, HIGH);  // fijo mientras dure la oscuridad
    reiniciarMorse();
  } else if (hayOscuridad && claridad > UMBRAL_SALE_OSCURO) {
    hayOscuridad = false;
    digitalWrite(PIN_LED_EXTERNO, LOW);
    callar();
    digitalWrite(PIN_LED, LOW);
  }
}

// ---------------------------------------------------------------------------
// Devuelve siempre "cuanta luz hay", de 0 a 1023, sin importar como este
// cableada la LDR. Asi el resto del programa razona en un solo sentido.
// ---------------------------------------------------------------------------
int claridadNormalizada() {
  return OSCURO_ES_VALOR_BAJO ? lecturaLuz : (1023 - lecturaLuz);
}

// ===========================================================================
// TAREA 2 - El S.O.S., como maquina de estados
//
// No usa delay(): en cada vuelta mira si al simbolo actual se le acabo el
// tiempo. Si no, se sale y deja el micro libre. Por eso la luz se sigue
// midiendo mientras suena, y la alarma se corta en el acto al dar luz.
// ===========================================================================
void tareaAlarma(unsigned long ahora) {
  if (!hayOscuridad) return;

  // Al entrar en alarma hay que emitir YA el primer simbolo. Sin esto se
  // esperaria la duracion del paso 0 antes de sonar, y el primer punto del
  // mensaje se perderia.
  if (!morseArrancado) {
    morseArrancado = true;
    tMorse = ahora;
    aplicarPasoMorse();
    return;
  }

  unsigned long duracion = (unsigned long)PATRON_SOS[pasoMorse] * UNIDAD;
  if (ahora - tMorse < duracion) return;      // al simbolo actual aun le queda

  tMorse = ahora;
  pasoMorse++;
  if (pasoMorse >= PASOS_SOS) pasoMorse = 0;  // el mensaje se repite
  aplicarPasoMorse();
}

// ---------------------------------------------------------------------------
// Las posiciones pares del patron suenan; las impares son silencio.
// El LED sigue al zumbador, para que el S.O.S. tambien se vea.
// ---------------------------------------------------------------------------
void aplicarPasoMorse() {
  bool suena = (pasoMorse % 2 == 0);
  digitalWrite(PIN_LED, suena);
  if (suena) sonar(TONO_SOS); else callar();
}

void reiniciarMorse() {
  pasoMorse = 0;
  morseArrancado = false;
}

// ---------------------------------------------------------------------------
// Una sola puerta para el sonido, valida para los dos tipos de zumbador. Asi
// el resto del programa dice "suena" o "callate" sin saber cual esta puesto.
// ---------------------------------------------------------------------------
void sonar(int frecuencia) {
  if (ZUMBADOR_PASIVO) {
    tone(PIN_ZUMBADOR, frecuencia);
  } else {
    digitalWrite(PIN_ZUMBADOR, HIGH);   // el activo se enciende y ya
  }
}

void callar() {
  if (ZUMBADOR_PASIVO) {
    noTone(PIN_ZUMBADOR);
  } else {
    digitalWrite(PIN_ZUMBADOR, LOW);
  }
}

// ===========================================================================
// TAREA 3 - Informe por el monitor serie (9600 baudios)
// ===========================================================================
void tareaReportar(unsigned long ahora) {
  if (ahora - tReporte < PERIODO_REPORTE) return;
  tReporte = ahora;

  Serial.print(F("Luz: "));
  Serial.print(claridadNormalizada());
  Serial.print(F("/1023"));

  Serial.print(F("  |  Ambiente: "));
  Serial.print(hayOscuridad ? F("OSCURO ") : F("CON LUZ"));

  Serial.print(F("  |  Alarma: "));
  Serial.print(hayOscuridad ? F("S.O.S. SONANDO") : F("en silencio"));

  // Para calibrar: tapar la LDR baja el minimo, destaparla sube el maximo.
  // Los dos umbrales deben quedar entre estos dos numeros.
  Serial.print(F("   [min "));
  Serial.print(luzMinima);
  Serial.print(F(" / max "));
  Serial.print(luzMaxima);
  Serial.println(F("]"));
}
