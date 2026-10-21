#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const int N = 5;              // jumlah sampel moving average
float buffer[N];              // menyimpan N pembacaan terakhir
int indeks = 0;               // posisi penulisan berikutnya pada buffer
int jumlah = 0;               // jumlah sampel yang sudah terisi

float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Fungsi untuk menghitung moving average
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

// TAMBAHAN: fungsi untuk menghitung simpangan baku data buffer
float hitungSimpanganBaku() {
  if (jumlah == 0) return 0.0;       // mencegah pembagian dengan nol

  float total = 0.0;                 // menyimpan jumlah seluruh data
  for (int i = 0; i < jumlah; i++) { // menelusuri data yang terisi
    total += buffer[i];              // menjumlahkan data buffer
  }

  float rataRata = total / jumlah;   // menghitung rata-rata buffer
  float jumlahKuadrat = 0.0;         // menyimpan total kuadrat selisih

  for (int i = 0; i < jumlah; i++) { // menelusuri data buffer kembali
    float selisih = buffer[i] - rataRata; // selisih data dengan rata-rata
    jumlahKuadrat += selisih * selisih;   // menjumlahkan kuadrat selisih
  }

  float varians = jumlahKuadrat / jumlah; // menghitung varians populasi
  return sqrt(varians);                   // akar varians adalah simpangan baku
}

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  float suhu = dht.readTemperature();

  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    float suhuFilter = hitungMovingAverage(suhu);
    float simpanganBaku = hitungSimpanganBaku(); // mod: menghitung simpangan baku buffer

    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    Serial.print("Mentah: ");
    Serial.print(suhu);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter);
    Serial.print(" | Simpangan Baku: "); // mod: label output simpangan baku
    Serial.print(simpanganBaku);         // mod: menampilkan simpangan baku
    Serial.print(" | Min: ");
    Serial.print(suhuMin);
    Serial.print(" | Max: ");
    Serial.println(suhuMax);
  }

  delay(2000);
}
