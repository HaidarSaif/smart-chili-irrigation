# 🌶️ Smart Chili Irrigation

## 📖 Deskripsi Proyek
Sistem IoT terintegrasi hasil kerja sama dengan Kelompok Wanita Tani (KWT) Mawar di Kasihan, Bantul. Sistem ini ditujukan untuk memodernisasi cara bertani dan meringankan beban fisik para petani melalui otomatisasi perawatan lahan.

Beberapa titik sensor kelembapan tanah dihubungkan ke pusat kendali menggunakan komunikasi nirkabel ESP-NOW. Ketika tanah terdeteksi kering, sistem akan menyalakan pompa *submersible* dan instalasi *sprinkler* untuk menciptakan penyiraman berbasis embun. Semua aktuator ditenagai langsung (*direct*) menggunakan sistem PLTS *Off-Grid* tanpa mengandalkan baterai utama.

**Fitur Utama:**
*   Penyiraman tanaman otomatis berbasis data kelembapan tanah aktual.
*   Komunikasi multi-node hemat daya menggunakan ESP-NOW.
*   Sistem kelistrikan ramah lingkungan bertenaga PLTS *Off-Grid*.
*   Pencatatan *log* data ke Firebase untuk pemantauan aplikasi *mobile*.

## 🚀 Teknologi yang Digunakan
*   **Hardware:** ESP32, Pompa Air Submersible, Mini PLTS (Panel Surya), Sensor Kelembapan.
*   **Software:** C++, Protokol ESP-NOW, Firebase Realtime Database.

## 📂 Struktur Direktori
*   `/microcontroller` - Berisi kode untuk *Node Sensor* (Sender) dan *Node Gateway/Relay* (Receiver).
*   `/mobile_app` - Berisi file aplikasi mobile hasil ekspor dari MIT APP Inventor.
*   `/documentation` - Berisi foto implementasi di lahan, interaksi dengan KWT, dan topologi jaringan.
<img width="1376" height="784" alt="smart_irrigation (9)" src="https://github.com/user-attachments/assets/209acc5e-cd92-436c-9c24-4f6cef59597b" />
<img width="540" height="1200" alt="smart_irrigation (1)" src="https://github.com/user-attachments/assets/46cb69a6-1d16-4475-a6e3-07cf39b7d1b4" />
