# 🌪️ Pendeteksi Angin Kencang untuk Sistem Penutup Otomatis Berbasis ESP32 dan ThingSpeak

Proyek Akhir Mata Kuliah Sistem Mikrokontroler

|  |  |
|------------|---------|
| Nama | Sandhy Safarudin Nurdiansyah |
| NIM | 23552011464 |
| Dosen Pengampu | Muchamad Rusdan, S.T., M.T. |

---

# 📖 Deskripsi Proyek

Proyek ini merupakan sistem monitoring dan otomatisasi yang dirancang untuk mendeteksi kondisi angin kencang dan melakukan tindakan perlindungan secara otomatis.

Sistem menggunakan ESP32 sebagai mikrokontroler utama untuk membaca data sensor angin yang disimulasikan menggunakan potentiometer pada Wokwi. Data yang diperoleh kemudian diproses untuk menentukan kondisi lingkungan berdasarkan tingkat kecepatan angin.

Apabila kecepatan angin melebihi batas aman yang telah ditentukan, sistem akan secara otomatis menutup pelindung menggunakan servo motor, mengaktifkan alarm buzzer, menyalakan indikator bahaya, dan mengirimkan data monitoring ke platform IoT ThingSpeak secara real-time.

---

# 🌱 Latar Belakang

Kondisi cuaca yang tidak menentu sering menyebabkan kerusakan pada area pertanian, greenhouse, maupun sistem pelindung luar ruangan.

Salah satu faktor yang dapat menimbulkan kerusakan adalah angin kencang yang dapat merobohkan tanaman, merusak penutup greenhouse, atau mengganggu peralatan yang berada di area terbuka.

Dengan memanfaatkan teknologi Internet of Things (IoT), sistem dapat melakukan pemantauan kondisi lingkungan secara real-time dan mengambil tindakan otomatis tanpa memerlukan intervensi pengguna secara langsung.

---

# 🎯 Tujuan Proyek

Tujuan dari proyek ini adalah:

- Mendeteksi kondisi angin berdasarkan data sensor.
- Mengklasifikasikan kondisi menjadi Aman, Waspada, dan Bahaya.
- Mengendalikan sistem penutup otomatis menggunakan servo motor.
- Memberikan notifikasi visual menggunakan LED indikator.
- Memberikan peringatan suara menggunakan buzzer.
- Mengirimkan data monitoring ke ThingSpeak secara real-time.
- Mengimplementasikan konsep IoT menggunakan ESP32 dan protokol HTTP.

---

# ⚙️ Cara Kerja Sistem

1. Sensor membaca nilai kecepatan angin.
2. ESP32 memproses nilai sensor.
3. Sistem menentukan status kondisi angin.
4. Aktuator merespons sesuai kondisi:
   - LED Hijau → Aman
   - LED Kuning → Waspada
   - LED Merah → Bahaya
5. Servo menutup pelindung saat status Bahaya.
6. Buzzer aktif sebagai alarm.
7. Data dikirim ke ThingSpeak melalui koneksi WiFi menggunakan protokol HTTP.

---

# 🧩 Komponen yang Digunakan

## Hardware

- ESP32 DevKit V1
- Potentiometer (Simulasi Sensor Angin)
- Servo SG90
- LED Hijau
- LED Kuning
- LED Merah
- Resistor 220Ω
- Buzzer

## Software

- Wokwi Simulator
- Arduino Framework
- ThingSpeak
- GitHub

---

# 🔌 Konfigurasi Pin

| Komponen | GPIO ESP32 |
|-----------|------------|
| Sensor Angin (Potentiometer) | GPIO34 |
| Servo Motor | GPIO18 |
| LED Hijau | GPIO14 |
| LED Kuning | GPIO12 |
| LED Merah | GPIO13 |
| Buzzer | GPIO27 |

---

# 🚦 Logika Status Sistem

| Kecepatan Angin | Status |
|-----------------|---------|
| < 25 km/jam | Aman |
| 25 - 34 km/jam | Waspada |
| ≥ 35 km/jam | Bahaya |

## Status Aman

- LED Hijau Menyala
- Servo Terbuka
- Buzzer Mati

## Status Waspada

- LED Kuning Menyala
- Servo Tetap Terbuka
- Buzzer Mati

## Status Bahaya

- LED Merah Menyala
- Servo Menutup Pelindung
- Buzzer Aktif

---

# 🌐 Implementasi IoT

Sistem menggunakan:

- ESP32 sebagai perangkat IoT
- WiFi sebagai media komunikasi
- HTTP sebagai protokol komunikasi
- ThingSpeak sebagai platform monitoring data

Data yang dikirim ke ThingSpeak:

| Field | Data |
|---------|---------|
| Field 1 | Kecepatan Angin |
| Field 2 | Status Sistem |
| Field 3 | Posisi Servo |
| Field 4 | Status Alarm |

---

# ✨ Fitur Utama

✅ Monitoring kecepatan angin secara real-time

✅ Klasifikasi kondisi lingkungan otomatis

✅ Sistem penutup otomatis menggunakan servo

✅ Alarm buzzer saat kondisi bahaya

✅ Indikator LED status sistem

✅ Monitoring jarak jauh menggunakan ThingSpeak

✅ Berbasis Internet of Things (IoT)

---

# 📸 Dokumentasi

## Rangkaian Wokwi

Tambahkan screenshot rangkaian di sini.

<img width="1912" height="1026" alt="image" src="https://github.com/user-attachments/assets/95f1efee-01a3-4996-88f5-157b102448a6" />

## Dashboard ThingSpeak

Tambahkan screenshot dashboard ThingSpeak.

<img width="1915" height="1024" alt="image" src="https://github.com/user-attachments/assets/7a8e4d4c-d51a-42df-8902-14f0ce32321e" />

---

# 🎥 Video Demonstrasi

Video demonstrasi proyek dapat dilihat pada tautan berikut:

📺 YouTube:

```text
https://youtu.be/NgagH5j3NZs
```

---

# 💻 Source Code

Repository ini berisi:

- Source code ESP32
- Dokumentasi proyek
- Screenshot hasil simulasi
- Dokumentasi IoT ThingSpeak

---

# 🔗 Tautan Penting

## Wokwi Project

```text
https://wokwi.com/projects/467678500448347137
```

## ThingSpeak Dashboard

```text
https://thingspeak.mathworks.com/channels/3415457
```

---

# 📚 Referensi

- Dokumentasi ESP32 Arduino
- Dokumentasi Wokwi Simulator
- Dokumentasi ThingSpeak
- Modul Praktikum Sistem Mikrokontroler Universitas Teknologi Bandung

---

# 📝 Kesimpulan

Proyek Pendeteksi Angin Kencang untuk Sistem Penutup Otomatis berhasil mengimplementasikan konsep mikrokontroler dan Internet of Things menggunakan ESP32.

Sistem mampu membaca data sensor, menentukan tingkat bahaya berdasarkan kecepatan angin, mengendalikan aktuator secara otomatis, serta mengirimkan data monitoring ke platform ThingSpeak secara real-time.

Melalui proyek ini, konsep integrasi sensor, aktuator, komunikasi jaringan, dan monitoring IoT dapat diterapkan dalam sebuah sistem otomatis yang sederhana namun relevan untuk kebutuhan pertanian cerdas (Smart Agriculture).

---

> Proyek Akhir Sistem Mikrokontroler
>
> Universitas Teknologi Bandung
