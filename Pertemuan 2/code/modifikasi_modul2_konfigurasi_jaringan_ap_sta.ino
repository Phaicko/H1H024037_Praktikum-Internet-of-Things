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
