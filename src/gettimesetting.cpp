#include "gettimesetting.h"

void getTimesetting(uint8_t &waktu1Awal, uint8_t &waktu1Akhir) {
    // Get data from SPIFFS and send it to global variable

    String inputWaktu = "";
    File file = LittleFS.open("time.csv", "r");

    if(!file){
    Serial.println("Failed to open file for reading");
    return;
    }

    while (file.available()) {
        inputWaktu += (char)file.read();
    }

    uint8_t pembatas1 = inputWaktu.indexOf("#");

    waktu1Awal = inputWaktu.substring(0, pembatas1).toInt();
    waktu1Akhir = inputWaktu.substring(pembatas1+1, inputWaktu.length()).toInt();

    file.close();
}