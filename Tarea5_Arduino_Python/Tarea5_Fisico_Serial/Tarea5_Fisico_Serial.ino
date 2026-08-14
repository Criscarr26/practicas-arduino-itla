/*
  ============================================================================
  PRACTICA 5 - Lectura de los datos sensados de Arduino desde otro lenguaje
               (parte 1 de 2: el firmware de Arduino)
  ============================================================================
  Asignatura : Inteligencia Artificial e Internet de las Cosas (2026-C-2)
  Profesor   : Luis Bessewell Feliz
  Estudiante : Cristian Carrera - Matricula 2024-1932
  Institucion: Instituto Tecnologico de Las Americas (ITLA)
  Placa      : Elegoo UNO R3 (compatible Arduino UNO)
  Valor      : 8 puntos

  La parte 2 es el programa lector, escrito en Python, que esta en la carpeta
  de al lado: lector_serial.py

  ============================================================================
  INVESTIGACION PREVIA: COMO SE COMUNICA ARDUINO CON UNA COMPUTADORA
  ============================================================================
  El UNO R3 no tiene WiFi ni Bluetooth. Su via de comunicacion es el PUERTO
  SERIE, el mismo cable USB con el que se le carga el programa.

  QUE PASA POR DENTRO DEL CABLE:
  El ATmega328P habla UART, un protocolo serie a nivel de 5 V. Una computadora
  moderna no entiende eso, asi que la placa lleva un segundo chip que hace de
  traductor USB<->serie. En el UNO original es un ATmega16U2; en los clones y
  en muchos Elegoo es un CH340. Ese chip es el que crea el "puerto COM" que
  aparece en Windows.

  POR ESO IMPORTA EL DRIVER: si Windows no reconoce la placa, casi siempre es
  que falta el driver del CH340. Es el problema numero uno con placas clonicas.

  LA VELOCIDAD (BAUDIOS):
  Serial.begin(9600) fija 9600 bits por segundo. Los dos extremos TIENEN que
  usar el mismo numero; si no coinciden se reciben caracteres basura. No es un
  "error de codigo", es que se estan leyendo los bits a destiempo.

  EL DETALLE QUE MAS CONFUNDE - EL AUTO-RESET:
  Al abrir el puerto serie, la señal DTR reinicia la placa. Es decir: el
  programa de Python abre el puerto y Arduino ARRANCA DE CERO en ese momento.
  Por eso el lector de Python espera dos segundos antes de leer nada; si no,
  recibe los primeros mensajes cortados o vacios.

  ============================================================================
  QUE FORMATO USAR PARA ENVIAR LOS DATOS
  ============================================================================
  Se podria imprimir "La temperatura es de 25 grados", pero eso obliga al
  programa receptor a descifrar el texto. Es fragil y feo.

  Aqui se usa CSV, valores separados por comas, con una cabecera al arrancar:

      LECTURA,MILIS,TEMPERATURA,HUMEDAD,LUZ,DISTANCIA

  Ventajas concretas:
    - Python lo parte con una sola llamada a split(",")
    - Se abre directo en Excel para el informe
    - Ocupa muy pocos bytes, y a 9600 baudios eso importa
    - Si mañana se agrega un sensor, solo se añade una columna

  Alternativa mas potente: JSON. Es autodescriptivo y no depende del orden,
  pero ocupa el triple y en un ATmega328 con 2 KB de RAM eso pesa. Para cinco
  campos, CSV es la eleccion correcta.

  ============================================================================
  MONTAJE (los tres sensores son los del kit Elegoo)
  ============================================================================
     DHT11:   DATA -> pin 2,  VCC -> 5V,  GND -> GND
     LDR:     un extremo -> 5V
              otro extremo -> A0 y ademas a GND por una resistencia de 10k
              (esto es un DIVISOR DE TENSION: sin la resistencia el pin
               queda "al aire" y lee ruido)
     HC-SR04: TRIG -> pin 9,  ECHO -> pin 10,  VCC -> 5V,  GND -> GND
     LED integrado del pin 13: parpadea al enviar cada lectura

  Si no tienes alguno de los sensores, comenta su #define de abajo: el
  programa sigue funcionando y envia -1 en esa columna.

  ============================================================================
  LIBRERIAS
  ============================================================================
     "DHT sensor library" de Adafruit + "Adafruit Unified Sensor"
  ============================================================================
*/

#define USAR_DHT       // comentar esta linea si no tienes el DHT11
#define USAR_LDR       // comentar si no tienes la fotorresistencia
#define USAR_SONAR     // comentar si no tienes el HC-SR04

#ifdef USAR_DHT
  #include <DHT.h>
  #define PIN_DHT   2
  #define TIPO_DHT  DHT11
  DHT dht(PIN_DHT, TIPO_DHT);
#endif

const int PIN_LDR  = A0;
const int PIN_TRIG = 9;
const int PIN_ECHO = 10;
const int PIN_LED  = 13;

// Un envio por segundo. Suficiente para graficar y respeta el limite del
// DHT11, que no admite mas de una lectura por segundo.
const unsigned long PERIODO_ENVIO = 1000;

unsigned long tEnvio = 0;
unsigned long contadorLecturas = 0;

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  pinMode(PIN_LED, OUTPUT);
#ifdef USAR_SONAR
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
#endif
#ifdef USAR_DHT
  dht.begin();
#endif

  // Se espera a que el puerto quede estable antes de escribir la cabecera.
  delay(1500);

  // Cabecera CSV. El programa de Python la reconoce y la usa para nombrar
  // las columnas, en lugar de tenerlas escritas a mano en dos sitios.
  Serial.println(F("LECTURA,MILIS,TEMPERATURA,HUMEDAD,LUZ,DISTANCIA"));
}

// ---------------------------------------------------------------------------
void loop() {
  unsigned long ahora = millis();
  if (ahora - tEnvio < PERIODO_ENVIO) return;
  tEnvio = ahora;

  contadorLecturas++;

  float temperatura = -1;
  float humedad     = -1;
  int   luz         = -1;
  float distancia   = -1;

#ifdef USAR_DHT
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  // isnan detecta la lectura fallida. Se envia -1 para que el receptor
  // sepa que ese dato no es valido, en vez de mandarle un cero enganoso.
  if (!isnan(t)) temperatura = t;
  if (!isnan(h)) humedad     = h;
#endif

#ifdef USAR_LDR
  // 0 = oscuridad total, 1023 = luz plena. El ADC del UNO es de 10 bits.
  luz = analogRead(PIN_LDR);
#endif

#ifdef USAR_SONAR
  distancia = medirDistancia();
#endif

  enviarCSV(contadorLecturas, ahora, temperatura, humedad, luz, distancia);

  // Parpadeo corto: confirma visualmente que se envio una lectura.
  digitalWrite(PIN_LED, HIGH);
  delay(20);
  digitalWrite(PIN_LED, LOW);
}

// ---------------------------------------------------------------------------
// Imprime una linea CSV. El orden DEBE coincidir con la cabecera.
// ---------------------------------------------------------------------------
void enviarCSV(unsigned long n, unsigned long ms,
               float temp, float hum, int luz, float dist) {
  Serial.print(n);            Serial.print(',');
  Serial.print(ms);           Serial.print(',');
  Serial.print(temp, 1);      Serial.print(',');
  Serial.print(hum, 1);       Serial.print(',');
  Serial.print(luz);          Serial.print(',');
  Serial.println(dist, 1);    // println cierra la linea con salto
}

// ---------------------------------------------------------------------------
#ifdef USAR_SONAR
float medirDistancia() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, 25000UL);
  if (duracion == 0) return -1;          // sin eco

  return (duracion / 29.1) / 2.0;        // us -> cm, ida y vuelta
}
#endif
