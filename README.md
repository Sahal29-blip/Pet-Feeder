# Pet-Feeder 🐾

Pet-Feeder adalah sistem pemberi makan otomatis berbasis ESP32 yang dirancang untuk membantu pemilik hewan peliharaan memberi makan secara terjadwal dan dapat dipantau dari jarak jauh. Sistem ini menggabungkan penggunaan sensor ultrasonik, motor servo, dan koneksi internet untuk pemantauan serta pengendalian melalui aplikasi **Blynk** dan notifikasi real-time via **Telegram**.

## 🔧 Fitur Utama

- 🎛 **Jadwal makan otomatis** (pagi, siang, sore)
- 📏 **Sensor ultrasonik** untuk mengukur level makanan
- 📲 **Kontrol manual dan pemantauan jarak jauh** melalui aplikasi Blynk
- 🤖 **Servo motor** untuk membuka penutup makanan secara otomatis
- 📡 **Notifikasi Telegram** saat makanan habis, hampir habis, atau saat feeding dilakukan
- 🌐 **Sinkronisasi waktu** menggunakan NTP (Network Time Protocol)

## ⚙️ Komponen yang Digunakan

- ESP32 DevKit V1
- Servo motor SG90
- Sensor Ultrasonik HC-SR04
- LED sebagai indikator WiFi
- Aplikasi Blynk (token & virtual pin terhubung)
- Bot Telegram (menggunakan token bot dan chat ID)

## 🧠 Cara Kerja Sistem

1. Saat sistem menyala, ia akan menyambung ke WiFi dan menampilkan status melalui Telegram, termasuk waktu dan level makanan.
2. Setiap beberapa detik, sistem mengukur level makanan dengan sensor ultrasonik dan mengirimkannya ke Blynk (V0 & V1).
3. Jika level makanan kurang dari batas minimum (5 cm), sistem akan mengirim notifikasi ke Telegram.
4. Pada jam-jam tertentu yang sudah ditentukan, sistem akan secara otomatis memberi makan selama durasi yang ditentukan.
5. Pengguna juga dapat mengatur jadwal dan durasi makan melalui Blynk (V10–V31) atau melakukan feeding manual (V40).
6. Bot Telegram juga mendukung perintah seperti `/status`, `/feed`, `/time`, dan `/schedule`.

## 📱 Tampilan Aplikasi Blynk

![Tampilan Blynk](https://github.com/Sahal29-blip/Pet-Feeder/raw/main/Tampilan%20Aplikasi%20Blynk.jpeg)

## 🧪 Contoh Perintah Telegram

- `/status` → Menampilkan status makanan dan waktu sistem
- `/feed` → Memberi makan manual selama 3 detik
- `/time` → Menampilkan waktu sistem
- `/schedule` → Menampilkan jadwal makan saat ini

## 📝 Konfigurasi Jadwal (Default)

| Waktu     | Jam  | Menit | Durasi |
|-----------|------|-------|--------|
| Pagi      | 08   | 00    | 3 detik |
| Siang     | 12   | 00    | 3 detik |
| Sore      | 19   | 30    | 3 detik |

Jadwal ini dapat diubah langsung melalui aplikasi Blynk.

## 📜 Lisensi

Proyek ini dikembangkan untuk keperluan edukasi dan tugas akhir pada mata kuliah Sistem Tertanam Cerdas, Universitas Muhammadiyah Malang. Dapat dimodifikasi dan dikembangkan lebih lanjut untuk kebutuhan pribadi.
