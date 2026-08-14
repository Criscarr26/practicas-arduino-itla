/*
  ============================================================================
  TAREA 5 - Lectura de los datos sensados de Arduino desde Python
               VERSION VIRTUAL (parte 1 de 2: el firmware)
  ============================================================================
  Asignatura : Inteligencia Artificial e Internet de las Cosas (2026-C-2)
  Profesor   : Luis Bessewell Feliz
  Estudiante : Cristian Carrera - Matricula 2024-1932
  Institucion: Instituto Tecnologico de Las Americas (ITLA)
  Placa      : ESP32 DevKit (simulador Wokwi)
  Valor      : 8 puntos

  La parte 2 es el receptor en Python: receptor_mqtt.py

  ============================================================================
  POR QUE ESTA VERSION NO USA EL PUERTO SERIE
  ============================================================================
  La forma clasica de leer un Arduino desde Python es la libreria pyserial
  sobre el puerto USB. Aqui NO se puede, y conviene explicar por que en vez
  de disimularlo:

    1. El simulador corre DENTRO del navegador. No crea ningun puerto COM en
       el sistema operativo, asi que no existe nada a lo que pyserial pueda
       conectarse. No es una limitacion del codigo: es que el "cable USB" no
       existe.

    2. El Arduino UNO no tiene radio. En el simulador tampoco tiene salida a
       la red, asi que ni siquiera podria hablar por otro medio.

  LA ALTERNATIVA ELEGIDA, Y POR QUE ES VALIDA:
  Se cambia la placa a un ESP32, que si tiene WiFi, y el transporte pasa a
  ser MQTT sobre un broker publico. El simulador de Wokwi SI da salida real a
  internet a las placas con WiFi, de modo que el flujo es autentico:

        [ESP32 en el navegador] --WiFi--> [broker MQTT] --> [Python en la PC]

  El objetivo academico se mantiene intacto: los datos que sensa Arduino se
  leen y procesan desde otro lenguaje. De hecho la arquitectura es MAS
  parecida a un sistema IoT real que la del cable serie, porque no exige que
  las dos maquinas esten fisicamente conectadas.

  ============================================================================
  QUE SENSA
  ============================================================================
  Tres magnitudes, con los mismos sensores de las practicas anteriores:
     - Temperatura y humedad ... DHT22
     - Luz ambiente ............ fotorresistencia (LDR)
     - Distancia ............... HC-SR04

  ============================================================================
  FORMATO DE LOS DATOS: JSON
  ============================================================================
  En la version por cable se uso CSV, porque a 9600 baudios cada byte cuenta.
  Aqui se usa JSON, y el cambio esta justificado:

     - MQTT no tiene la limitacion de ancho de banda del puerto serie.
     - JSON es AUTODESCRIPTIVO: cada valor viaja con su nombre, asi que el
       receptor no depende del ORDEN de los campos. Si mañana se agrega un
       sensor, el codigo de Python no se rompe.
     - Es el formato estandar de la industria IoT.

  Ejemplo de mensaje publicado:
     {"nodo":"esp32-2024-1932","seq":12,"ms":24310,
      "temperatura":24.5,"humedad":58.0,"luz":812,"distancia":45.3}

  ============================================================================
  MONTAJE EN WOKWI
  ============================================================================
     DHT22:   DATA -> GPIO 15,  VCC -> 3V3,  GND -> GND
     LDR:     AO   -> GPIO 34   (entrada analogica, solo lectura)
     HC-SR04: TRIG -> GPIO 5,   ECHO -> GPIO 18
     LED integrado del GPIO 2: parpadea al publicar cada lectura

  ============================================================================
  LIBRERIAS QUE WOKWI DEBE DESCARGAR
  ============================================================================
     DHT sensor library
     Adafruit Unified Sensor
     PubSubClient
  ============================================================================
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ---------------------------------------------------------------------------
// Red. En el simulador la WiFi virtual es siempre esta y no hay que tocarla.
// ---------------------------------------------------------------------------
const char *WIFI_SSID = "Wokwi-GUEST";
const char *WIFI_PASS = "";
const int   WIFI_CANAL = 6;

const char *MQTT_HOST = "test.mosquitto.org";
const int   MQTT_PORT = 1883;
const char *CLIENTE_ID = "esp32-2024-1932";

// El topico lleva la matricula para que nadie mas publique en el mismo sitio.
// test.mosquitto.org es PUBLICO: cualquiera puede leerlo. Sirve para la
// practica; para datos reales haria falta un broker propio con TLS.
const char *TOPICO = "itla/2024-1932/practica5/sensores";

// ---------------------------------------------------------------------------
// Pines
// ---------------------------------------------------------------------------
#define PIN_DHT   15
#define TIPO_DHT  DHT22
const int PIN_LDR  = 34;
const int PIN_TRIG = 5;
const int PIN_ECHO = 18;
const int PIN_LED  = 2;

DHT dht(PIN_DHT, TIPO_DHT);
WiFiClient   red;
PubSubClient mqtt(red);

const unsigned long PERIODO_ENVIO = 2000;   // el DHT no admite mas rapido
unsigned long tEnvio = 0;
unsigned long secuencia = 0;

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(PIN_LED,  OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  dht.begin();

  Serial.println(F("\n== Tarea 5: Arduino -> MQTT -> Python =="));
  Serial.println(F("Cristian Carrera - 2024-1932 - ITLA"));

  Serial.print(F("Conectando a WiFi"));
  WiFi.begin(WIFI_SSID, WIFI_PASS, WIFI_CANAL);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print('.');
  }
  Serial.print(F("\nConectado. IP: "));
  Serial.println(WiFi.localIP());

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  Serial.print(F("Publicando en el topico: "));
  Serial.println(TOPICO);
}

// ---------------------------------------------------------------------------
void loop() {
  // Reconexion perezosa: solo se intenta cuando hace falta, y sin bloquear
  // mas de lo necesario.
  if (!mqtt.connected()) {
    Serial.print(F("Conectando al broker... "));
    if (mqtt.connect(CLIENTE_ID)) {
      Serial.println(F("listo"));
    } else {
      Serial.print(F("fallo, rc="));
      Serial.println(mqtt.state());
      delay(2000);
      return;
    }
  }
  mqtt.loop();

  unsigned long ahora = millis();
  if (ahora - tEnvio < PERIODO_ENVIO) return;
  tEnvio = ahora;

  // --- Lectura de los tres sensores -------------------------------------
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  // isnan detecta lectura fallida. Se envia -1 para que el receptor sepa que
  // el dato NO es valido, en vez de mandarle un cero enganoso: cero grados
  // es una temperatura perfectamente real.
  if (isnan(t)) t = -1;
  if (isnan(h)) h = -1;

  int   luz = analogRead(PIN_LDR);      // ADC de 12 bits: 0 a 4095
  float d   = medirDistancia();

  // --- Armado del JSON --------------------------------------------------
  secuencia++;
  char carga[256];
  snprintf(carga, sizeof(carga),
           "{\"nodo\":\"%s\",\"seq\":%lu,\"ms\":%lu,"
           "\"temperatura\":%.1f,\"humedad\":%.1f,"
           "\"luz\":%d,\"distancia\":%.1f}",
           CLIENTE_ID, secuencia, ahora, t, h, luz, d);

  // --- Publicacion ------------------------------------------------------
  bool ok = mqtt.publish(TOPICO, carga);
  Serial.printf("[%s] %s\n", ok ? "ENVIADO" : "FALLO  ", carga);

  // Parpadeo corto: confirma visualmente que se publico.
  digitalWrite(PIN_LED, HIGH);
  delay(30);
  digitalWrite(PIN_LED, LOW);
}

// ---------------------------------------------------------------------------
// HC-SR04: pulso de 10 us por TRIG y se mide cuanto dura ECHO en alto.
// La division entre 2 es porque el tiempo medido es de IDA Y VUELTA.
// ---------------------------------------------------------------------------
float medirDistancia() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, 25000UL);
  if (duracion == 0) return -1;        // sin eco
  return (duracion / 29.1) / 2.0;
}
