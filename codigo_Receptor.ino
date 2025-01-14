#include <ArduinoBLE.h>       // Librería de BLE para Arduino
#include <ESP32Firebase.h>    // Librería para interactuar con Firebase usando ESP32
#include <WiFi.h>             // Librería para conectarse a redes WiFi
#include <freertos/FreeRTOS.h> // Librería FreeRTOS para multitarea en el ESP32
#include <freertos/task.h>     // Tareas de FreeRTOS
#include <freertos/queue.h>    // Colas de FreeRTOS para comunicación entre tareas

// Configuración de la conexión WiFi
const char *ssid = "Javeriana";
const char *password = ""; // Asegúrate de poner tu contraseña aquí
const char *firebaseReference = "https://pruebav2-2a797-default-rtdb.firebaseio.com/"; // URL de Firebase

// Instancia de Firebase para enviar/recibir datos
Firebase firebase(firebaseReference);

// Funciones
void taskBLEScan(void *parameter);
void taskSendToFirebase(void *parameter);
void sendValueToFirebase(String assetId);  // Cambiado a String

// Manejadores de tareas para FreeRTOS
TaskHandle_t bleScanTask, sendToFirebaseTask;
QueueHandle_t bufferQueue;  // Cola para almacenar datos temporalmente antes de enviarlos a Firebase

// Definición del tamaño del buffer
const int bufferSize = 3 // se modifica de acuerdo a la necesidad;

void setup() {
  Serial.begin(115200);
  delay(1000);  // Pausa para asegurarse de que el monitor serie esté conectado.

  // Conexión a la red WiFi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a la red Wi-Fi");
  Serial.println(WiFi.localIP());

  // Inicialización del BLE
  Serial.println("Inicializando BLE...");
  if (!BLE.begin()) {
    Serial.println("No se pudo inicializar BLE");
    while (1);  // Si falla, detener el programa
  }
  Serial.println("BLE inicializado correctamente");

  // Crear una cola para almacenar los valores antes de enviarlos a Firebase
  bufferQueue = xQueueCreate(bufferSize, sizeof(String));  // Cambiado a String

  // Crear las tareas de FreeRTOS
  xTaskCreatePinnedToCore(taskBLEScan, "BLEScanTask", 12000, NULL, 1, &bleScanTask, 0);  // Tarea de escaneo BLE
  xTaskCreatePinnedToCore(taskSendToFirebase, "SendToFirebaseTask", 8000, NULL, 1, &sendToFirebaseTask, 1);  // Tarea de envío a Firebase
}

void loop() {
  // El loop está vacío ya que las tareas se manejan por FreeRTOS
}

void taskBLEScan(void *parameter) {
  while (1) {
    // Revisa si se ha encontrado algún dispositivo BLE
    BLEDevice peripheral = BLE.available();

    // Si hay un dispositivo disponible
    if (peripheral) {
      Serial.print("Dispositivo encontrado: ");
      Serial.println(peripheral.address());

      // Captura la información del paquete de publicidad
      int rssi = peripheral.rssi(); // RSSI del dispositivo
      String name = peripheral.localName(); // Nombre del dispositivo
      String advertisedServiceUUID = peripheral.advertisedServiceUuid(); // UUID del servicio publicitado

      // Obtener Manufacturer Data
      uint8_t manufacturerData[24]; // Buffer para almacenar los datos del fabricante
      int dataLength = peripheral.manufacturerData(manufacturerData, sizeof(manufacturerData));
      
      if (dataLength > 0) {
        // Convertir los datos de manufactura a una cadena sin espacios
        String manufacturerDataStr = "";
        for (int i = 0; i < dataLength; i++) {
          manufacturerDataStr += String(manufacturerData[i], HEX);
        }
        manufacturerDataStr.toUpperCase(); // Convertir a mayúsculas
        Serial.print("Manufacturer Data: ");
        Serial.println(manufacturerDataStr);

        // Verificar si comienza con "4C0215"
        if (manufacturerDataStr.startsWith("4C0215")) {
          // Extraer los siguientes 4 caracteres
          String extractedValue = manufacturerDataStr.substring(6, 10);
          Serial.println("Valor extraído para operación matemática: " + extractedValue);

          // Convertir a número entero
          int numericValue = extractedValue.toInt();
          Serial.print("Valor numérico: ");
          Serial.println(numericValue);
          
          // Verificar si la cadena contiene "5678"
          if (manufacturerDataStr.indexOf("5678") != -1) {
            Serial.println("La secuencia '5678' está presente en la cadena.");
            
            // Usar el valor extraído como assetId
            String assetId = extractedValue;  // Aquí asignamos el valor extraído como assetId
            Serial.print("Asset ID a usar: ");
            Serial.println(assetId);

            // Enviar el assetId a Firebase
            xQueueSend(bufferQueue, &assetId, portMAX_DELAY);  // Enviando assetId a la cola para enviarlo a Firebase
          } else {
            Serial.println("La secuencia '5678' no está presente en la cadena.");
          }
        } else {
          Serial.println("No se encontraron datos iBeacon.");
        }
      } else {
        Serial.println("No se encontraron Manufacturer Data");
      }

      // UUID, RSSI
      Serial.print("UUID del servicio publicitado: ");
      Serial.println(advertisedServiceUUID);
      Serial.print("RSSI: ");
      Serial.println(rssi);
    }

    // Reinicia el escaneo
    BLE.scan(); // Reinicia el escaneo sin detenerse
    vTaskDelay(50); // Pausa de 100ms antes del siguiente escaneo
  }
}

// Tarea encargada de enviar datos a Firebase
void taskSendToFirebase(void *parameter) {
  String receivedAssetId;  // Cambiado a String

  while (1) {
    // Espera hasta que haya un valor en la cola
    if (xQueueReceive(bufferQueue, &receivedAssetId, portMAX_DELAY) == pdPASS) {
      // Envía el valor a Firebase
      sendValueToFirebase(receivedAssetId);
    }
  }
}

// Función que envía el valor a Firebase para registrar que el objeto está en la zona actual
void sendValueToFirebase(String assetId) {
  // Cambiar la zona a 1 (representando la nueva zona)
  if (firebase.setString("/Ubicacion/Objetos/" + assetId + "/Ubicacion", "UCI Adulto - Con PED")) {
    Serial.println("Objeto agregado a la zona actual con UID: " + assetId);  // Mensaje de confirmación
  } else {
    Serial.println("Error al actualizar la zona en Firebase");
  }
}
