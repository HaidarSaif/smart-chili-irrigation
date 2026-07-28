#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <Firebase_ESP_Client.h>
#include <EEPROM.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// Insert Firebase project API Key
#define API_KEY "API_KEY"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "URL"

// EEPROM addresses for storing minMoist values
#define EEPROM_SIZE 12  // We will store 3 integers, each 4 bytes (int = 4 bytes)
#define EEPROM_ADDR_SENSOR1 0
#define EEPROM_ADDR_SENSOR2 4
#define EEPROM_ADDR_SENSOR3 8

FirebaseData fbdo;
FirebaseData streamData;
FirebaseAuth auth;
FirebaseConfig config;

const char* ssid = "KWT_Mawar_I";
const char* password = "123456789";

const int relayPin1 = 25, relayPin2 = 26, relayPin3 = 27, relayPin4 = 14, moistPin1 = 36;

String listenerPath = "A23DR562GHHG6DSG/relay";

float moist1, moist2, moist3, moist4;
int auto1, auto2, auto3, auto4, relay1, relay2, relay3, relay4, counterError;
int setMinMoist1, setMinMoist2, setMinMoist3;
String cp1, cp2, cp3, cp4;

bool signupOK = false;

unsigned long previousMillis1, previousMillis2;

TaskHandle_t Task2;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int count;
  while (WiFi.status() != WL_CONNECTED) {
    count++;
    delay(500);
    Serial.print(".");
    if (count >= 60) ESP.restart();
  }

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  fbdo.setBSSLBufferSize(4096, 4096);

  fbdo.setResponseSize(4096);

  if (!Firebase.RTDB.beginStream(&streamData, listenerPath.c_str()))
    Serial.printf("stream begin error, %s\n\n", streamData.errorReason().c_str());

  Firebase.RTDB.setStreamCallback(&streamData, streamCallback, streamTimeoutCallback);

  pinMode(moistPin1, INPUT);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);

  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);
  digitalWrite(relayPin4, HIGH);

  // Load min moisture values from EEPROM if no internet
  setMinMoist1 = readEEPROM(EEPROM_ADDR_SENSOR1);
  setMinMoist2 = readEEPROM(EEPROM_ADDR_SENSOR2);
  setMinMoist3 = readEEPROM(EEPROM_ADDR_SENSOR3);

  xTaskCreatePinnedToCore(
    Task2code,  // Function to implement the task
    "Task2",    // Name of the task
    10000,      // Stack size in words
    NULL,       // Task input parameter
    1,          // Priority of the task
    &Task2,     // Task handle
    1);         // Core where the task should run
}

void sendPacket() {
  String node1 = "/sensor1";
  String node2 = "/sensor2";
  String node3 = "/sensor3";
  String node4 = "/sensor4";

  FirebaseJson dataSensor;

  dataSensor.set(node1, moist1);
  dataSensor.set(node2, moist2);
  dataSensor.set(node3, moist3);
  dataSensor.set(node4, moist4);

  if (Firebase.RTDB.set(&fbdo, "/A23DR562GHHG6DSG/sensorData", &dataSensor)) {
    Serial.println("SUCCESS KIRIM DATA");
  } else {
    Serial.println("Failed to send data");
    Serial.println("Reason: " + fbdo.errorReason());
    restartESP();
  }
}

void recvPacket() {
  String node6 = "/relay/A1";
  String node7 = "/relay/A2";
  String node8 = "/relay/A3";
  String node9 = "/relay/A4";
  String node10 = "/relay/R1";
  String node11 = "/relay/R2";
  String node12 = "/relay/R3";
  String node13 = "/relay/R4";
  String nodeMinMoist1 = "/setMinMoist/sens1";
  String nodeMinMoist2 = "/setMinMoist/sens2";
  String nodeMinMoist3 = "/setMinMoist/sens3";

  FirebaseJson command;

  if (Firebase.RTDB.getJSON(&fbdo, "/A23DR562GHHG6DSG", &command)) {
    FirebaseJsonData datarecieved;
    command.get(datarecieved, node6);
    if (datarecieved.success) {
      auto1 = datarecieved.to<int>();
      Serial.print("auto1 ");
      Serial.println(auto1);
    }
    command.get(datarecieved, node7);
    if (datarecieved.success) {
      auto2 = datarecieved.to<int>();
      Serial.print("auto2 ");
      Serial.println(auto2);
    }
    command.get(datarecieved, node8);
    if (datarecieved.success) {
      auto3 = datarecieved.to<int>();
      Serial.print("auto3 ");
      Serial.println(auto3);
    }
    command.get(datarecieved, node9);
    if (datarecieved.success) {
      auto4 = datarecieved.to<int>();
      Serial.print("auto4 ");
      Serial.println(auto4);
    }
    command.get(datarecieved, node10);
    if (datarecieved.success) {
      relay1 = datarecieved.to<int>();
      Serial.print("relay1 ");
      Serial.println(relay1);
    }
    command.get(datarecieved, node11);
    if (datarecieved.success) {
      relay2 = datarecieved.to<int>();
      Serial.print("relay2 ");
      Serial.println(relay2);
    }
    command.get(datarecieved, node12);
    if (datarecieved.success) {
      relay3 = datarecieved.to<int>();
      Serial.print("relay3 ");
      Serial.println(relay3);
    }
    command.get(datarecieved, node13);
    if (datarecieved.success) {
      relay4 = datarecieved.to<int>();
      Serial.print("relay4 ");
      Serial.println(relay4);
    }
    // Get min moisture for sensor 1
    command.get(datarecieved, nodeMinMoist1);
    if (datarecieved.success) {
      String dataSet1 = datarecieved.to<String>();
      dataSet1 = dataSet1.substring(2, dataSet1.length() - 2);
      setMinMoist1 = dataSet1.toInt();
      Serial.print("setMinMoist1: ");
      Serial.println(setMinMoist1);
      // Save to EEPROM
      writeEEPROM(EEPROM_ADDR_SENSOR1, setMinMoist1);
    }

    // Get min moisture for sensor 2
    command.get(datarecieved, nodeMinMoist2);
    if (datarecieved.success) {
      String dataSet2 = datarecieved.to<String>();
      dataSet2 = dataSet2.substring(2, dataSet2.length() - 2);
      setMinMoist2 = dataSet2.toInt();
      Serial.print("setMinMoist2: ");
      Serial.println(setMinMoist2);
      // Save to EEPROM
      writeEEPROM(EEPROM_ADDR_SENSOR2, setMinMoist2);
    }

    // Get min moisture for sensor 3
    command.get(datarecieved, nodeMinMoist3);
    if (datarecieved.success) {
      String dataSet3 = datarecieved.to<String>();
      dataSet3 = dataSet3.substring(2, dataSet3.length() - 2);
      setMinMoist3 = dataSet3.toInt();
      Serial.print("setMinMoist3: ");
      Serial.println(setMinMoist3);
      // Save to EEPROM
      writeEEPROM(EEPROM_ADDR_SENSOR3, setMinMoist3);
    }
    command.clear();
  }
}

void streamCallback(FirebaseStream data) {
  Serial.println();

  String streamPath = String(data.dataPath());

  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_string) {
    if (Firebase.RTDB.set(&fbdo, "/A23DR562GHHG6DSG/state/status", "1")) {
      Serial.println("SUCCESS KIRIM DATA");
    } else {
      Serial.println("Failed to send data");
      Serial.println("Reason: " + fbdo.errorReason());
      restartESP();
    }
  }
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n", data.payloadLength(), data.maxPayloadLength());
}

void streamTimeoutCallback(bool timeout) {
  if (timeout)
    Serial.println("stream timeout, resuming...\n");
  if (!streamData.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", streamData.httpCode(), streamData.errorReason().c_str());
}

void Task2code(void* pvParameters) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    while (WiFi.status() == WL_CONNECTED) {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis2 > 700) {
        recvPacket();
        previousMillis2 = currentMillis;
      }
      if (currentMillis - previousMillis1 > 30000) {
        previousMillis1 = currentMillis;
        sendPacket();
      }
    }
  }
}

// Function to read from EEPROM
int readEEPROM(int addr) {
  int value = 0;
  for (int i = 0; i < 4; i++) {
    value |= EEPROM.read(addr + i) << (8 * i);
  }
  return value;
}

// Function to write to EEPROM
void writeEEPROM(int addr, int value) {
  for (int i = 0; i < 4; i++) {
    EEPROM.write(addr + i, (value >> (8 * i)) & 0xFF);
  }
  EEPROM.commit();  // Commit changes to EEPROM
}

void loop() {
  if (Serial2.available()) {
    // Baca data dari Serial2
    String data = Serial2.readStringUntil('\n');  // Membaca hingga karakter newline

    // Kirim data ke Serial Monitor
    Serial.print("Data diterima: ");
    Serial.println(data);

    // Memisahkan data berdasarkan delimiter koma
    int index1 = data.indexOf(',');
    int index2 = data.indexOf(',', index1 + 1);

    if (index1 > 0 && index2 > index1) {
      // Ambil nilai kelembaban dan konversi menjadi float
      moist1 = data.substring(0, index1).toFloat();
      moist3 = data.substring(index1 + 1, index2).toFloat();
      moist2 = data.substring(index2 + 1).toFloat();

      // Tampilkan nilai kelembaban
      Serial.print("Moisture 1: ");
      Serial.println(moist1);
      Serial.print("Moisture 2: ");
      Serial.println(moist2);
      Serial.print("Moisture 3: ");
      Serial.println(moist3);
    }
  }
  if (!auto1)
    digitalWrite(relayPin1, !relay1);
  else {
    if (moist1 < setMinMoist1)
      digitalWrite(relayPin1, 0);
    else
      digitalWrite(relayPin1, 1);
  }
  if (!auto2)
    digitalWrite(relayPin2, !relay2);
  else {
    if (moist2 < setMinMoist2)
      digitalWrite(relayPin2, 0);
    else
      digitalWrite(relayPin2, 1);
  }
  if (!auto3)
    digitalWrite(relayPin3, !relay3);
  else {
    if (moist3 < setMinMoist3)
      digitalWrite(relayPin3, 0);
    else
      digitalWrite(relayPin3, 1);
  }
  if (!auto4)
    digitalWrite(relayPin4, !relay4);
}

void restartESP() {
  counterError++;
  if (counterError > 3) ESP.restart();
}