#include <ESP8266WiFi.h>    // Library untuk Wi-Fi ESP8266
#include <PubSubClient.h>   // Library untuk komunikasi MQTT
#include <ArduinoJson.h>    // Library untuk parsing JSON

// ==================== KONFIGURASI WIFI ====================
const char* ssid = "NAMA_WIFI";          // Ganti dengan SSID WiFi Anda
const char* password = "PASSWORD_WIFI";  // Ganti dengan password WiFi Anda

// ==================== KONFIGURASI MQTT ====================
const char* mqttServer = "broker.hivemq.com";  // Broker MQTT publik
const int mqttPort = 1883;                      // Port MQTT standar
const char* topicPerintah = "unsoed/tk245004/kelompok1/perintah";  // Topic untuk menerima perintah

// ==================== KONFIGURASI PIN ====================
const int ledPin = 4;  // GPIO 4 untuk LED indikator

// ==================== KONFIGURASI PWM ====================
const int pwmFrekuensi = 5000;   // Frekuensi PWM 5 kHz
const int pwmResolusi = 8;       // Resolusi 8 bit -> nilai 0..255
const int intensitasMaks = 255;  // Nilai maksimum untuk resolusi 8 bit

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

    //  Pastikan field "perintah" ada sebelum dipakai
    if (perintah == nullptr) {
        Serial.println("Field 'perintah' tidak ditemukan");
        return;
    }

    //  Ambil "intensitas"; jika tidak ada atau bukan angka, pakai nilai maksimum
    int intensitas = doc["intensitas"] | intensitasMaks;
    //  Batasi nilai ke rentang 0..255 agar aman untuk PWM 8 bit
    intensitas = constrain(intensitas, 0, intensitasMaks);

    // ==================== KONTROL AKTUATOR ====================
    if (String(perintah) == "ON") {
        ledcWrite(ledPin, intensitas);  //  duty cycle PWM sesuai intensitas
        Serial.print("Aktuator: ON, intensitas: ");  // Aktuator LED menyala dengan kecerahan tertentu
        Serial.println(intensitas);                  // Print nilai intensitas ke Serial Monitor
    } else if (String(perintah) == "OFF") {
        ledcWrite(ledPin, 0);  //  duty cycle 0 = LED mati
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
    ledcAttach(ledPin, pwmFrekuensi, pwmResolusi);  // Pengganti pinMode: pin LED jadi keluaran PWM
    ledcWrite(ledPin, 0);           // Pengganti digitalWrite LOW: LED mati saat mulai

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
