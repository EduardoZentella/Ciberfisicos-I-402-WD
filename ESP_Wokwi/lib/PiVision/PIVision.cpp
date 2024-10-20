#include "PIVision.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define AWS_PythonServer "https://h8p5h3aeyn.us-east-2.awsapprunner.com/pivision"

extern const char *USER_EMAIL;
extern const char *SENSOR_ID;
char *USER_WEBID;

String user_webIdStr;
String point_webIdStr;

String obtenerWebId(const char* payload);

void iniciarPiVisionAccount() {
    Serial.println("Iniciando cuenta de PiVision...");
    verificarUsuario(USER_EMAIL);
    verificarPoint(USER_EMAIL, SENSOR_ID);
    verificarUnidad(USER_EMAIL, SENSOR_ID);
}

void verificarUsuario(const char *email) {
    HTTPClient http;
    String path = "?RootPath=elements?path=\\\\EC2AMAZ-7JHAK8I\\Ciberfisicos-I-402-WD_DB\\Usuarios\\";
    path.concat(email);
    path.replace("\\", "\\\\");
    String url = String(AWS_PythonServer) + path;
    Serial.printf("Verificando usuario en la ruta: %s\n", url.c_str());
    http.begin(url);

    int httpCode = http.GET();    
    if (httpCode > 0) {
        Serial.printf("Código de respuesta: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            // Parsear el JSON y obtener el WebId
            user_webIdStr = obtenerWebId(payload.c_str());
            Serial.println(user_webIdStr);
        } else if (httpCode == HTTP_CODE_NOT_FOUND) {
            Serial.println("Usuario no encontrado, creando usuario...");
            crearUsuario(email);
        } else if (httpCode == HTTP_CODE_BAD_REQUEST) {
            Serial.println("Solicitud incorrecta");
            Serial.println("Payload: " + http.getString());
        }
        else if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR) {
            Serial.println("Error interno del servidor");
            Serial.println("Payload: " + http.getString());
        }
    } else {
        Serial.printf("Error al verificar usuario: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
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
            verificarUsuario(email);
        }
    } else {
        Serial.printf("Error al crear usuario: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void verificarPoint(const char *email, const char *sensor_id) {
    HTTPClient http;
    // Crear el cuerpo de la solicitud GET
    String path = "?RootPath=points?path=";
    path.concat("\\\\EC2AMAZ-7JHAK8I");
    path.concat("\\");
    path.concat(email);
    path.concat("_");
    path.concat(sensor_id);
    path.concat("_Flujo");
    path.replace("\\", "\\\\");
    String url = String(AWS_PythonServer) + path;
    Serial.printf("Verificando punto en la ruta: %s\n", url.c_str());
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();
    if (httpCode > 0) {
        Serial.printf("Código de respuesta: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            // Parsear el JSON y obtener el WebId
            point_webIdStr = obtenerWebId(payload.c_str());
            Serial.println(point_webIdStr);
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
    Serial.print("Creando punto con el id: " + String(email) + "_" + String(sensor_id) + "_Flujo");
    Serial.printf("Path: %s\n", doc["path"].as<String>().c_str());
    JsonObject body = doc.createNestedObject("body");
    body["Name"] = String(email) + "_" + String(sensor_id) + "_Flujo";
    body["Descriptor"] = "Flujo de un sensor";
    body["PointClass"] = "classic";
    body["PointType"] = "Float32";
    body["Future"] = false;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    if (httpCode > 0) {
        Serial.printf("Código de respuesta: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            verificarPoint(email, sensor_id);
        }
    } else {
        Serial.printf("Error al crear punto: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void verificarUnidad(const char *email,const char *sensor_id) {
    HTTPClient http;
    // Crear el cuerpo de la solicitud GET
    String path = "?RootPath=elements?path=\\\\EC2AMAZ-7JHAK8I\\Ciberfisicos-I-402-WD_DB\\Usuarios\\";
    path.concat(email);
    path.concat("\\");
    path.concat(sensor_id);
    path.replace("\\", "\\\\");
    String url = String(AWS_PythonServer) + path;
    Serial.printf("Verificando microcontrolador en la ruta: %s\n", url.c_str());
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
        Serial.printf("Código de respuesta: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
        } else if (httpCode == HTTP_CODE_NOT_FOUND) {
            Serial.println("Unidad no encontrada, creando unidad...");
            crearUnidad(sensor_id);
        }
    } else {
        Serial.printf("Error al verificar Unidad: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void crearUnidad(const char *sensor_id) {
    HTTPClient http;
    http.begin(AWS_PythonServer);
    http.addHeader("Content-Type", "application/json");

    // Crear el cuerpo de la solicitud POST
    StaticJsonDocument<200> doc;
    doc["path"] = "elements/"+ user_webIdStr +"/elements";
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
            verificarUnidad(USER_EMAIL, sensor_id);
        }
    } else {
        Serial.printf("Error al crear Unidad: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}


String obtenerWebId(const char* payload) {
    const char* webId = nullptr;
    const char* key = "\"WebId\":\"";
    char* pos = strstr(payload, key);
    if (pos) {
        pos += strlen(key);
        char* end = strchr(pos, '"');
        if (end) {
            size_t len = end - pos;
            char* buffer = new char[len + 1];
            strncpy(buffer, pos, len);
            buffer[len] = '\0';
            webId = buffer;
            String webIdStr = String(webId);
            delete[] buffer;
            return webIdStr;
        }
    }
    Serial.println("Error: No se encontró el WebId en el JSON.");
    return String();
}