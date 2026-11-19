#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include <Wire.h>
#include <U8g2lib.h>

// ---- Wifi rumah ----
const char* ssid     = "NU";
const char* password = "11111111";

// ---- Thingspeak ----
unsigned long channelID = 3568396;
const char*   apiKey    = "MR4A5G8IM0EZQB42";

// ---- Sensor DHT11 di pin 4 ----
#define pinDHT 4
#define tipeDHT DHT11
DHT dht(pinDHT, tipeDHT);

// ---- OLED SH1106 ----
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

WiFiClient net;

void setup() {
  Serial.begin(115200);
  dht.begin();
  oled.begin();

  // tampilan awal waktu alat dinyalakan
  oled.clearBuffer();
  oled.drawRFrame(0, 0, 128, 64, 5);
  oled.setFont(u8g2_font_ncenB08_tr);
  oled.drawStr(15, 35, "Sistem Menyala...");
  oled.sendBuffer();

  // sambung wifi
  Serial.print("\nMenghubungkan ke WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");

  ThingSpeak.begin(net);
}

void loop() {
  // ---- kalau wifi putus, coba sambung lagi dulu ----
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Terputus! Mencoba menghubungkan kembali...");

    oled.clearBuffer();
    oled.drawRFrame(0, 0, 128, 64, 5);
    oled.setFont(u8g2_font_ncenB08_tr);
    oled.drawStr(22, 30, "WiFi Terputus!");
    oled.drawStr(15, 45, "Menghubungkan...");
    oled.sendBuffer();

    WiFi.disconnect();
    WiFi.reconnect();

    int coba = 0;
    while (WiFi.status() != WL_CONNECTED && coba < 10) {
      delay(1000);
      coba++;
    }
    if (WiFi.status() != WL_CONNECTED) return;
  }

  // ---- baca suhu ----
  float suhu = dht.readTemperature();

  if (isnan(suhu)) {
    Serial.println("Gagal membaca sensor DHT11!");
    oled.clearBuffer();
    oled.drawRFrame(0, 0, 128, 64, 5);
    oled.setFont(u8g2_font_ncenB08_tr);
    oled.drawStr(10, 35, "Error Sensor DHT!");
    oled.sendBuffer();
    delay(2000);
    return;
  }

  String ketSerial = "";
  String ketLayar  = "";

  // ---- tentukan kondisi berdasarkan suhu ----
  if (suhu >= 29.5) {
    ketSerial = "Normal (Kamar Tertutup Tanpa AC)";
    ketLayar  = "Normal";
    ThingSpeak.setField(1, suhu);
  }
  else if (suhu >= 27.0 && suhu < 29.5) {
    ketSerial = "Pintu/Jendela Terbuka";
    ketLayar  = "Jendela/Pintu Terbuka";
    ThingSpeak.setField(3, suhu);
  }
  else {
    ketSerial = "AC Menyala (Suhu Dingin)";
    ketLayar  = "AC Menyala";
    ThingSpeak.setField(2, suhu);
  }

  // ---- tampilkan di oled ----
  oled.clearBuffer();

  // bingkai
  oled.drawRFrame(0, 0, 128, 64, 4);

  // judul
  oled.setFont(u8g2_font_ncenB08_tr);
  oled.setCursor(6, 12);
  oled.print("Kamar Nugrah");

  // ikon sinyal wifi (3 bar)
  oled.drawBox(108, 9, 2, 4);
  oled.drawBox(112, 6, 2, 7);
  oled.drawBox(116, 3, 2, 10);

  // garis pemisah
  oled.drawLine(0, 16, 128, 16);

  // angka suhu
  oled.setFont(u8g2_font_fub14_tr);
  oled.setCursor(25, 40);
  oled.print(suhu, 1);
  oled.print(" \xb0" "C");

  // keterangan status
  oled.setFont(u8g2_font_helvB08_tr);
  oled.setCursor(6, 54);
  oled.print(ketLayar);

  oled.sendBuffer();

  // ---- kirim ke thingspeak ----
  Serial.print("Suhu kamar saat ini : ");
  Serial.print(suhu);
  Serial.println(" *C");
  Serial.print("Status Ruangan      : ");
  Serial.println(ketSerial);

  ThingSpeak.setStatus(ketSerial);
  int hasil = ThingSpeak.writeFields(channelID, apiKey);

  if (hasil == 200) {
    Serial.println("Data SUKSES dikirim ke grafik ThingSpeak!");
  } else {
    Serial.println("GAGAL mengirim data. Kode Error: " + String(hasil));
  }

  // ---- progress bar 30 detik ----
  Serial.println("Menunggu 30 detik...\n");

  for (int detik = 0; detik <= 30; detik++) {
    int panjangBar = map(detik, 0, 30, 0, 118);
    oled.setDrawColor(1);
    oled.drawBox(5, 58, panjangBar, 3);
    oled.sendBuffer();
    delay(1000);
  }
}