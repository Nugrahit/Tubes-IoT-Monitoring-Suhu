# Sistem Monitoring Suhu Kamar Berbasis ESP32

Repositori ini dibuat untuk memenuhi Tugas Besar Mata Kuliah Internet of Things (AAK4WBB3) Telkom University Purwokerto. 
Repositori ini berisi:
1. Kode program (C/C++) pembacaan sensor DHT11 menggunakan ESP32 (Bagian A).
2. Data Log Eksperimen Protokol (Bagian B).

## Perangkat Keras
- Mikrokontroler: ESP32
- Sensor Suhu: DHT11
- Tampilan: OLED SH1106 I2C (128x64)

## Dependensi Library
Untuk menjalankan kode `.ino`, pastikan telah menginstal *library* berikut di Arduino IDE:
1. `WiFi` dan `Wire` (bawaan board ESP32)
2. `ThingSpeak` (oleh MathWorks)
3. `DHT sensor library` (oleh Adafruit)
4. `U8g2` (oleh oliver)

## Cara Menjalankan Kode
1. Buka kode program `.ino` menggunakan Arduino IDE.
2. Ganti nilai variabel kredensial pada bagian atas kode:
   - `ssid` dan `password`: dengan kredensial jaringan Wi-Fi Anda.
   - `channelID` dan `apiKey`: dengan identitas ThingSpeak Anda.
3. Hubungkan ESP32 ke komputer, pilih board ESP32 Dev Module, dan *port* yang sesuai.
4. Klik *Upload*. ESP32 akan membaca suhu dan mengirimkannya setiap 30 detik.
