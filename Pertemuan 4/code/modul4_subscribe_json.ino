/*
 * Modul 4 - Percobaan 4A
 * Subscribe dan Deserialisasi Data JSON untuk Kendali Aktuator
 * 
 * Board: ESP32
 * Sensor: - (hanya LED sebagai aktuator)
 * Aktuator: LED pada GPIO 26
 * 
 * Program ini melakukan subscribe ke topic MQTT, menerima pesan JSON,
 * melakukan deserialisasi, dan mengendalikan LED berdasarkan perintah.
 */

#include <ESP8266WiFi.h>    // Library untuk koneksi WiFi
#include <PubSubClient.h>   // Library untuk komunikasi MQTT
#include <ArduinoJson.h>    // Library untuk parsing JSON

// ==================== KONFIGURASI WIFI ====================
const char* ssid = "MosoZydraGyrottaZao";          // Ganti dengan SSID WiFi Anda
const char* password = "stone park";  // Ganti dengan password WiFi Anda

// ==================== KONFIGURASI MQTT ====================
const char* mqttServer = "broker.hivemq.com";  // Broker MQTT publik
const int mqttPort = 1883;                      // Port MQTT standar
const char* topicPerintah = "unsoed/tk245004/kelompok1/perintah";  // Topic untuk menerima perintah

// ==================== KONFIGURASI PIN ====================
const int ledPin = 4;  // GPIO 4 untuk LED indikator

// ==================== OBJEK GLOBAL ====================
WiFiClient espClient;           // Objek client WiFi
PubSubClient client(espClient); // Objek client MQTT

// ==================== FUNGSI CALLBACK ====================
// Fungsi ini dipanggil secara otomatis setiap ada pesan baru masuk
// pada topic yang di-subscribe
void callback(char* topic, byte* payload, unsigned int length) {
    String pesan;
    
    // Mengubah payload (byte array) menjadi String
    for (unsigned int i = 0; i < length; i++) {
        pesan += (char)payload[i];
    }
    
    // Menampilkan pesan mentah yang diterima
    Serial.print("Pesan diterima [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(pesan);
    
    // ==================== DESERIALISASI JSON ====================
    JsonDocument doc;  // Membuat objek JSON document
    DeserializationError error = deserializeJson(doc, pesan);  // Parsing JSON
    
    // Cek apakah parsing berhasil
    if (error) {
        Serial.print("Gagal parsing JSON: ");
        Serial.println(error.c_str());
        return;  // Keluar dari fungsi jika parsing gagal
    }
    
    // Mengambil nilai "perintah" dari JSON
    const char* perintah = doc["perintah"];
    
    // ==================== KONTROL AKTUATOR ====================
    if (String(perintah) == "ON") {
        digitalWrite(ledPin, HIGH);  // Nyalakan LED
        Serial.println("Aktuator: ON");
    } else if (String(perintah) == "OFF") {
        digitalWrite(ledPin, LOW);   // Matikan LED
        Serial.println("Aktuator: OFF");
    }
}

// ==================== FUNGSI KONEKSI WIFI ====================
void hubungkanWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Menghubungkan ke WiFi");
    
    // Tunggu sampai terhubung
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi berhasil terhubung!");
}

// ==================== FUNGSI KONEKSI MQTT ====================
void hubungkanMQTT() {
    // Loop sampai berhasil terhubung ke broker
    while (!client.connected()) {
        Serial.print("Menghubungkan ke broker MQTT...");
        
        // Membuat client ID unik menggunakan random
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        
        if (client.connect(clientId.c_str())) {
            Serial.println("berhasil terhubung!");
            
            // Subscribe ke topic perintah setelah berhasil terhubung
            client.subscribe(topicPerintah);
            Serial.print("Subscribe ke topic: ");
            Serial.println(topicPerintah);
        } else {
            Serial.print("gagal, rc=");
            Serial.print(client.state());
            Serial.println(" coba lagi dalam 2 detik");
            delay(2000);
        }
    }
}

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);           // Inisialisasi Serial Monitor
    pinMode(ledPin, OUTPUT);        // Set GPIO 26 sebagai output
    digitalWrite(ledPin, LOW);      // Pastikan LED mati saat mulai
    
    hubungkanWiFi();                // Hubungkan ke WiFi
    
    client.setServer(mqttServer, mqttPort);  // Set alamat broker MQTT
    client.setCallback(callback);            // Daftarkan fungsi callback
}

// ==================== LOOP ====================
void loop() {
    // Cek koneksi MQTT, jika terputus maka sambungkan ulang
    if (!client.connected()) {
        hubungkanMQTT();
    }
    
    // Wajib dipanggil terus-menerus agar pesan dapat diterima
    // dan koneksi ke broker tetap aktif
    client.loop();
}