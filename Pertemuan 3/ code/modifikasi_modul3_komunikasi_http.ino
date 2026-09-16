#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
const char* ssid = "MosoZydraGyrottaZao"; // SSID Wi-Fi Eksternal
const char* password = "stone park"; // Password Wi-Fi Eksternal
const char* serverUrl = "http://httpbin.org/post"; // Endpoint uji HTTP POST

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client; // Objek untuk menangani koneksi jaringan dasar
    HTTPClient http; // Objek untuk menyediakan fungsi saar request HTTP
    http.begin(client, serverUrl); // Menyiapkan alamat server HTTP
    http.addHeader("Content-Type", "application/json");
    
    // Membuat objek data sensor dalam format JSON
    JsonDocument doc;
    doc["suhu"] = 28.5;             // contoh data suhu (°C)
    doc["kelembaban"] = 65.0;       // contoh data kelembaban (%)
    doc["waktu_ms"] = millis();     // waktu sejak ESP32 menyala (dalam milidetik)
    
    String requestBody; // Membuat body kosong
    serializeJson(doc, requestBody); // Mengisi body dengan value yang diubah menjadi teks JSON
    
    Serial.print("Mengirim data: "); // Menulis data ke Serial Monitor
    Serial.println(requestBody);
    
    // Mengirim data melalui HTTP POST
    int httpResponseCode = http.POST(requestBody);
    
    if (httpResponseCode > 0) {
      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);
      Serial.println("Isi Response:");
      Serial.println(http.getString());
    } else {
      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  delay(10000); // kirim data setiap 10 detik
}