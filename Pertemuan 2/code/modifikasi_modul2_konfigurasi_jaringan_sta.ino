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
