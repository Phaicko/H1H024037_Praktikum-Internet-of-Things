import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

# ---------------- KONFIGURASI ----------------
BROKER        = "broker.hivemq.com"
PORT          = 1883
TOPIC         = "unsoed/tk245004/kelompok1/data"   # ← SAMAKAN dengan topicData ESP8266
JUMLAH_SAMPEL = 30
# ---------------------------------------------

data = []

# ---------- CALLBACK ----------
def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)
    print("Subscribe topik:", TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu    = float(payload["suhu"])
    except (ValueError, KeyError):
        print("Data tidak valid, diabaikan")
        return

    waktu = time.strftime("%Y-%m-%d %H:%M:%S")
    data.append({"waktu": waktu, "suhu": suhu})
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")

    if len(data) >= JUMLAH_SAMPEL:
        print("Jumlah sampel terpenuhi, memutus koneksi...")
        client.disconnect()

# ---------- TAHAP 1: AKUISISI MQTT ----------
print("Menghubungkan ke broker...")
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT)
client.loop_forever()

# ---------- TAHAP 2: SIMPAN CSV AWAL ----------
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")
df = pd.read_csv("data_suhu.csv")  # Membaca data dari CSV sebagai sumber analisis berikutnya.
df["suhu_ma5"] = df["suhu"].rolling(window=5, min_periods=1).mean()  # Menghitung MA5 terlebih dahulu dari data CSV.

# ---------- TAHAP 3: STATISTIK + Z-SCORE (DATA ASLI UNTUK PEMBANDING) ----------
rata2 = df["suhu"].mean()
std   = df["suhu"].std()

if std == 0:
    df["zscore"] = 0.0
else:
    df["zscore"] = (df["suhu"] - rata2) / std

df["anomali"] = df["zscore"].abs() > 2

print("\n=== Statistik Data Asli ===")
print(f"Rata-rata     : {rata2:.2f}")
print(f"Std deviasi   : {std:.2f}")
print(f"Minimum       : {df['suhu'].min():.2f}")
print(f"Maksimum      : {df['suhu'].max():.2f}")
print(f"Jumlah anomali: {df['anomali'].sum()}")

# ---------- TAHAP 3B: Z-SCORE PADA DATA HASIL MA5 ----------
rata2_ma = df["suhu_ma5"].mean()  # Menghitung rata-rata dari hasil MA5.
std_ma = df["suhu_ma5"].std()  # Menghitung simpangan baku hasil MA5.

if std_ma == 0:  # Memeriksa apakah semua nilai MA5 sama.
    df["zscore_ma"] = 0.0  # Menghindari pembagian dengan nol.
else:  # Dilakukan jika simpangan baku MA5 tidak nol.
    df["zscore_ma"] = (df["suhu_ma5"] - rata2_ma) / std_ma  # Menghitung z-score dari nilai MA5.

df["anomali_ma"] = df["zscore_ma"].abs() > 2  # Menandai MA5 sebagai anomali jika |z| > 2.
jumlah_anomali_asli = int(df["anomali"].sum())  # Menghitung anomali pada data suhu asli.
jumlah_anomali_ma5 = int(df["anomali_ma"].sum())  # Menghitung anomali setelah MA5.
selisih_anomali = jumlah_anomali_asli - jumlah_anomali_ma5  # Membandingkan jumlah anomali sebelum dan sesudah MA5.

print("\n=== Statistik Setelah Moving Average 5 ===")  # Menampilkan judul ringkasan hasil MA5.
print(f"Rata-rata MA5     : {rata2_ma:.2f}")  # Menampilkan rata-rata hasil MA5.
print(f"Std deviasi MA5   : {std_ma:.2f}")  # Menampilkan simpangan baku hasil MA5.
print(f"Jumlah anomali asli: {jumlah_anomali_asli}")  # Menampilkan jumlah anomali data asli.
print(f"Jumlah anomali MA5 : {jumlah_anomali_ma5}")  # Menampilkan jumlah anomali hasil MA5.
print(f"Selisih (asli-MA5) : {selisih_anomali}")  # Positif berarti MA5 mendeteksi lebih sedikit anomali.

# ---------- TAHAP 3C: TAMPILKAN Z-SCORE TIAP DATA DI TERMINAL ----------
print("\n=== Z-Score Tiap Data (Data Asli) ===")
print(df[["waktu", "suhu", "zscore", "anomali"]].to_string(index=False))

print("\n=== Data yang Terdeteksi Anomali (|z| > 2) ===")
anomali_only = df[df["anomali"]][["waktu", "suhu", "zscore"]]
if len(anomali_only) == 0:
    print("(Tidak ada anomali terdeteksi)")
else:
    print(anomali_only.to_string(index=False))

print("\n=== Z-Score Tiap Data Setelah MA5 ===")  # Menampilkan judul tabel hasil deteksi setelah MA5.
print(df[["waktu", "suhu_ma5", "zscore_ma", "anomali_ma"]].to_string(index=False))  # Menampilkan waktu, MA5, z-score, dan status anomali.

# ---------- TAHAP 4: GRAFIK DATA ASLI ----------
plt.figure(figsize=(10, 4))
plt.plot(df.index, df["suhu"], marker="o", label="Suhu (asli)")

anom = df[df["anomali"]]
plt.scatter(anom.index, anom["suhu"], color="red", s=100,
            label="Anomali (|z| > 2)", zorder=3)

plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata")
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Data Suhu dan Deteksi Anomali (Data Asli)")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu.png")
print("\nGrafik disimpan: grafik_suhu.png")
plt.show()

# ---------- TAHAP 5: GRAFIK SETELAH MA5 ----------
plt.figure(figsize=(10, 4))  # Membuat kanvas grafik MA5.
plt.plot(df.index, df["suhu"], marker="o", color="lightblue", alpha=0.6, label="Suhu (asli)")  # Menampilkan suhu asli sebagai pembanding.
plt.plot(df.index, df["suhu_ma5"], marker="o", color="blue", label="Suhu MA5")  # Menampilkan garis hasil moving average.
anom_ma = df[df["anomali_ma"]]  # Memilih baris yang terdeteksi anomali setelah MA5.
plt.scatter(anom_ma.index, anom_ma["suhu_ma5"], color="orange", s=100, label="Anomali MA5 (|z| > 2)", zorder=3)  # Menandai anomali MA5.
plt.axhline(rata2_ma, color="gray", linestyle="--", label="Rata-rata MA5")  # Menambahkan garis rata-rata MA5.
plt.xlabel("Urutan data")  # Memberi label sumbu horizontal.
plt.ylabel("Suhu (°C)")  # Memberi label sumbu vertikal.
plt.title("Data Suhu Setelah Moving Average 5 dan Deteksi Anomali")  # Memberi judul grafik MA5.
plt.legend()  # Menampilkan keterangan garis dan titik.
plt.tight_layout()  # Merapikan jarak antarelemen grafik.
plt.savefig("grafik_suhu_ma5.png")  # Menyimpan grafik hasil MA5 ke file PNG.
print("Grafik disimpan: grafik_suhu_ma5.png")  # Menampilkan nama file grafik yang disimpan.
plt.show()  # Menampilkan grafik MA5 di layar.

# ---------- TAHAP 6: SIMPAN CSV LENGKAP ----------
df.to_csv("data_suhu.csv", index=False)
print("CSV lengkap (dengan kolom z-score & MA5) tersimpan di data_suhu.csv")  # Mengonfirmasi bahwa hasil statistik dan MA5 telah disimpan.
print("Selesai.")
