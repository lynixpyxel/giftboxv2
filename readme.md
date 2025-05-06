<h1 align="center">🎁 GiftBox V2</h1>
<p align="center">
  Proyek mikrokontroler untuk menampilkan gambar romantis di layar kecil.  
  Cocok buat ngasih kejutan digital ke doi... atau ke diri sendiri 😌.
</p>

---

## 🔧 Tentang Proyek

**GiftBox V2** dibuat menggunakan mikrokontroler ESP8266/ESP32 dan layar TFT ST7789 1,69 inch menggunakan lib TFT_eSPI. Proyek ini akan menampilkan beberapa gambar secara otomatis, ideal untuk kado digital atau tampilan aesthetic.

---

## 🚀 Cara Instal

```bash
git clone https://github.com/lynixpyxel/giftboxv2.git
```
- Langkah-langkah:
    - ESP32
        
        Buka giftboxv2.ino di Arduino IDE.

        Pastikan semua file header gambar (img1.h, img2.h, img3.h) ada di satu folder dengan file .ino.

        Sambungkan board ESP32.
        
        Pilih board dan port yang sesuai di Arduino IDE.

        Upload sketch ke board lo.

        Sketch ini pakai filesystem LittleFS, jadi lakukan upload folder data/:

        Install plugin 🔗 [ESP32 Sketch Data Upload](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) buat upload data/


        Klik Tools > ESP32 Sketch Data Upload.
    - ESP8266
        
        Buka giftboxv2.ino di Arduino IDE.

        Pastikan semua file header gambar (img1.h, img2.h, img3.h) ada di satu folder dengan file .ino.

        Sambungkan board ESP8266.
        
        Pilih board dan port yang sesuai di Arduino IDE.

        Upload sketch ke board lo.

        Sketch ini pakai filesystem LittleFS, jadi lakukan upload folder data/:

        Install plugin 🔗 [ESP8266 LittleFS Data Upload](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin) buat upload data/


        Klik Tools > ESP8266 Sketch Data Upload.


---
## 🖼️ Cara Nambah Gambar Sendiri

Convert gambar lo ke array C lewat [Image2CPP](https://javl.github.io/image2cpp/)

Gunakan format RGB565 untuk TFT ST7789

Simpan hasil convert sebagai file header (imgX.h).

Tambahkan #include "imgX.h" di file .ino.

Tambahkan logic tampil gambar di bagian loop atau fungsi khusus.

---
## Note
* Jika menggunakan ESP32 gambar bisa diload lebih cepat