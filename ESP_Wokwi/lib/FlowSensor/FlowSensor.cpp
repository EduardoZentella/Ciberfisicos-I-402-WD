#include <LiquidCrystal_I2C.h>
#include <Firebase_ESP_Client.h>
#include "FlowSensor.h"
#include <ezTime.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define AWS_PythonServer <TU_SERVIDOR_PYTHON>

// Declarar myTZ como externa
extern Timezone myTZ;

// Declarar http
HTTPClient http;

// Inicializamos la funcion PulseGen
void pulseGen(int pulseTime);

// Inicializa la pantalla LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile int flow_frequency; // Mide los pulsos del sensor de flujo
float vol = 0.0, l_minute;
unsigned long cloopTime;
unsigned long currentTime;
unsigned long previousTime = 0;
// Pines del sensor flujo
unsigned int flowsensor = 2; // Pin de entrada del sensor de flujo
unsigned int pulsePin = 4;
// Pines de los LEDs
unsigned int LedVerde = 9;
unsigned int LedRojo = 10;

// Función de interrupción para contar los pulsos del sensor de flujo
void IRAM_ATTR flow() {
  flow_frequency++;
}

void initFlowSensor() {
  pinMode(pulsePin, OUTPUT);
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH); // Pull-Up interno opcional
  pinMode(LedVerde, OUTPUT);
  pinMode(LedRojo, OUTPUT);

  // Inicializa la pantalla LCD
  lcd.init(21, 22);
  lcd.backlight();
  attachInterrupt(digitalPinToInterrupt(flowsensor), flow, RISING); // Configura la interrupción

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water Flow Meter");
  lcd.setCursor(0, 1);
  lcd.print("Circuit Digest");
  cloopTime = millis();
  lcd.clear();
}

void calcularFlujo();
void mostrarEnLCD();
void guardarEnFirebase();
void guardarEnPiVision();
void mostrarFlujoCero();

void measureFlow(unsigned long currentTime) {
  int randNum = random(10, 20);
  pulseGen(randNum);

  // Cada segundo, calcula e imprime litros/hora
  if (currentTime >= (cloopTime + 1000)) {
    cloopTime = currentTime; // Actualiza cloopTime

    if (flow_frequency != 0) {
      calcularFlujo();
      mostrarEnLCD();
      guardarEnFirebase();
      guardarEnPiVision();
    } else {
      mostrarFlujoCero();
    }
  }
}

void calcularFlujo() {
  // Frecuencia de pulsos (Hz) = 7.5Q, Q es la tasa de flujo en L/min.
  l_minute = (flow_frequency / 7.5); // (Frecuencia de pulsos x 60 min) / 7.5Q = tasa de flujo en L/hora
  l_minute = l_minute / 60; // Convertir a L/seg
  vol += l_minute; // Actualizar el volumen total
  flow_frequency = 0; // Reinicia el contador
  Serial.print(l_minute, DEC); // Imprime litros/seg
  Serial.println(" L/Sec");
}

void mostrarEnLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Rate: ");
  lcd.print(l_minute);
  lcd.print(" L/M");
  lcd.setCursor(0, 1);
  lcd.print("Vol:");
  lcd.print(vol);
  lcd.print(" L");
}

void guardarEnFirebase() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nObteniendo la fecha y hora actual...");
    if (timeStatus() == timeSet) {
      String fecha = myTZ.dateTime("Y-m-d");
      String hora = myTZ.dateTime("H");
      String min = myTZ.dateTime("i");
      String seg = myTZ.dateTime("s");
      Serial.println("Fecha y hora obtenidas correctamente");

      // Crear la ruta base en la base de datos
      String basePath = "/usuarios/usuario/";
      basePath.concat(USER_EMAIL);
      basePath.concat("/unidades/");
      basePath.concat(SENSOR_ID);
      basePath.concat("/sensores/flujo/fechas/");
      basePath.concat(fecha);
      basePath.concat("/horas/");
      basePath.concat(hora);
      basePath.concat("/minutos/");
      basePath.concat(min);
      basePath.concat("/segundos/");
      basePath.concat(seg);


      // Crear un JSON para el flujo
      FirebaseJson jsonFlujo;
      jsonFlujo.set("Flujo", l_minute);

      if (Firebase.ready() && auth.token.uid.length() > 0) {
        // Guardar los datos en Firebase
        Serial.println("Guardando datos en Firebase...");
        if (Firebase.RTDB.setJSON(&fbdo, basePath.c_str(), &jsonFlujo)) {
          Serial.println("Datos guardados correctamente en Firebase");
          digitalWrite(LedVerde, HIGH);
        } else {
          String errorMsg = "Error al guardar datos en Firebase: ";
          digitalWrite(LedRojo, HIGH);
          errorMsg += fbdo.errorReason();
          Serial.println(errorMsg);
        }
      } else {
        Serial.println("Firebase no está listo o el token no está listo");
      }
    } else {
      Serial.println("Error: No se pudo obtener la fecha y hora");
    }
  } else {
    Serial.println("Error: No hay conexión Wi-Fi");
  }
}

void mostrarFlujoCero() {
  Serial.println(" flow rate = 0 ");
  lcd.setCursor(0, 0);
  lcd.print("Rate: ");
  lcd.print(flow_frequency);
  lcd.print(" L/M");
  lcd.setCursor(0, 1);
  lcd.print("Vol:");
  lcd.print(vol);
  lcd.print(" L");
}

void pulseGen(int pulseTime) {
  currentTime = millis();
  if (currentTime - previousTime >= pulseTime) {
    digitalWrite(pulsePin, HIGH);
    delay(pulseTime);
    digitalWrite(pulsePin, LOW);
    delay(pulseTime);
    previousTime = currentTime;
  }
}


void guardarEnPiVision(){
  http.begin(AWS_PythonServer);
  http.addHeader("Content-Type", "application/json");
  // Crear el cuerpo de la solicitud POST
    StaticJsonDocument<200> doc;
    String path = "streams/";
    path.concat(point_webIdStr);
    path.concat("/value");
    doc["path"] = path;
    Serial.printf("Subiendo en punto en la ruta: %s\n", path.c_str());
    JsonObject body = doc.createNestedObject("body");
    body["Value"] = l_minute;

    String requestBody;
    serializeJson(doc, requestBody);

  int httpCode = http.POST(requestBody);
  if (httpCode > 0) {
    Serial.printf("Código de respuesta: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
    }
  } else {
    Serial.printf("Error al subir punto: %s\n", http.errorToString(httpCode).c_str());
  }
}