# Pertemuan 4 - Komunikasi Pertukaran Data

## Dokumentasi 1

![alt text](<rangkaian4-1.jpg>)
*Gambar 4.1. Rangkaian pada Percobaan 1*

![alt text](<SerialMonitor4-1.png>)
*Gambar 4.2. Serial Monitor saat Percobaan 1*

![alt text](<hivemq1.png>)
*Gambar 4.3. Tampilan History pada MQTT Explorer*

## Library Yang Diperlukan

ESP8266WiFi.h
ArduinoJson.h
PubSubClient.h
DHT.h (Untuk Percobaan 4B)

## Percobaan 4A - Subscribe dan Deserialisasi JSON

Percobaan pertama adalah mengirimkan data berupa perintah melalui MQTT dalam format JSON.

```C++
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
```

## Modifikasi Program untuk Pertanyaan 4A

```C++
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
    // Wajib dipanggil terus-menerus agar pesan dapat diterima dan koneksi ke broker tetap aktif
    client.loop();
}
```

## Percobaan 4B - Pertukaran Data Dua Arah

Percobaan kedua adalah mengirim data berupa perintah sekaligus menerima data berupa suhu dan kelembapan melalui MQTT dalam format JSON.

## Dokumentasi 2

![alt text](<rangkaian4-2.jpg>)
*Gambar 4.4. Rangkaian pada Percobaan 2*

![alt text](<SerialMonitor4-2.png>)
*Gambar 4.5. Serial Monitor saat Percobaan 2*

![alt text](<hivemq2.png>)
*Gambar 4.6. Tampilan History pada MQTT Explorer*

```C++
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
```

## Modifikasi Program untuk Pertanyaan 4B

```C++
#include <ESP8266WiFi.h>  // Library untuk koneksi WiFi
#include <PubSubClient.h> // Library untuk komunikasi MQTT
#include <ArduinoJson.h>  // Library untuk JSON
#include <DHT.h>          // Library untuk sensor DHT
// ==================== KONFIGURASI WIFI ====================
const char *ssid = "MosoZydraGyrottaZao"; // Ganti dengan SSID WiFi Anda
const char *password = "stone park";      // Ganti dengan password WiFi Anda
// ==================== KONFIGURASI MQTT ====================
const char *mqttServer = "broker.hivemq.com";                     // Broker MQTT publik
const int mqttPort = 1883;                                        // Port MQTT standar
const char *topicData = "unsoed/tk245004/kelompok1/data";         // Topic untuk publish data sensor
const char *topicPerintah = "unsoed/tk245004/kelompok1/perintah"; // Topic untuk subscribe perintah (LED)
const char *topicBuzzer = "unsoed/tk245004/kelompok1/buzzer";     //  Topic untuk subscribe perintah buzzer
// ==================== KONFIGURASI SENSOR DHT ====================
#define DHTPIN 5      // GPIO 5 untuk sensor DHT
#define DHTTYPE DHT11 // Tipe sensor DHT11
// ==================== KONFIGURASI PIN ====================
const int ledPin = 4;     // GPIO 4 untuk LED
const int buzzerPin = 14; //   GPIO 14 untuk buzzer
// ==================== OBJEK GLOBAL ====================
DHT dht(DHTPIN, DHTTYPE);       // Objek sensor DHT
WiFiClient espClient;           // Objek client WiFi
PubSubClient client(espClient); // Objek client MQTT
// ==================== VARIABEL NON-BLOCKING ====================
unsigned long waktuTerakhirPublish = 0; // Menyimpan waktu terakhir publish
const long intervalPublish = 5000;      // Interval publish 5 detik
// ==================== FUNGSI CALLBACK ====================
// Dipanggil otomatis saat ada pesan masuk pada topic yang di-subscribe, sekarang callback membedakan topic asal pesan (LED atau buzzer)
void callback(char *topic, byte *payload, unsigned int length)
{
    String pesan;
    // Konversi payload byte array ke String
    for (unsigned int i = 0; i < length; i++)
    {
        pesan += (char)payload[i];
    }
    Serial.print("Pesan masuk dari topic: "); //   Tampilkan topic asal pesan
    Serial.println(topic);                    //
    // Parsing JSON
    JsonDocument doc;
    if (deserializeJson(doc, pesan))
        return; // Abaikan jika parsing gagal
    // Ambil nilai perintah
    const char *perintah = doc["perintah"];
    if (perintah == NULL)
        return; //   Abaikan jika key "perintah" tidak ada
    bool nyala = (String(perintah) == "ON"); //   true jika ON, false jika selain ON
    //   Bedakan aktuator berdasarkan topic yang menerima pesan
    if (String(topic) == topicPerintah)
    {
        // Pesan datang dari topic LED
        digitalWrite(ledPin, nyala ? HIGH : LOW);
        Serial.print("Perintah diterima -> LED: ");
        Serial.println(perintah);
    }
    else if (String(topic) == topicBuzzer)
    {
        // Pesan datang dari topic buzzer
        digitalWrite(buzzerPin, nyala ? HIGH : LOW);
        Serial.print("Perintah diterima -> Buzzer: ");
        Serial.println(perintah);
    }
}
// ==================== FUNGSI KONEKSI WIFI ====================
void hubungkanWiFi()
{
    WiFi.begin(ssid, password);
    // Tunggu sampai terhubung
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
    Serial.println("WiFi berhasil terhubung!");
}
// ==================== FUNGSI KONEKSI MQTT ====================
void hubungkanMQTT()
{
    // Loop sampai berhasil terhubung
    while (!client.connected())
    {
        // Buat client ID unik
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);

        if (client.connect(clientId.c_str()))
        {
            // Subscribe ke topic perintah
            client.subscribe(topicPerintah);
            client.subscribe(topicBuzzer); //   Subscribe juga ke topic buzzer
            Serial.println("Terhubung dan subscribe topic perintah & buzzer");
        }
        else
        {
            delay(2000); // Tunggu 2 detik sebelum coba lagi
        }
    }
}
// ==================== SETUP ====================
void setup()
{
    Serial.begin(115200);         // Inisialisasi Serial Monitor
    pinMode(ledPin, OUTPUT);      // Set pin LED sebagai output
    pinMode(buzzerPin, OUTPUT);   //   Set pin buzzer sebagai output
    digitalWrite(buzzerPin, LOW); //   Pastikan buzzer mati saat awal
    dht.begin();                  // Inisialisasi sensor DHT
    hubungkanWiFi(); // Hubungkan ke WiFi
    client.setServer(mqttServer, mqttPort); // Set alamat broker MQTT
    client.setCallback(callback);           // Daftarkan fungsi callback
}
// ==================== LOOP ====================
void loop()
{
    // Cek koneksi MQTT, sambungkan ulang jika terputus
    if (!client.connected())
        hubungkanMQTT();
    // Memproses pesan masuk secara terus-menerus (non-blocking)
    client.loop();
// ==================== PUBLISH DATA SENSOR (NON-BLOCKING) ====================
    // Cek apakah sudah waktunya publish berdasarkan millis()
    if (millis() - waktuTerakhirPublish > intervalPublish)
    {
        waktuTerakhirPublish = millis(); // Update waktu terakhir publish
        // Baca suhu dari sensor DHT
        float suhu = dht.readTemperature();
        // Cek apakah pembacaan valid (bukan NaN)
        if (!isnan(suhu))
        {
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
```
