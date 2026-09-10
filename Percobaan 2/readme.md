# Pertemuan 2 - Konfigurasi Jaringan

## Dokumentasi 1

![alt text](<Rangkaian2.jpeg>)
*Gambar 2.1. Rangkaian ESP8266 Untuk Percobaan*

![alt text](<SerialMonitor1.png>)
*Gambar 2.2. Serial Monitor saat Percobaan*

## Library Yang Diperlukan:

ESP8266WiFi.h

## Percobaan 1 - Konfigurasi Mode Station (STA)

```C++
#include <ESP8266WiFi.h>
const char *ssid = "MosoZydraGyrottaZao"; // SSID Wi-Fi
const char *password = "stone park";      // Password Wi-Fi
const int ledPin = 2; // LED indikator status koneksi
void setup()
{
    Serial.begin(115200);      // Baud rate ESP8266
    pinMode(ledPin, OUTPUT);   // Setup LED sebagai output
    digitalWrite(ledPin, LOW); // Setup kondisi default LED
    WiFi.mode(WIFI_STA);        // Set mode WiFi menjadi Station
    WiFi.begin(ssid, password); // Memasukkan kredensial Wi-Fi sesuai yang diatur diatas
    Serial.print("Menghubungkan ke WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {               // Indikator saat mencoba menghubungkan Wi-Fi
        delay(500); // Jeda 0.5 detik
        Serial.print(".");
    }
    // Jika berhasil terhubung
    Serial.println();
    Serial.println("WiFi berhasil terhubung!");
    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP()); // Mengembalikan alamat IP lokal
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress()); // Mengembalikan alamat MAC
    Serial.print("RSSI (dBm) : ");
    Serial.println(WiFi.RSSI()); // Pengukuran seberapa kuat sinyal Wi-Fi dalam satuan dBm
    digitalWrite(ledPin, HIGH);  // nyalakan LED sebagai indikator
}
void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {                                        // Mengecek apakah Wi-Fi terhubung menggunakan WiFi.status()
        Serial.println("Status: Terhubung"); // Status Wi-Fi terhubung
    }
    else
    {
        Serial.println("Status: Terputus");
        digitalWrite(ledPin, LOW); // Indikator LED mati jika Wi-Fi terputus
    }
    delay(5000); // Jeda loop 5 detik
}
```

## Modifikasi Program untuk Pertanyaan 1 - Tambah Percabangan untuk Reconnect

```C++
#include <ESP8266WiFi.h>
const char *ssid = "MosoZydraGyrottaZao"; // SSID Wi-Fi
const char *password = "stone park";      // Password Wi-Fi
const int ledPin = 2; // LED indikator status koneksi
unsigned long previousReconnectAttempt = 0; // Menghitung percobaan menyambung ulang
const unsigned long reconnectInterval = 5000; // jeda antar percobaan reconnect (dalam ms)
void setup()
{
    Serial.begin(115200);      // Baud rate ESP8266
    pinMode(ledPin, OUTPUT);   // Setup LED sebagai output
    digitalWrite(ledPin, LOW); // Setup kondisi default LED
    WiFi.mode(WIFI_STA);        // Set mode WiFi menjadi Station
    WiFi.begin(ssid, password); // Memasukkan kredensial Wi-Fi sesuai yang diatur diatas
    Serial.print("Menghubungkan ke WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {               // Indikator saat mencoba menghubungkan Wi-Fi
        delay(500); // Jeda 0.5 detik
        Serial.print(".");
    }
    // Jika berhasil terhubung
    Serial.println();
    Serial.println("WiFi berhasil terhubung!");
    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP()); // Mengembalikan alamat IP lokal
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress()); // Mengembalikan alamat MAC
    Serial.print("RSSI (dBm) : ");
    Serial.println(WiFi.RSSI()); // Pengukuran seberapa kuat sinyal Wi-Fi dalam satuan dBm
    digitalWrite(ledPin, HIGH);  // nyalakan LED sebagai indikator
}
void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {                                        // Mengecek apakah Wi-Fi terhubung menggunakan WiFi.status()
        Serial.println("Status: Terhubung"); // Status Wi-Fi terhubung
    }
    else
    {
        Serial.println("Status: Terputus");
        digitalWrite(ledPin, LOW); // Indikator LED mati jika Wi-Fi terputus
        unsigned long currentMillis = millis(); // Timer non blocking
        if (currentMillis - previousReconnectAttempt >= reconnectInterval) {
            previousReconnectAttempt = currentMillis;
            Serial.println("Mencoba menyambungkan ulang ke WiFi...");
            WiFi.disconnect(); // Melepas sambungan ke Wi-Fi
            WiFi.begin(ssid, password); // Mencoba menyambung ulang setelah melepas sambungan Wi-Fi
        }
    }
    delay(5000); // Jeda loop 5 detik
}
```

## Percobaan 2 - Konfigurasi Mode Access Point (AP)

## Dokumentasi 2

![alt text](<ScreenshotSettings.png>)
*Gamber 2.3. Screenshot dari Handphone yang Terhubung dengan ESP8266*

```C++
#include <ESP8266WiFi.h>                     // Library Wi-Fi untuk ESP8266
const char *ap_ssid = "dj gyrotta zao - ▯▯"; // SSID Wi-Fi AP
const char *ap_password = "123456789";       // Password Wi-Fi AP, minimal 8 karakter
void setup()
{
    Serial.begin(115200);              // Baud rate ESP8266
    WiFi.mode(WIFI_AP);                // Set mode Wi-Fi menjadi Access Point
    WiFi.softAP(ap_ssid, ap_password); // Menyetel kredensial Wi-Fi Access Point
    IPAddress apIP = WiFi.softAPIP();  // Menyetel alamat IP menggunakan alamat bawaan (umumnya 192.162.4.1)
    Serial.println("Access Point aktif!");
    Serial.print("SSID : ");
    Serial.println(ap_ssid); // Mencetak nama SSID AP
    Serial.print("IP Address : ");
    Serial.println(apIP); // Mencetak alamat IP AP
}
void loop()
{
    int jumlahClient = WiFi.softAPgetStationNum(); // Menampilkan jumlah perangkat yang terhubung setiap 5 detik
    Serial.print("Jumlah perangkat terhubung: ");
    Serial.println(jumlahClient); // Mencetak jumlah device terhubung dalam Serial Monitor
    delay(5000);                  // Jeda loop 5 detik
}
```

## Modifikasi Program untuk Pertanyaan 2 - Ubah menjadi mode STA + AP

```C++
#include <ESP8266WiFi.h> // Library Wi-Fi untuk ESP8266
// Kredensial Access Point (AP)
const char *ap_ssid = "dj gyrotta zao - AP"; // SSID Wi-Fi AP
const char *ap_password = "123456789";       // Password Wi-Fi AP, minimal 8 karakter

// Kredensial Wi-Fi yang akan disambungi (Station)
const char *sta_ssid = "MosoZydraGyrottaZao"; // SSID Wi-Fi tujuan
const char *sta_password = "stone park";      // Password Wi-Fi tujuan

void setup()
{
    Serial.begin(115200);              // Baud rate ESP8266
    WiFi.mode(WIFI_AP_STA);            // Mengaktifkan mode Access Point + Station
    WiFi.softAP(ap_ssid, ap_password); // Menyalakan Access Point
    IPAddress apIP = WiFi.softAPIP();  // Alamat IP AP, default 192.168.4.1

    Serial.println("Access Point aktif!");
    Serial.print("AP SSID       : ");
    Serial.println(ap_ssid); // Mencetak nama SSID AP
    Serial.print("AP IP Address : ");
    Serial.println(apIP); // Mencetak alamat IP AP

    // Menyambungkan ESP8266 sebagai Station ke jaringan Wi-Fi lain
    Serial.println();
    Serial.print("Menghubungkan ke Wi-Fi: ");
    Serial.println(sta_ssid);
    WiFi.begin(sta_ssid, sta_password);

    // Menunggu hingga koneksi STA terhubung (dengan indikator titik-titik)
    int percobaan = 0;
    while (WiFi.status() != WL_CONNECTED && percobaan < 30)
    {
        delay(500);
        Serial.print(".");
        percobaan++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) // Menampilkan status koneksi STA
    {
        Serial.println("Berhasil terhubung ke Wi-Fi!");
        Serial.print("STA IP Address : ");
        Serial.println(WiFi.localIP()); // Alamat IP yang didapat dari router
    }
    else
    {
        Serial.println("Gagal terhubung ke Wi-Fi (mode STA). AP tetap berjalan.");
    }
}

void loop()
{
    int jumlahClient = WiFi.softAPgetStationNum(); // Menampilkan jumlah perangkat yang terhubung ke AP setiap 5 detik
    Serial.print("Jumlah perangkat terhubung ke AP : ");
    Serial.println(jumlahClient);
    if (WiFi.status() == WL_CONNECTED) // Menampilkan status koneksi STA (apakah masih terhubung ke router)
    {
        Serial.print("Status STA: Terhubung, IP = ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("Status STA: Terputus dari Wi-Fi");
    }
    Serial.println("--------------------------------------------------");
    delay(5000); // Jeda loop 5 detik
}
```
