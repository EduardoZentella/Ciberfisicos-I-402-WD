#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <WiFiClientSecure.h>
#include <ezTime.h>
#include "FlowSensor.h"
#include "QualitySensor.h"

// Credenciales del proyecto Firebase
#define FIREBASE_API_KEY <INSERT_API_KEY>
#define FIREBASE_USER_EMAIL <INSERT_USER_EMAIL>
#define FIREBASE_USER_PASSWORD <INSERT_USER_PASSWORD>
#define FIREBASE_DB_URL <INSERT_DB_URL>

// Configuración de la red Wi-Fi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

const char *USER_EMAIL = "test";
const char *SENSOR_ID = "Unidad-1";

// Configuración de EzTime
Timezone myTZ;

// Configuración de Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configF;

bool taskComplete = false;

void inicializarSensores() {
    Serial.println("Inicializando sensores...");
    initFlowSensor();
    // initQualitySensor();
}

void conectarWiFi() {
    Serial.println("Conectando a la red Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        Serial.print(".");
        delay(500);
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("Conectado con IP: ");
        Serial.println(WiFi.localIP());
        Serial.println();
        WiFi.setAutoReconnect(true);
    } else {
        Serial.println("Error: No se pudo conectar a la red Wi-Fi.");
    }
}

void inicializarFirebase() {
    Serial.printf("Cliente Firebase v%s\n", FIREBASE_CLIENT_VERSION);
    Serial.println("Inicializando autenticación...");
    configF.api_key = FIREBASE_API_KEY;
    configF.database_url = FIREBASE_DB_URL;
    auth.user.email = FIREBASE_USER_EMAIL;
    auth.user.password = FIREBASE_USER_PASSWORD;
    configF.token_status_callback = tokenStatusCallback;
    Firebase.begin(&configF, &auth);
    Firebase.reconnectWiFi(true);
}

void sincronizarTiempo() {
    Serial.println("\nInicializando sincronización de tiempo...");
    myTZ.setLocation("America/Monterrey");

    for (int attempts = 0; attempts < 3; attempts++) {
        if (WiFi.status() == WL_CONNECTED && waitForSync(10)) {
            Serial.println("Sincronización exitosa");
            break;
        } else {
            Serial.println("Fallo al sincronizar. Intentando de nuevo...");
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Iniciando configuración...");
    conectarWiFi();
    sincronizarTiempo();
    inicializarFirebase();
    inicializarSensores();
    Serial.println("Configuración completada.");
}

void loop() {
    // Verificar y reconectar Wi-Fi si es necesario
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Conexión Wi-Fi perdida. Intentando reconectar...");
        conectarWiFi();
    }

    if (Firebase.ready() && !taskComplete) {
        unsigned long currentTime = millis();
        measureFlow(currentTime);
        // captureQualityData(currentTime);
    }
    else {
        Serial.println("Esperando a que Firebase esté listo...");
        taskComplete = true;
    }
}
