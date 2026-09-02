# Jawaban dari Pertanyaan 1 Modifikasi
```cpp
#include <DHT.h>
#define DHTPIN 2               // pin data DHT terhubung ke GPIO 2
#define DHTTYPE DHT11          // tipe sensor yang digunakan
#define NUM_READINGS 5         // konstanta jumlah pembacaan untuk rata-rata
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();                 // inisialisasi sensor DHT
  Serial.println("Memulai akuisisi data sensor DHT dengan metode rata-rata...");
}

void loop() {
  float total_suhu = 0;        // variabel untuk menyimpan akumulasi suhu
  float total_kelembaban = 0;  // variabel untuk menyimpan akumulasi kelembaban
  int valid_readings = 0;      // variabel untuk menghitung jumlah pembacaan yang sukses (valid)
  // melakukan loop sebanyak 5 kali
  for (int i = 0; i < NUM_READINGS; i++) {
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();
    // memeriksa apakah data berhasil terbaca
    if (!isnan(kelembaban) && !isnan(suhu)) {
      total_suhu += suhu;             // menambahkan nilai suhu saat ini ke total
      total_kelembaban += kelembaban; // menambahkan nilai kelembaban saat ini ke total
      valid_readings++;               // menambah counter pembacaan yang berhasil
    } else {
      Serial.println("Gagal membaca data dari sensor pada iterasi ini!");
    }
    delay(2000); // jeda setiap pembacaan 2 detik (DHT membutuhkan jeda ini antar pembacaan)
  }
  // jika minimal ada 1 data yang valid, maka hitung dan tampilkan rata-ratanya
  if (valid_readings > 0) {
    float rata_suhu = total_suhu / valid_readings;              // menghitung rata-rata suhu
    float rata_kelembaban = total_kelembaban / valid_readings;  // menghitung rata-rata kelembaban
    Serial.print("Hasil Rata-rata (");
    Serial.print(valid_readings);
    Serial.print(" sampel) -> Suhu: ");
    Serial.print(rata_suhu);
    Serial.print(" °C, Kelembaban: ");
    Serial.print(rata_kelembaban);
    Serial.println(" %");
  } else {
    Serial.println("Siklus ini gagal mendapatkan data yang valid dari sensor.");
  }
}
```
