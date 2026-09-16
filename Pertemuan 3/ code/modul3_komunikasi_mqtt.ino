#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "MosoZydraGyrottaZao"; // SSID Wi-Fi Eksternal
const char* password = "stone park"; // Password Wi-Fi Eksternal

const char* mqttServer = "broker.hivemq.com"; // Server Broker
const int mqttPort = 1883; // Port Broker
const char* mqttTopic = "unsoed/tk245004/kelompok1/sensor"; // Path Topik Broker

WiFiClient espClient; // Objek untuk menangani koneksi jaringan dasar
PubSubClient client(espClient); // Objek untuk menggunakan koneksi jaringan TCP dari espClient

void hubungkanWiFi() { // Menghubungkan Wi-Fi Eksternal
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() { // Menghubungkan ke Broker MQTT
  while (!client.connected()) { // Perulangan untuk mencoba menghubungkan ke Broker
    Serial.print("Menghubungkan ke broker MQTT...");
    // Membuat Client ID unik berbasis alamat MAC/Random
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() { // Setup Wi-Fi Station dan client server dengan alamat dan port
  Serial.begin(115200); // Baud rate ESP8266
  hubungkanWiFi(); // Menghubungkan ke Wi-Fi eksternal
  client.setServer(mqttServer, mqttPort); // Memasukkan alamat dan port server yang akan dihubungkan
}

void loop() { // Setelah client disetup, menghubungkan dengan server MQTT
  if (!client.connected()) { // Pengecekan apakah terhubung dengan broker
    hubungkanMQTT(); // Menghubungkan kembali jika terputus
  }
  client.loop(); // Dijalankan terus menerus untuk menjaga koneksi

  // Membuat data sensor dalam format JSON
  JsonDocument doc;
  doc["suhu"] = 28.5;
  doc["kelembaban"] = 65.0;
  
  char buffer[128];
  serializeJson(doc, buffer);

  // Mempublikasikan data ke topic MQTT
  client.publish(mqttTopic, buffer);
  
  Serial.print("Data terkirim ke topic ");
  Serial.print(mqttTopic);
  Serial.print(": ");
  Serial.println(buffer);

  delay(5000); // publish data setiap 5 detik
}