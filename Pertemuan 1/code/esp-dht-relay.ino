#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT11 // tipe sensor yang digunakan
#define RELAYPIN 26 // deklarasi pin yang digunakan oleh relay
DHT dht(DHTPIN, DHTTYPE); // deklarasi pin yang digunakan oleh DHT11
const float suhuThreshold = 30.0; // threshold suhu
void setup() {
  Serial.begin(115200);
  dht.begin(); // inisialisasi sensor DHT
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW); // pastikan aktuator mati di awal
}
void loop() {
  float suhu = dht.readTemperature(); // mengambil data suhu yang dibaca sensor
  if (isnan(suhu)) { // percabangan untuk mengecek apakah suhu berupa angka valid
    Serial.println("Gagal membaca data sensor!"); // pesan error jika suhu merupakan angka tidak valid
    } else {
    Serial.print("Suhu: ");
    Serial.print(suhu); // mencetak suhu tercatat oleh sensor
    Serial.print(" °C -> "); // dalam satuan celcius
    if (suhu > suhuThreshold) {
      digitalWrite(RELAYPIN, HIGH);
      Serial.println("Aktuator: ON"); // menyalakan relay jika suhu terbaca melebihi threshold
    } else {
      digitalWrite(RELAYPIN, LOW);
      Serial.println("Aktuator: OFF"); // relay akan tetap mati
    }
  }
  delay(2000); // jeda setiap pembacaan 2 detik
}
