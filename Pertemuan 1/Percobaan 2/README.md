# Jawaban dari Pertanyaan 2 Modifikasi
```C++
#include <DHT.h>
#define DHTPIN 4        // pin data DHT22 terhubung ke GPIO 4
#define DHTTYPE DHT22
#define RELAYPIN 26     // pin kendali relay/LED indikator

DHT dht(DHTPIN, DHTTYPE);
const float suhuNyala = 30.0;  // ambang batas ATAS: aktuator ON jika suhu > nilai ini
const float suhuMati  = 28.0;  // ambang batas BAWAH: aktuator OFF jika suhu < nilai ini
bool statusAktuator = false;   // status aktuator saat ini (false = OFF, true = ON)

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW); // pastikan aktuator mati di awal
}

void loop() {
  float suhu = dht.readTemperature();
  if (isnan(suhu)) {
    Serial.println("Gagal membaca data sensor!");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C -> ");
    // Kendali aktuator berbasis histerisis dua ambang batas
    if (!statusAktuator && suhu > suhuNyala) {
      // Aktuator off
      statusAktuator = true;
    } else if (statusAktuator && suhu < suhuMati) {
      // Aktuator on
      statusAktuator = false;
    }
    // Jika suhu berada di antara suhuMati dan suhuNyala status aktuator TIDAK diubah (mempertahankan kondisi sebelumnya)
    digitalWrite(RELAYPIN, statusAktuator ? HIGH : LOW);
    Serial.println(statusAktuator ? "Aktuator: ON" : "Aktuator: OFF");
  }
  delay(2000); // jeda pembacaan sensor
}
```
