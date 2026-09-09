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
