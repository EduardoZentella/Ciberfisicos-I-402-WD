#include "PIVision.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#define AWS_PythonServer "https://h8p5h3aeyn.us-east-2.awsapprunner.com"

extern const char *USER_EMAIL;
extern const char *SENSOR_ID;
char *USER_WEBID;

void iniciarPiVisionAccount() {
    Serial.println("Iniciando cuenta de PiVision...");
    verificarUsuario(USER_EMAIL);
    verificarPoint(USER_EMAIL, SENSOR_ID);
    verificarUnidad(USER_EMAIL, SENSOR_ID);
}

void verificarUsuario(const char *email) {
    WiFiClientSecure client;
    client.setInsecure(); // Deshabilitar la verificación del certificado

    const char* server = AWS_PythonServer;
    const int port = 8080;

    if (!client.connect(server, port)) {
        Serial.println("Conexión fallida");
        return;
    }

    // Crear el cuerpo de la solicitud GET
    StaticJsonDocument<200> doc;
    String path = "elements?path=\\\\EC2AMAZ-7JHAK8I\\Ciberfisicos-I-402-WD_DB\\Usuarios\\";
    path.concat(email);
    Serial.printf("Verificando usuario en la ruta: %s\n", path.c_str());
    doc["path"] = path;

    String requestBody;
    serializeJson(doc, requestBody);

    // Imprimir el path y el body en la consola
    Serial.printf("Request Body: %s\n", requestBody.c_str());

    // Crear la solicitud GET
    String request = String("GET /pivision HTTP/1.1\r\n") +
                     "Host: " + server + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + requestBody.length() + "\r\n" +
                     "X-HTTP-Method-Override: GET\r\n" +
                     "\r\n" +
                     requestBody;

    client.print(request);

    // Esperar la respuesta del servidor
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            break;
        }
    }

    // Leer el cuerpo de la respuesta
    String response = client.readString();
    Serial.println("Respuesta del servidor:");
    Serial.println(response);

    // Analizar el código de respuesta
    if (response.indexOf("200 OK") != -1) {
        Serial.println("Usuario verificado exitosamente.");
    } else if (response.indexOf("404 Not Found") != -1) {
        Serial.println("Usuario no encontrado, creando usuario...");
        crearUsuario(email);
    } else {
        Serial.println("Error al verificar usuario.");
    }

    client.stop();
}

void crearUsuario(const char *email) {
    HTTPClient http;
    http.begin(AWS_PythonServer);
    http.addHeader("Content-Type", "application/json");

    // Crear el cuerpo de la solicitud POST
    StaticJsonDocument<200> doc;
    doc["path"] = "elements/F1EmHLuiX6VJ50qMPRcAO0J9JQ6LPonkOM7xG62ga88L_JUwRUMyQU1BWi03SkhBSzhJXENJQkVSRklTSUNPUy1JLTQwMi1XRF9EQlxVU1VBUklPUw/elements";
    Serial.printf("Creando usuario con el correo: %s\n", email);
    Serial.printf("Path: %s\n", doc["path"].as<String>().c_str());
    JsonObject body = doc.createNestedObject("body");
    body["Name"] = email;
    body["Description"] = "Usuario del servicio";

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    if (httpCode > 0) {
        Serial.printf("Código de respuesta: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        }
    } else {
        Serial.printf("Error al crear usuario: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void verificarPoint(const char *email, const char *sensor_id) {
    HTTPClient http;
    http.begin(AWS_PythonServer);
    http.addHeader("Content-Type", "application/json");

    // Crear el cuerpo de la solicitud GET
    StaticJsonDocument<200> doc;
    String path = "points?path=";
    path.concat("\\\\\\\\EC2AMAZ-7JHAK8I");
    path.concat("\\\\");
    path.concat(email);
    path.concat("_");
    path.concat(sensor_id);
    path.concat("_Flujo");
    Serial.printf("Verificando punto en la ruta: %s\n", path.c_str());
    doc["path"] = path;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    if (httpCode > 0) {
        Serial.printf("Código de respuesta: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        } else if (httpCode == HTTP_CODE_NOT_FOUND) {
            Serial.println("Punto no encontrado, creando punto...");
            crearPoint(email, sensor_id);
        }
    } else {
        Serial.printf("Error al verificar punto: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void crearPoint(const char *email, const char *sensor_id) {
    HTTPClient http;
    http.begin(AWS_PythonServer);
    http.addHeader("Content-Type", "application/json");

    // Crear el cuerpo de la solicitud POST
    StaticJsonDocument<200> doc;
    doc["path"] = "dataservers/F1DSjp-hXxHDukqWpX1dSyKdTARUMyQU1BWi03SkhBSzhJ/points";
    Serial.printf("Creando punto con el sensor Id: %s\n", sensor_id);
    Serial.printf("Path: %s\n", doc["path"].as<String>().c_str());
    JsonObject body = doc.createNestedObject("body");
    body["Name"] = String(email) + "_" + String(sensor_id) + "_Flujo";
    body["Descriptor"] = "Flujo de un sensor";
    body["PointClass"] = "classic";
    body["PointType"] = "Float32";
    body["EngineeringUnits"] = "";
    body["Step"] = false;
    body["Future"] = false;
    body["DisplayDigits"] = -5;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    if (httpCode > 0) {
        Serial.printf("Código de respuesta: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        }
    } else {
        Serial.printf("Error al crear punto: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void verificarUnidad(const char *email,const char *sensor_id) {
    HTTPClient http;
    http.begin(AWS_PythonServer);
    http.addHeader("Content-Type", "application/json");

    // Crear el cuerpo de la solicitud GET
    StaticJsonDocument<200> doc;
    String path = "elements?path=\\\\\\\\EC2AMAZ-7JHAK8I\\\\Ciberfisicos-I-402-WD_DB\\\\Usuarios\\\\";
    path.concat(email);
    path.concat("\\\\");
    path.concat(sensor_id);
    Serial.printf("Verificando microcontrolador en la ruta: %s\n", path.c_str());
    doc["path"] = path;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    if (httpCode > 0) {
        Serial.printf("Código de respuesta: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        } else if (httpCode == HTTP_CODE_NOT_FOUND) {
            Serial.println("Usuario no encontrado, creando usuario...");
            crearUnidad(sensor_id);
        }
    } else {
        Serial.printf("Error al verificar usuario: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void crearUnidad(const char *sensor_id) {
    HTTPClient http;
    http.begin(AWS_PythonServer);
    http.addHeader("Content-Type", "application/json");

    // Crear el cuerpo de la solicitud POST
    StaticJsonDocument<200> doc;
    doc["path"] = "elements/F1EmHLuiX6VJ50qMPRcAO0J9JQX9ahscGM7xG62ga88L_JUwRUMyQU1BWi03SkhBSzhJXENJQkVSRklTSUNPUy1JLTQwMi1XRF9EQlxVU1VBUklPU1xVU1VBUklPIDE/elements";
    Serial.printf("Creando unidad con el sensor Id: %s\n", sensor_id);
    Serial.printf("Path: %s\n", doc["path"].as<String>().c_str());
    JsonObject body = doc.createNestedObject("body");
    body["Name"] = sensor_id;
    body["Description"] = "Microcontrolador de un usuario";
    body["TemplateName"] = "Microcontrolador";

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    if (httpCode > 0) {
        Serial.printf("Código de respuesta: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        }
    } else {
        Serial.printf("Error al crear usuario: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}