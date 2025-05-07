#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>

TFT_eSPI tft = TFT_eSPI();
AsyncWebServer server(80);
DNSServer dnsServer;

const char* ssid = "EchoBOX";
const char* password = "";

// Include semua file header gambar
#include "img1.h"
#include "img2.h"
#include "img3.h" // Dan seterusnya sesuai kebutuhan
#include "icon.h" //love and bootlogo
// Buffer untuk render per blok
uint16_t blokBuffer[40 * 40];

// Fungsi untuk menampilkan gambar dari PROGMEM
void drawImageFromProgmem(const uint16_t* imageData) {
  for (int y = 0; y < 280; y += 40) {
    for (int x = 0; x < 240; x += 40) {
      
      // Ambil data dari PROGMEM ke buffer
      for (int by = 0; by < 40; by++) {
        for (int bx = 0; by < 40; bx++) {
          if ((x + bx < 240) && (y + by < 280)) {
            blokBuffer[by * 40 + bx] = pgm_read_word(&imageData[(y + by) * 240 + (x + bx)]);
          }
        }
      }

      // Tampilkan blok
      tft.pushImage(x, y, 40, 40, blokBuffer, TFT_RGB);
      delay(5); // Jeda untuk hindari crash
    }
  }
}

// Variabel global untuk mengingat gambar yang sedang ditampilkan
int currentImageIndex = 0;
const int totalImages = 3; // Jumlah total gambar yang tersedia

// Fungsi untuk mengganti gambar
void switchToNextImage() {
  tft.fillScreen(TFT_BLACK);
  currentImageIndex = (currentImageIndex + 1) % totalImages;
  
  // Pilih gambar berdasarkan index
  switch(currentImageIndex) {
    case 0:
      drawImageFromProgmem(gambar1);
      break;
    case 1:
      drawImageFromProgmem(gambar2);
      break;
    case 2:
        drawImageFromProgmem(gambar3);
    break;
    // Dan seterusnya...
  }
}

// Fungsi untuk menampilkan gambar love kecil di tengah layar
void showLoveIcon() {
  int y = (240 - 200) / 2;
  int x = (280 - 200) / 2; // agak ke atas biar muat teks di bawahnya
  tft.pushImage(x, y, 200, 200, love, TFT_RGB);
}
void booting() {
  int y = (240 - 200) / 2;
  int x = (280 - 200) / 2; // agak ke atas biar muat teks di bawahnya
  tft.pushImage(x, y, 200, 200, bootlogo, TFT_RGB);
}

// Fungsi untuk menampilkan teks dengan word wrap otomatis
void drawWrappedText(String msg, int x, int y, int maxWidth) {
  tft.setTextDatum(TL_DATUM);
  int cursorY = y;
  String line = "";
  String word = "";
  int lineHeight = tft.fontHeight();
  for (int i = 0; i <= msg.length(); i++) {
    char c = (i < msg.length()) ? msg[i] : ' ';
    if (c == ' ' || c == '\n' || i == msg.length()) {
      if (line.length() == 0) {
        line = word;
      } else {
        String testLine = line + " " + word;
        int w = tft.textWidth(testLine);
        if (w > maxWidth) {
          // Bersihkan area baris sebelum menulis
          tft.fillRect(x, cursorY, maxWidth, lineHeight, TFT_BLACK);
          tft.drawString(line, x, cursorY);
          cursorY += lineHeight;
          line = word;
        } else {
          line = testLine;
        }
      }
      if (c == '\n') {
        tft.fillRect(x, cursorY, maxWidth, lineHeight, TFT_BLACK);
        tft.drawString(line, x, cursorY);
        cursorY += lineHeight;
        line = "";
      }
      word = "";
    } else {
      word += c;
    }
  }
  if (line.length() > 0) {
    tft.fillRect(x, cursorY, maxWidth, lineHeight, TFT_BLACK);
    tft.drawString(line, x, cursorY);
  }
}

// Fungsi animasi love jedag-jedug 2 frame


// Fungsi animasi teks dengan word wrap, font 3, dan memenuhi satu layar
void showAnimatedText() {
  String msg = "haii, sebelum lanjut koneksikan dulu ke wifi EchoBOX";
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(4); // font ukuran 3
  int x = 10;
  int y = 10;
  int maxWidth = 270;
  for (int i = 1; i <= msg.length(); i++) {
    tft.fillRect(0, 0, 240, 280, TFT_BLACK); // clear seluruh layar
    drawWrappedText(msg.substring(0, i), x, y, maxWidth);
    delay(35);
  }
}

void setup() {
  Serial.begin(115200);
  
  // TFT setup
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  booting();
  delay(2000);
  tft.fillScreen(TFT_BLACK);
  showLoveIcon();
  delay(1000);
  tft.fillScreen(TFT_BLACK);
    showAnimatedText();
  
  // WiFi AP mode
  WiFi.softAP(ssid, password);
  delay(100);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  dnsServer.start(53, "*", myIP);

  // Tambahkan captive portal redirect
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("/");
  });

   // LittleFS
   if (!LittleFS.begin()) {
    Serial.println("Gagal mount LittleFS!");
    return;
  }
  Serial.println("LittleFS mounted successfully");
  
  // Web server setup untuk mengontrol gambar
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });
  server.on("/lagu.mp3", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/lagu.mp3", "audio/mpeg");
  });
  server.on("/cover.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/cover.jpg", "image/jpeg");
  });
  // Endpoint untuk mengganti gambar
  server.on("/next", HTTP_GET, [](AsyncWebServerRequest *request) {
    switchToNextImage();
    request->redirect("/");
  });
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
}
