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
    Serial.println(ap_ssid); // Mencetak nama SSID
    Serial.print("IP Address : ");
    Serial.println(apIP); // Mencetak alamat IP
}
void loop()
{
    int jumlahClient = WiFi.softAPgetStationNum(); // Menampilkan jumlah perangkat yang terhubung setiap 5 detik
    Serial.print("Jumlah perangkat terhubung: ");
    Serial.println(jumlahClient); // Mencetak jumlah device terhubung dalam Serial Monitor
    delay(5000);                  // Jeda loop 5 detik
}
