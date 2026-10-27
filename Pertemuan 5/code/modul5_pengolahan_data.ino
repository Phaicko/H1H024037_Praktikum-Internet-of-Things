#include <DHT.h>

#define DHTPIN 4          // D2 = GPIO4 pada NodeMCU
#define DHTTYPE DHT11     // gunakan DHT11

DHT dht(DHTPIN, DHTTYPE);

const int N = 5;          // ukuran jendela moving average
float buffer[N];          // buffer data suhu
int indeks = 0;
int jumlah = 0;

float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Menghitung moving average
float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru;
  indeks = (indeks + 1) % N;
  if (jumlah < N) jumlah++;

  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }
  return total / jumlah;
}

// Menghitung simpangan baku (populasi) dari buffer
float hitungSimpanganBaku(float rataRata) {
  float jumlahKuadrat = 0.0;
  for (int i = 0; i < jumlah; i++) {
    float selisih = buffer[i] - rataRata;
    jumlahKuadrat += selisih * selisih;
  }
  if (jumlah <= 1) return 0.0;
  return sqrt(jumlahKuadrat / jumlah);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("Percobaan 5A - ESP8266 + DHT11");
}

void loop() {
  float suhu = dht.readTemperature();

  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    float suhuFilter = hitungMovingAverage(suhu);
    float simpangan  = hitungSimpanganBaku(suhuFilter);

    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    Serial.print("Mentah: ");
    Serial.print(suhu, 1);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter, 2);
    Serial.print(" | Std: ");
    Serial.print(simpangan, 2);
    Serial.print(" | Min: ");
    Serial.print(suhuMin, 1);
    Serial.print(" | Max: ");
    Serial.println(suhuMax, 1);
  }

  delay(2000);  // DHT11 minimal 1 detik, aman 2 detik
}