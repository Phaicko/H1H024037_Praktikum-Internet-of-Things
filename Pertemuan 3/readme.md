# Pertemuan 3 - Protokol Komunikasi

## Dokumentasi 1

![alt text](<SerialMonitor3A.png>)
*Gambar 3.1. Serial Monitor saat Pengiriman Data*

![alt text](<percobaan3a.gif>)
*Gambar 3.2. Serial Monitor sedang Berjalan*

## Library Yang Diperlukan

ESP8266HTTPClient.h (untuk percobaan 3A - HTTP)
ESP8266WiFi.h
ArduinoJson.h
PubSubClient.h (untuk percobaan 3B - MQTT)

## Percobaan 3A - Komunikasi Data Menggunakan HTTP

Percobaan pertama adalah mengirimkan data yang berisikan suhu dan kelembapan yang sudah di set melalui protokol HTTP

```C++
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
const char* ssid = "MosoZydraGyrottaZao"; // SSID Wi-Fi Eksternal
const char* password = "stone park"; // Password Wi-Fi Eksternal
const char* serverUrl = "http://httpbin.org/post"; // Endpoint uji HTTP POST

void setup() { // Setup Wi-Fi Station
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
  if (WiFi.status() == WL_CONNECTED) { // Setelah Wi-Fi terhubung, setup client untuk menghubungkan ke server HTTP
    WiFiClient client; // Objek untuk menangani koneksi jaringan dasar
    HTTPClient http; // Objek untuk menyediakan fungsi saar request HTTP
    http.begin(client, serverUrl); // Menyiapkan alamat server HTTP
    http.addHeader("Content-Type", "application/json");
    
    // Membuat objek data sensor dalam format JSON
    JsonDocument doc;
    doc["suhu"] = 28.5;       // contoh data suhu (°C)
    doc["kelembaban"] = 65.0; // contoh data kelembaban (%)
    
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
```

## Modifikasi Program untuk Pertanyaan 3A - Tambah Timestamp pada Data dengan millis()

```C++
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
```

## Percobaan 3B - Komunikasi MQTT

Percobaan kedua adalah mengirim data berisikan suhu dan kelembapan yang sudah di set melalui protokol MQTT dengan broker HiveMQ

## Dokumentasi 2

![alt text](<SerialMonitor3B.png>)
*Gamber 3.3. Serial Monitor saat Pengiriman Data*

![alt text](<hivemq.png>)
*Gamber 3.4. Log / History Broker HiveMQ*

![alt text](<percobaan3b.gif>)
*Gamber 3.5. Log / History melakukan Pencatatan Pengiriman pada Broker HiveMQ*

```C++
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "MosoZydraGyrottaZao"; // SSID Wi-Fi Eksternal
const char* password = "stone park"; // Password Wi-Fi Eksternal

const char* mqttServer = "broker.hivemq.com"; // Server Broker
const int mqttPort = 1883; // Port Broker
const char* mqttTopic = "unsoed/tk245004/kelompok1/sensor"; // Path Topik Broker

WiFiClient espClient; // Objek untuk menangani koneksi jaringan dasar
PubSubClient client(espClient); // Objek untuk menggunakan koneksi jaringan TCP dari espClient

void hubungkanWiFi() { // Menghubungkan Wi-Fi Eksternal
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() { // Menghubungkan ke Broker MQTT
  while (!client.connected()) { // Perulangan untuk mencoba menghubungkan ke Broker
    Serial.print("Menghubungkan ke broker MQTT...");
    // Membuat Client ID unik berbasis alamat MAC/Random
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() { // Setup Wi-Fi Station dan client server dengan alamat dan port
  Serial.begin(115200); // Baud rate ESP8266
  hubungkanWiFi(); // Menghubungkan ke Wi-Fi eksternal
  client.setServer(mqttServer, mqttPort); // Memasukkan alamat dan port server yang akan dihubungkan
}

void loop() { // Setelah client disetup, menghubungkan dengan server MQTT
  if (!client.connected()) { // Pengecekan apakah terhubung dengan broker
    hubungkanMQTT(); // Menghubungkan kembali jika terputus
  }
  client.loop(); // Dijalankan terus menerus untuk menjaga koneksi

  // Membuat data sensor dalam format JSON
  JsonDocument doc;
  doc["suhu"] = 28.5;
  doc["kelembaban"] = 65.0;
  
  char buffer[128];
  serializeJson(doc, buffer);

  // Mempublikasikan data ke topic MQTT
  client.publish(mqttTopic, buffer);
  
  Serial.print("Data terkirim ke topic ");
  Serial.print(mqttTopic);
  Serial.print(": ");
  Serial.println(buffer);

  delay(5000); // publish data setiap 5 detik
}
```
