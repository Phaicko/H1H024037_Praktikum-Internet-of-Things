#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); // deklarasi pin yang digunakan DHT11
void setup()
{
  Serial.begin(115200);
  dht.begin();
  Serial.println("Memulai akuisisi data sensor DHT11...");
}
void loop()
{
  float kelembaban = dht.readHumidity(); // mengambil data kelembapan yang dibaca sensor
  float suhu = dht.readTemperature(); // mengambil data suhu yang dibaca sensor
  if (isnan(kelembaban) || isnan(suhu)) // percabangan untuk mengecek apakah suhu dan kelembapan berupa angka valid
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
  delay(2000);
}
