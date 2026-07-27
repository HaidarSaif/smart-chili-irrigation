#include <esp_now.h>
#include <WiFi.h>

typedef struct {
  float sensorValue;
} structMessage;

structMessage data;

uint8_t gatewayMACAddress[] = { 0x08, 0xD1, 0xF9, 0xDF, 0xF9, 0xB4 };  // Ganti dengan alamat MAC Slave
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(onSent);
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, gatewayMACAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  float readSensor = analogRead(36);
  readSensor = map(readSensor, 0, 3300, 100, 0);
  if (readSensor < 0)readSensor = 0;
  if (readSensor > 100)readSensor = 100;
  data.sensorValue = readSensor;
  esp_err_t result = esp_now_send(gatewayMACAddress, (uint8_t *)&data, sizeof(data));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  delay(1000);
}