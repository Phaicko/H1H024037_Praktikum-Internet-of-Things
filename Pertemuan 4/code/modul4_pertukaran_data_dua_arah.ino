/*
 * Modul 4 - Percobaan 4B
 * Pertukaran Data Dua Arah (Full Duplex)
 * 
 * Board: ESP32
 * Sensor: DHT22 pada GPIO 4
 * Aktuator: LED pada GPIO 26
 * 
 * Program ini mempublikasikan data suhu secara berkala ke topic data
 * dan secara bersamaan menerima perintah kendali dari topic perintah.
 * Menggunakan millis() untuk non-blocking publish.
 */

#include <ESP8266WiFi.h>    // Library untuk koneksi WiFi
#include <PubSubClient.h>   // Library untuk komunikasi MQTT
#include <ArduinoJson.h>    // Library untuk JSON
#include <DHT.h>            // Library untuk sensor DHT

// ==================== KONFIGURASI WIFI ====================
const char* ssid = "MosoZydraGyrottaZao";          // Ganti dengan SSID WiFi Anda
const char* password = "stone park";  // Ganti dengan password WiFi Anda

// ==================== KONFIGURASI MQTT ====================
const char* mqttServer = "broker.hivemq.com";  // Broker MQTT publik
const int mqttPort = 1883;                      // Port MQTT standar
const char* topicData = "unsoed/tk245004/kelompok1/data";        // Topic untuk publish data sensor
const char* topicPerintah = "unsoed/tk245004/kelompok1/perintah"; // Topic untuk subscribe perintah

// ==================== KONFIGURASI SENSOR DHT ====================
#define DHTPIN 5        // GPIO 5 untuk sensor DHT
#define DHTTYPE DHT11   // Tipe sensor DHT11

// ==================== KONFIGURASI PIN ====================
const int ledPin = 4;  // GPIO 4 untuk LED

// ==================== OBJEK GLOBAL ====================
DHT dht(DHTPIN, DHTTYPE);       // Objek sensor DHT
WiFiClient espClient;           // Objek client WiFi
PubSubClient client(espClient); // Objek client MQTT

// ==================== VARIABEL NON-BLOCKING ====================
unsigned long waktuTerakhirPublish = 0;     // Menyimpan waktu terakhir publish
const long intervalPublish = 5000;          // Interval publish 5 detik

// ==================== FUNGSI CALLBACK ====================
// Dipanggil otomatis saat ada pesan masuk pada topic yang di-subscribe
void callback(char* topic, byte* payload, unsigned int length) {
    String pesan;
    
    // Konversi payload byte array ke String
    for (unsigned int i = 0; i < length; i++) {
        pesan += (char)payload[i];
    }
    
    // Parsing JSON
    JsonDocument doc;
    if (deserializeJson(doc, pesan)) return;  // Abaikan jika parsing gagal
    
    // Ambil nilai perintah
    const char* perintah = doc["perintah"];
    
    // Kontrol LED berdasarkan perintah
    digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
    
    Serial.println("Perintah diterima -> Aktuator: ");
    Serial.println(perintah);
}

// ==================== FUNGSI KONEKSI WIFI ====================
void hubungkanWiFi() {
    WiFi.begin(ssid, password);
    
    // Tunggu sampai terhubung
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Serial.println("WiFi berhasil terhubung!");
}

// ==================== FUNGSI KONEKSI MQTT ====================
void hubungkanMQTT() {
    // Loop sampai berhasil terhubung
    while (!client.connected()) {
        // Buat client ID unik
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        
        if (client.connect(clientId.c_str())) {
            // Subscribe ke topic perintah
            client.subscribe(topicPerintah);
            Serial.println("Terhubung dan subscribe topic perintah");
        } else {
            delay(2000);  // Tunggu 2 detik sebelum coba lagi
        }
    }
}

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);       // Inisialisasi Serial Monitor
    pinMode(ledPin, OUTPUT);    // Set GPIO 26 sebagai output
    dht.begin();                // Inisialisasi sensor DHT
    
    hubungkanWiFi();            // Hubungkan ke WiFi
    
    client.setServer(mqttServer, mqttPort);  // Set alamat broker MQTT
    client.setCallback(callback);            // Daftarkan fungsi callback
}

// ==================== LOOP ====================
void loop() {
    // Cek koneksi MQTT, sambungkan ulang jika terputus
    if (!client.connected()) hubungkanMQTT();
    
    // Memproses pesan masuk secara terus-menerus (non-blocking)
    client.loop();
    
    // ==================== PUBLISH DATA SENSOR (NON-BLOCKING) ====================
    // Cek apakah sudah waktunya publish berdasarkan millis()
    if (millis() - waktuTerakhirPublish > intervalPublish) {
        waktuTerakhirPublish = millis();  // Update waktu terakhir publish
        
        // Baca suhu dari sensor DHT
        float suhu = dht.readTemperature();
        
        // Cek apakah pembacaan valid (bukan NaN)
        if (!isnan(suhu)) {
            // Buat JSON document
            JsonDocument doc;
            doc["suhu"] = suhu;
            
            // Serialisasi JSON ke buffer
            char buffer[128];
            serializeJson(doc, buffer);
            
            // Publish ke topic data
            client.publish(topicData, buffer);
            
            Serial.print("Data terkirim: ");
            Serial.println(buffer);
        }
    }
}