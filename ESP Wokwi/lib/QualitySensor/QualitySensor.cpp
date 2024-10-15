#include <Firebase_ESP_Client.h>
#include <ezTime.h>
#include "QualitySensor.h"
#include <base64.h>
#include <SPIFFS.h>

#define FIREBASE_STORAGE_BUCKET_ID "ciberfisicos-i-402---equipo-5.appspot.com"

const uint8_t dummyImage[] = { 137, 80, 78, 71, 13, 10, 26, 10, 0, 0, 0, 13, 73, 72, 68, 82, 0, 0, 0, 17, 0, 0, 0, 14, 8, 3, 0, 0, 0, 254, 51, 7, 134, 0, 0, 3, 0, 80, 76, 84, 69, 0, 0, 0, 0, 0, 85, 0, 0, 170, 0, 0, 255, 0, 36, 0, 0, 36, 85, 0, 36, 170, 0, 36, 255, 0, 73, 0, 0, 73, 85, 0, 73, 170, 0, 73, 255, 0, 109, 0, 0, 109, 85, 0, 109, 170, 0, 109, 255, 0, 146, 0, 0, 146, 85, 0, 146, 170, 0, 146, 255, 0, 182, 0, 0, 182, 85, 0, 182, 170, 0, 182, 255, 0, 219, 0, 0, 219, 85, 0, 219, 170, 0, 219, 255, 0, 255, 0, 0, 255, 85, 0, 255, 170, 0, 255, 255, 36, 0, 0, 36, 0, 85, 36, 0, 170, 36, 0, 255, 36, 36, 0, 36, 36, 85, 36, 36, 170, 36, 36, 255, 36, 73, 0, 36, 73, 85, 36, 73, 170, 36, 73, 255, 36, 109, 0, 36, 109, 85, 36, 109, 170, 36, 109, 255, 36, 146, 0, 36, 146, 85, 36, 146, 170, 36, 146, 255, 36, 182, 0, 36, 182, 85, 36, 182, 170, 36, 182, 255, 36, 219, 0, 36, 219, 85, 36, 219, 170, 36, 219, 255, 36, 255, 0, 36, 255, 85, 36, 255, 170, 36, 255, 255, 73, 0, 0, 73, 0, 85, 73, 0, 170, 73, 0, 255, 73, 36, 0, 73, 36, 85, 73, 36, 170, 73, 36, 255, 73, 73, 0, 73, 73, 85, 73, 73, 170, 73, 73, 255, 73, 109, 0, 73, 109, 85, 73, 109, 170, 73, 109, 255, 73, 146, 0, 73, 146, 85, 73, 146, 170, 73, 146, 255, 73, 182, 0, 73, 182, 85, 73, 182, 170, 73, 182, 255, 73, 219, 0, 73, 219, 85, 73, 219, 170, 73, 219, 255, 73, 255, 0, 73, 255, 85, 73, 255, 170, 73, 255, 255, 109, 0, 0, 109, 0, 85, 109, 0, 170, 109, 0, 255, 109, 36, 0, 109, 36, 85, 109, 36, 170, 109, 36, 255, 109, 73, 0, 109, 73, 85, 109, 73, 170, 109, 73, 255, 109, 109, 0, 109, 109, 85, 109, 109, 170, 109, 109, 255, 109, 146, 0, 109, 146, 85, 109, 146, 170, 109, 146, 255, 109, 182, 0, 109, 182, 85, 109, 182, 170, 109, 182, 255, 109, 219, 0, 109, 219, 85, 109, 219, 170, 109, 219, 255, 109, 255, 0, 109, 255, 85, 109, 255, 170, 109, 255, 255, 146, 0, 0, 146, 0, 85, 146, 0, 170, 146, 0, 255, 146, 36, 0, 146, 36, 85, 146, 36, 170, 146, 36, 255, 146, 73, 0, 146, 73, 85, 146, 73, 170, 146, 73, 255, 146, 109, 0, 146, 109, 85, 146, 109, 170, 146, 109, 255, 146, 146, 0, 146, 146, 85, 146, 146, 170, 146, 146, 255, 146, 182, 0, 146, 182, 85, 146, 182, 170, 146, 182, 255, 146, 219, 0, 146, 219, 85, 146, 219, 170, 146, 219, 255, 146, 255, 0, 146, 255, 85, 146, 255, 170, 146, 255, 255, 182, 0, 0, 182, 0, 85, 182, 0, 170, 182, 0, 255, 182, 36, 0, 182, 36, 85, 182, 36, 170, 182, 36, 255, 182, 73, 0, 182, 73, 85, 182, 73, 170, 182, 73, 255, 182, 109, 0, 182, 109, 85, 182, 109, 170, 182, 109, 255, 182, 146, 0, 182, 146, 85, 182, 146, 170, 182, 146, 255, 182, 182, 0, 182, 182, 85, 182, 182, 170, 182, 182, 255, 182, 219, 0, 182, 219, 85, 182, 219, 170, 182, 219, 255, 182, 255, 0, 182, 255, 85, 182, 255, 170, 182, 255, 255, 219, 0, 0, 219, 0, 85, 219, 0, 170, 219, 0, 255, 219, 36, 0, 219, 36, 85, 219, 36, 170, 219, 36, 255, 219, 73, 0, 219, 73, 85, 219, 73, 170, 219, 73, 255, 219, 109, 0, 219, 109, 85, 219, 109, 170, 219, 109, 255, 219, 146, 0, 219, 146, 85, 219, 146, 170, 219, 146, 255, 219, 182, 0, 219, 182, 85, 219, 182, 170, 219, 182, 255, 219, 219, 0, 219, 219, 85, 219, 219, 170, 219, 219, 255, 219, 255, 0, 219, 255, 85, 219, 255, 170, 219, 255, 255, 255, 0, 0, 255, 0, 85, 255, 0, 170, 255, 0, 255, 255, 36, 0, 255, 36, 85, 255, 36, 170, 255, 36, 255, 255, 73, 0, 255, 73, 85, 255, 73, 170, 255, 73, 255, 255, 109, 0, 255, 109, 85, 255, 109, 170, 255, 109, 255, 255, 146, 0, 255, 146, 85, 255, 146, 170, 255, 146, 255, 255, 182, 0, 255, 182, 85, 255, 182, 170, 255, 182, 255, 255, 219, 0, 255, 219, 85, 255, 219, 170, 255, 219, 255, 255, 255, 0, 255, 255, 85, 255, 255, 170, 255, 255, 255, 234, 155, 110, 46, 0, 0, 0, 212, 73, 68, 65, 84, 120, 218, 29, 79, 49, 110, 2, 65, 12, 28, 239, 121, 165, 20, 145, 130, 64, 40, 69, 34, 157, 17, 135, 68, 65, 25, 94, 64, 21, 69, 202, 11, 242, 143, 124, 33, 93, 170, 188, 132, 130, 130, 124, 33, 125, 10, 16, 94, 184, 171, 67, 207, 158, 140, 55, 141, 61, 150, 199, 51, 99, 250, 0, 32, 0, 103, 239, 188, 55, 66, 64, 129, 232, 74, 113, 152, 112, 96, 196, 218, 113, 253, 216, 58, 17, 169, 54, 9, 217, 180, 220, 176, 168, 42, 132, 136, 184, 237, 87, 96, 1, 99, 102, 192, 84, 133, 3, 85, 0, 1, 61, 172, 72, 87, 39, 171, 198, 183, 199, 252, 228, 144, 154, 225, 104, 116, 247, 55, 232, 67, 221, 161, 251, 102, 230, 155, 24, 117, 251, 110, 196, 140, 135, 28, 19, 36, 186, 247, 122, 247, 245, 123, 73, 193, 85, 204, 38, 169, 196, 217, 185, 171, 74, 160, 150, 129, 237, 212, 135, 141, 54, 93, 51, 65, 181, 28, 20, 214, 121, 249, 114, 255, 250, 182, 16, 30, 255, 4, 75, 228, 230, 249, 179, 121, 6, 137, 239, 252, 74, 60, 137, 169, 206, 255, 63, 179, 136, 43, 132, 12, 71, 151, 64, 236, 168, 101, 0, 0, 0, 0, 73, 69, 78, 68, 174, 66, 96, 130 };
const size_t dummyImage_size = sizeof(dummyImage);

// Declarar myTZ como externa
extern Timezone myTZ;

// Variables para el temporizador de captura de calidad
unsigned long lastQualityCaptureTime = 0;
const unsigned long qualityCaptureInterval = 1 * 60 * 1000; // 15 minutos en milisegundos

void initQualitySensor() {
  // Inicialización de cualquier hardware necesario para la calidad del agua
}

void fcsUploadCallback(FCS_UploadStatusInfo info) {
    Serial.printf("Upload status: %d, message: %s\n", info.status, info.errorMsg.c_str());
}

bool uploadImageToStorage(const String &storagePath) {
    Serial.println("Subiendo imagen a Firebase Storage...");

    // Asegúrar que la ruta no termine con '/' ni contenga dos '/' consecutivos
    String adjustedPath = storagePath;
    if (adjustedPath.endsWith("/")) {
        adjustedPath.remove(adjustedPath.length() - 1);
    }
    adjustedPath.replace("//", "/");

    // Subir la imagen a Firebase Storage directamente desde el buffer en memoria
    if (Firebase.Storage.upload(&fbdo, FIREBASE_STORAGE_BUCKET_ID, dummyImage, dummyImage_size, storagePath.c_str(), "image/jpeg", fcsUploadCallback)) {
        Serial.printf("\nDownload URL: %s\n", fbdo.downloadURL().c_str());
        return true;
    } else {
        Serial.println("Error al subir la imagen a Firebase Storage");
        Serial.println(fbdo.errorReason());
        return false;
    }
}


bool saveImageLinkToDatabase(const String &path, const String &storagePath) {
  if (WiFi.status() == WL_CONNECTED) {
      // Crear un JSON para la imagen
      FirebaseJson jsonImagen;
      jsonImagen.set("imagen", storagePath);

      if (Firebase.ready() && auth.token.uid.length() > 0) {
        // Guardar los datos en Firebase
        Serial.println("Guardando enlace de la imagen en Firebase...");
        if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &jsonImagen)) {
          Serial.println("Enlace de la imagen guardado correctamente en Firebase");
          return true;
        } else {
          String errorMsg = "Error al guardar el enlace de la imagen en Firebase: ";
          errorMsg += fbdo.errorReason();
          Serial.println(errorMsg);
          return false;
        }
      } else {
        Serial.println("Firebase no está listo o el token no está listo");
        return false;
      }
      Serial.println("Error: No se pudo obtener la fecha y hora");
      return false;
    } else {
    Serial.println("Error: No hay conexión Wi-Fi");
    return false;
  }
}

bool isWaitingForCapture = false;

void captureQualityData(unsigned long currentTime) {
  // Verificar si ha pasado el intervalo de captura
  if (currentTime - lastQualityCaptureTime >= qualityCaptureInterval) {
    lastQualityCaptureTime = currentTime;

    // Obtener la fecha y hora actual usando ezTime
    String fecha = myTZ.dateTime("Y-m-d");
    String hora = myTZ.dateTime("H");
    String min = myTZ.dateTime("i");

    // Simular la captura de una imagen JPEG
    String imagePath = "usuarios/usario/";
    imagePath.concat(USER_EMAIL);
    imagePath.concat("/images/");
    imagePath.concat(fecha);
    imagePath.concat("/");
    imagePath.concat(hora);
    imagePath.concat("/");
    imagePath.concat(min);
    imagePath.concat(".jpg");

    // Crear la ruta en la base de datos
    // Crear la ruta base en la base de datos
      String path = "/usuarios/usuario/";
      path.concat(USER_EMAIL);
      path.concat("/unidades/");
      path.concat(SENSOR_ID);
      path.concat("/sensores/flujo/fechas/");
      path.concat(fecha);
      path.concat("/horas/");
      path.concat(hora);
      path.concat("/minutos/");
      path.concat(min);

    // Subir la imagen a Firebase Storage
    if (uploadImageToStorage(imagePath)) {
      // Generar el URL completo para almacenar en RTDB
      String storageURL = "gs://";
      storageURL.concat(FIREBASE_STORAGE_BUCKET_ID);
      storageURL.concat(imagePath);
      saveImageLinkToDatabase(path, storageURL);
    }

    // Resetear el estado de espera
    isWaitingForCapture = false;
  } else {
    // Enviar mensaje inicial una vez por ciclo
    if (!isWaitingForCapture) {
      Serial.println("Proceso iniciado, esperando el tiempo de captura...");
      isWaitingForCapture = true;
    }
  }
}

