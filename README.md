# Pet-Feeder 🐾

Pet-Feeder adalah sebuah proyek IoT berbasis ESP32 yang dirancang untuk memberi makan hewan peliharaan secara otomatis dan cerdas. Proyek ini memanfaatkan servo motor untuk membuka tutup tempat makanan, serta sensor ultrasonik untuk mendeteksi ketinggian atau keberadaan makanan di wadah. Selain itu, pengguna juga dapat mengontrol dan memantau perangkat ini dari jarak jauh melalui aplikasi Blynk.

## 🎯 Tujuan Proyek

Membantu pemilik hewan peliharaan dalam memberikan makanan secara terjadwal atau manual tanpa harus selalu berada di rumah, sekaligus memonitor status tempat makanan secara real-time melalui smartphone.

## 🛠 Komponen yang Digunakan

- ESP32 DevKit V1
- Servo Motor (untuk membuka penutup makanan)
- Sensor Ultrasonik HC-SR04 (untuk deteksi ketinggian makanan)
- Modul Wi-Fi (bawaan ESP32)
- Aplikasi Blynk (Android/iOS)
- Power supply dan kabel jumper

## ⚙️ Cara Kerja

1. Saat tombol pada aplikasi Blynk ditekan, ESP32 akan mengaktifkan servo untuk membuka penutup makanan.
2. Sensor ultrasonik membaca apakah makanan sudah habis atau masih tersedia.
3. Status makanan akan dikirim ke aplikasi Blynk dalam bentuk notifikasi atau indikator.
4. Sistem juga dapat dikembangkan untuk memberi makan otomatis berdasarkan waktu tertentu.

## 📱 Tampilan Aplikasi Blynk

![Tampilan Blynk](https://github.com/Sahal29-blip/Pet-Feeder/raw/main/Tampilan%20Aplikasi%20Blynk.jpeg)

## 🔧 Instalasi dan Upload Kode

1. Buka file `.ino` menggunakan Arduino IDE.
2. Pilih board **ESP32 Dev Module**.
3. Install library yang dibutuhkan (contoh: `Blynk`, `Servo`, dll).
4. Upload kode ke ESP32 menggunakan kabel USB.
5. Sesuaikan token Blynk, Wi-Fi SSID, dan password di dalam kode.

## 📂 Struktur Folder

