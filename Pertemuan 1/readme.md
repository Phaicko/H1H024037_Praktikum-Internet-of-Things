# Pertemuan 1 - Sensor dan Aktuator
![alt text](<board.jpeg>)

## Sensor Yang Digunakan: DHT11
![alt text](<dht11.jpeg>)

## Library Yang Diperlukan: DHT sensor library

## Percobaan 1 - Akuisisi Data Sensor DHT11
```C++
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11 // tipe sensor yang digunakan
DHT dht(DHTPIN, DHTTYPE); // deklarasi pin yang digunakan DHT11
void setup()
{
  Serial.begin(115200);
  dht.begin();  // inisialisasi sensor DHT
  Serial.println("Memulai akuisisi data sensor DHT11...");
}
void loop()
{
  float kelembaban = dht.readHumidity(); // mengambil data kelembapan yang dibaca sensor
  float suhu = dht.readTemperature(); // mengambil data suhu yang dibaca sensor
  if (isnan(kelembaban) || isnan(suhu)) // percabangan untuk mengecek apakah suhu dan/atau kelembapan berupa angka valid
  {
    Serial.println("Gagal membaca data dari sensor DHT11!"); // pesan error jika salah satu data merupakan angka tidak valid
  }
  else
  {
    Serial.print("Suhu: ");
    Serial.print(suhu); // mencetak suhu tercatat oleh sensor
    Serial.print(" °C, Kelembaban: "); // dalam satuan celcius
    Serial.print(kelembaban); // mencetak kelembapan tercatat oleh sensor
    Serial.println(" %"); // dalam satuan persentase
  }
  delay(2000); // jeda setiap pembacaan 2 detik
}
```
## Modifikasi Program untuk Pertanyaan 1
```C++
#include <DHT.h>
#define DHTPIN 2           // pin data DHT terhubung ke GPIO 2
#define DHTTYPE DHT11      // tipe sensor yang digunakan
#define NUM_READINGS 5     // jumlah pembacaan untuk rata-rata
DHT dht(DHTPIN, DHTTYPE);  // deklarasi pin yang digunakan DHT11

void setup() {
  Serial.begin(115200);
  dht.begin();             // inisialisasi sensor DHT
  Serial.println("Memulai akuisisi data sensor DHT dengan metode rata-rata...");
}

void loop() {
  float total_suhu = 0;        // variabel untuk menyimpan akumulasi suhu
  float total_kelembaban = 0;  // variabel untuk menyimpan akumulasi kelembaban
  int valid_readings = 0;
  for (int i = 0; i < NUM_READINGS; i++) { // melakukan loop sebanyak 5 kali
    float kelembaban = dht.readHumidity(); // mengambil data kelembapan yang dibaca sensor
    float suhu = dht.readTemperature(); // mengambil data suhu yang dibaca sensor
    if (!isnan(kelembaban) && !isnan(suhu)) { // percabangan untuk mengecek apakah suhu dan kelembapan berupa angka valid
      total_suhu += suhu;
      total_kelembaban += kelembaban;
      valid_readings++;
    } else {
      Serial.println("Gagal membaca data dari sensor pada iterasi ini!");  // pesan error jika salah satu atau semua data merupakan angka tidak valid
    }
    delay(2000); // jeda setiap pembacaan 2 detik
  }
 
  if (valid_readings > 0) { // jika minimal ada 1 data yang valid, maka hitung dan tampilkan rata-ratanya
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
## Percobaan 2 - Kendali Aktuator Relay Berdasarkan Data Sensor
```C++
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
```
## Modifikasi Program untuk Pertanyaan 2
```C++
#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT11
#define RELAYPIN 26     // deklarasi pin yang digunakan oleh relay
DHT dht(DHTPIN, DHTTYPE); // deklarasi pin yang digunakan oleh DHT11
const float suhuNyala = 30.0;  // threshold atas: aktuator ON jika suhu > nilai ini
const float suhuMati  = 28.0;  // threshold bawah: aktuator OFF jika suhu < nilai ini
bool statusAktuator = false;   // status aktuator saat ini (false = OFF, true = ON)

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
    
    if (!statusAktuator && suhu > suhuNyala) { // kendali aktuator berbasis histerisis dua threshold
      statusAktuator = true;     // relay off
    } else if (statusAktuator && suhu < suhuMati) {
      statusAktuator = false;      // relay on
    }
    digitalWrite(RELAYPIN, statusAktuator ? HIGH : LOW);    // jika suhu berada di antara suhuMati dan suhuNyala status aktuator TIDAK diubah (sustain)
    Serial.println(statusAktuator ? "Aktuator: ON" : "Aktuator: OFF");
  }
  delay(2000); // jeda pembacaan sensor
}
```