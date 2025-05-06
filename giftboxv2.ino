#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>

TFT_eSPI tft = TFT_eSPI();
AsyncWebServer server(80);
DNSServer dnsServer;

const char* ssid = "RomanticAP";
const char* password = "";

// Teknik 1: Pisahkan gambar ke file header terpisah
// File: image1.h
// -------------------------------------------
// #ifndef IMAGE1_H
// #define IMAGE1_H
// const uint16_t gambar1[240 * 280] PROGMEM = {
//   0xef5c, 0xef5c, ... // data gambar 1
// };
// #endif
// -------------------------------------------

// File: image2.h
// -------------------------------------------
// #ifndef IMAGE2_H
// #define IMAGE2_H
// const uint16_t gambar2[240 * 280] PROGMEM = {
//   0xf123, 0xf123, ... // data gambar 2
// };
// #endif
// -------------------------------------------

// Include semua file header gambar
#include "img1.h"
#include "img2.h"
#include "img3.h" // Dan seterusnya sesuai kebutuhan

// Buffer untuk render per blok
uint16_t blokBuffer[40 * 40];

// Fungsi untuk menampilkan gambar dari PROGMEM
void drawImageFromProgmem(const uint16_t* imageData) {
  for (int y = 0; y < 280; y += 40) {
    for (int x = 0; x < 240; x += 40) {
      
      // Ambil data dari PROGMEM ke buffer
      for (int by = 0; by < 40; by++) {
        for (int bx = 0; bx < 40; bx++) {
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

void setup() {
  Serial.begin(115200);
  
  // TFT setup
  tft.begin();
  tft.setRotation(4);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  
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
  
  // Tampilkan gambar pertama
  drawImageFromProgmem(gambar1);
  
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
  
  // Jika menggunakan file MP3 dari SPIFFS/LittleFS
  // server.on("/lagu.mp3", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(LittleFS, "/lagu.mp3", "audio/mpeg");
  // });
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
}
