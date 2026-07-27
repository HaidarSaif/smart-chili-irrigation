#include <WiFi.h>
#include <esp_now.h>

// Variabel untuk menyimpan data float
float mac1, mac2, mac3;

// Struktur untuk menerima data
typedef struct __attribute__((packed)) {
    float data;
} sensorData;

// Callback ketika data diterima
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
    sensorData receivedData;
    memcpy(&receivedData, incomingData, sizeof(receivedData));

    // Konversi MAC address menjadi string
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             info->src_addr[0], info->src_addr[1], info->src_addr[2], 
             info->src_addr[3], info->src_addr[4], info->src_addr[5]);

    // Memeriksa MAC address pengirim dan menyimpan data ke variabel yang sesuai
    if (strcmp(macStr, "80:7D:3A:EF:7D:D8") == 0) {  // Ganti dengan MAC address ESP32 A
        mac1 = receivedData.data;
        Serial.print("Data dari ESP32 A: ");
        Serial.println(mac1);
    } 
    else if (strcmp(macStr, "C8:2E:18:C3:5B:7C") == 0) {  // Ganti dengan MAC address ESP32 B
        mac2 = receivedData.data;
        Serial.print("Data dari ESP32 B: ");
        Serial.println(mac2);
    } 
    else if (strcmp(macStr, "30:C9:22:D1:96:48") == 0) {  // Ganti dengan MAC address ESP32 C
        mac3 = receivedData.data;
        Serial.print("Data dari ESP32 C: ");
        Serial.println(mac3);
    }
    
    // Mengirim data ke Serial2 dengan format mac1,mac2,mac3
    Serial2.print(mac1);
    Serial2.print(",");
    Serial2.print(mac2);
    Serial2.print(",");
    Serial2.println(mac3);
}

void setup() {
    // Inisialisasi Serial dan Serial2
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, 16, 17);  // Inisialisasi Serial2 pada pin 16 (RX) dan 17 (TX)

    // Inisialisasi WiFi
    WiFi.mode(WIFI_STA);
    esp_now_init();
    
    // Register callback
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    // Tidak ada kode yang diperlukan di loop
}